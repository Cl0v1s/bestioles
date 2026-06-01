#include <iostream>
#include "../utils/memory.h"
#include "../utils/string.h"

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

// --- memory ---

static void testMallocBasic() {
    pool_reset();
    Ptr* p = pool_alloc(10);
    check(p != 0,          "pool_alloc: returns non-null");
    check(p->size == 10,   "pool_alloc: size is correct");
    check(p->data != 0,    "pool_alloc: data pointer is non-null");
}

static void testMallocZero() {
    pool_reset();
    check(pool_alloc(0) == 0, "pool_alloc: size 0 returns null");
}

static void testMallocPoolFull() {
    pool_reset();
    Ptr* p = pool_alloc(POOL_SIZE);
    check(p != 0,           "pool_alloc: exactly fills pool");
    check(pool_alloc(1) == 0,   "pool_alloc: null when pool full");
}

static void testMallocPtrsFull() {
    pool_reset();
    for (int i = 0; i < PTR_SIZE; i++) pool_alloc(1);
    check(pool_alloc(1) == 0, "pool_alloc: null when ptr slots full");
}

static void testFreeAndReallocate() {
    pool_reset();
    Ptr* p = pool_alloc(10);
    check(p != 0, "free: initial alloc succeeds");
    pool_free(p);
    check(poolIndex == 0 && ptrsIndex == 0, "free: pool compacted after freeing only block");
    Ptr* p2 = pool_alloc(10);
    check(p2 != 0, "free: can reallocate after free");
}

static void testFreeMiddleNoCompact() {
    pool_reset();
    Ptr* a = pool_alloc(4);
    Ptr* b = pool_alloc(4);
    pool_alloc(4);  // c, last
    (void)a;
    pool_free(b);
    check(ptrsIndex == 3, "free: middle free leaves ptrsIndex unchanged");
    check(poolIndex == 12, "free: middle free doesn't reclaim pool");
}

static void testFreeLastThenMiddle() {
    pool_reset();
    Ptr* a  = pool_alloc(4);
    Ptr* b  = pool_alloc(4);
    Ptr* c  = pool_alloc(4);
    (void)a;
    pool_free(b);       // middle: marked, not compacted
    pool_free(c);       // last: compacts c then b
    check(ptrsIndex == 1, "free: compacts last + adjacent freed middle");
    check(poolIndex == 4, "free: reclaims pool for both");
}

// --- string ---

static void testStrlen() {
    check(strlen("hello") == 5, "strlen: \"hello\" == 5");
    check(strlen("") == 0,      "strlen: empty == 0");
    check(strlen("a") == 1,     "strlen: single char == 1");
    check(strlen(0) == 0,       "strlen: null == 0");
}

static void testStringDefault() {
    pool_reset();
    String s;
    check(s.length() == 0, "String: default constructor length == 0");
}

static void testStringFromLiteral() {
    pool_reset();
    String s("hello");
    check(s.length() == 5, "String: \"hello\" length == 5");
}

static void testStringEmptyLiteral() {
    pool_reset();
    String s("");
    check(s.length() == 0, "String: empty literal length == 0");
}

int main() {
    std::cout << "=== testMallocBasic ===\n";
    testMallocBasic();

    std::cout << "\n=== testMallocZero ===\n";
    testMallocZero();

    std::cout << "\n=== testMallocPoolFull ===\n";
    testMallocPoolFull();

    std::cout << "\n=== testMallocPtrsFull ===\n";
    testMallocPtrsFull();

    std::cout << "\n=== testFreeAndReallocate ===\n";
    testFreeAndReallocate();

    std::cout << "\n=== testFreeMiddleNoCompact ===\n";
    testFreeMiddleNoCompact();

    std::cout << "\n=== testFreeLastThenMiddle ===\n";
    testFreeLastThenMiddle();

    std::cout << "\n=== testStrlen ===\n";
    testStrlen();

    std::cout << "\n=== testStringDefault ===\n";
    testStringDefault();

    std::cout << "\n=== testStringFromLiteral ===\n";
    testStringFromLiteral();

    std::cout << "\n=== testStringEmptyLiteral ===\n";
    testStringEmptyLiteral();

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
