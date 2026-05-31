#include <iostream>
#include "../engine/pet.h"
#include "../engine/world.h"
#include "../engine/device.h"

static int passed = 0;
static int failed = 0;

static void check(bool condition, const char* name) {
    if (condition) {
        std::cout << "[PASS] " << name << "\n";
        passed++;
    } else {
        std::cout << "[FAIL] " << name << "\n";
        failed++;
    }
}

class TestDevice: public Device {
    void alert() override {}
};

// zero bonuses/maluses so manageStats() never changes happy during tests
static StateModifier noopModifier = {0, 0, 0, 0, 0, 0};

static void testOvernightSleep() {
    // sleep at 22:00, wake at 08:00
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(22, 0, 8, 0);

    world.setTime(22 * 3600);
    pet.update();
    check(pet.getSleeping(), "overnight: sleeping at 22:00 (sleep start)");

    world.setTime(23 * 3600);
    pet.update();
    check(pet.getSleeping(), "overnight: sleeping at 23:00");

    world.setTime(0);
    pet.update();
    check(pet.getSleeping(), "overnight: sleeping at 00:00 (midnight)");

    world.setTime(3 * 3600);
    pet.update();
    check(pet.getSleeping(), "overnight: sleeping at 03:00");

    world.setTime(8 * 3600 - 1);
    pet.update();
    check(pet.getSleeping(), "overnight: sleeping at 07:59:59");

    world.setTime(8 * 3600);
    pet.update();
    check(!pet.getSleeping(), "overnight: awake at 08:00 (wake time)");

    world.setTime(10 * 3600);
    pet.update();
    check(!pet.getSleeping(), "overnight: awake at 10:00");

    world.setTime(22 * 3600 - 1);
    pet.update();
    check(!pet.getSleeping(), "overnight: awake at 21:59:59");
}

static void testDayNap() {
    // sleep at 10:00, wake at 14:00
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(10, 0, 14, 0);

    world.setTime(8 * 3600);
    pet.update();
    check(!pet.getSleeping(), "nap: awake at 08:00 (before nap)");

    world.setTime(10 * 3600);
    pet.update();
    check(pet.getSleeping(), "nap: sleeping at 10:00 (nap start)");

    world.setTime(12 * 3600);
    pet.update();
    check(pet.getSleeping(), "nap: sleeping at 12:00 (during nap)");

    world.setTime(14 * 3600 - 1);
    pet.update();
    check(pet.getSleeping(), "nap: sleeping at 13:59:59");

    world.setTime(14 * 3600);
    pet.update();
    check(!pet.getSleeping(), "nap: awake at 14:00 (wake time)");

    world.setTime(16 * 3600);
    pet.update();
    check(!pet.getSleeping(), "nap: awake at 16:00 (after nap)");
}

static void testTimeWrapping() {
    // multi-day timestamps should wrap correctly
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(22, 0, 8, 0);

    // 2 full days + 23h
    world.setTime(2 * 24 * 3600 + 23 * 3600);
    pet.update();
    check(pet.getSleeping(), "wrap: sleeping at 23:00 (day 3)");

    // 3 full days + 10h
    world.setTime(3 * 24 * 3600 + 10 * 3600);
    pet.update();
    check(!pet.getSleeping(), "wrap: awake at 10:00 (day 4)");
}

static void testSleepWithMinutes() {
    // sleep at 22:30, wake at 07:45
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(22, 30, 7, 45);

    world.setTime(22 * 3600 + 29 * 60);
    pet.update();
    check(!pet.getSleeping(), "minutes: awake at 22:29");

    world.setTime(22 * 3600 + 30 * 60);
    pet.update();
    check(pet.getSleeping(), "minutes: sleeping at 22:30");

    world.setTime(7 * 3600 + 44 * 60);
    pet.update();
    check(pet.getSleeping(), "minutes: sleeping at 07:44");

    world.setTime(7 * 3600 + 45 * 60);
    pet.update();
    check(!pet.getSleeping(), "minutes: awake at 07:45");
}

static void testForceSleepLightOff() {
    // strain=50 keeps the pet below the 75 threshold so forced sleep is not cleared by the strain check
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(22, 0, 8, 0);
    pet.setStrain(50);

    world.setTime(10 * 3600);
    world.light = true;
    pet.update();
    check(!pet.getSleeping(), "light: awake at 10:00 with light on");

    world.light = false;
    pet.update();
    check(pet.getSleeping(), "light: sleeping when light turned off");

    world.light = true;
    pet.update();
    check(pet.getSleeping(), "light: still forced sleep after light on (strain < 75)");

    pet.setStrain(100);
    pet.update();
    check(!pet.getSleeping(), "light: awake after energy recovery");
}

static void testForceSleepExhaustion() {
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(22, 0, 8, 0);

    world.setTime(10 * 3600);
    world.light = true;
    pet.update();
    check(!pet.getSleeping(), "exhaustion: awake before exhaustion");

    pet.setStrain(0);
    pet.update();
    check(pet.getSleeping(), "exhaustion: sleeping when strain hits 0");

    pet.setStrain(50);
    pet.update();
    check(pet.getSleeping(), "exhaustion: still sleeping at strain 50");

    pet.setStrain(100);
    pet.update();
    check(!pet.getSleeping(), "exhaustion: awake after full recovery");
}

static void testCaremissLightPenalty() {
    // sleeping with light on creates a LIGHT care miss; ignoring it for 15 min costs 10 happy
    World world;
    TestDevice device;
    Pet pet(world, device, noopModifier);
    pet.setSleepSchedule(10, 0, 14, 0);

    world.setTime(12 * 3600);
    world.light = true;
    pet.update(); // sleeping + light on → LIGHT care miss created at t=12*3600

    check(pet.getHappy() == 100, "light penalty: happy still 100 before delay");

    world.setTime(12 * 3600 + CAREMISS_DELAY + 1);
    pet.update(); // care miss applies → happy 100-10=90

    check(pet.getHappy() == 90, "light penalty: happy reduced to 90 after delay");
}

int main() {
    std::cout << "=== testOvernightSleep ===\n";
    testOvernightSleep();

    std::cout << "\n=== testDayNap ===\n";
    testDayNap();

    std::cout << "\n=== testTimeWrapping ===\n";
    testTimeWrapping();

    std::cout << "\n=== testSleepWithMinutes ===\n";
    testSleepWithMinutes();

    std::cout << "\n=== testForceSleepLightOff ===\n";
    testForceSleepLightOff();

    std::cout << "\n=== testForceSleepExhaustion ===\n";
    testForceSleepExhaustion();

    std::cout << "\n=== testCaremissLightPenalty ===\n";
    testCaremissLightPenalty();

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
