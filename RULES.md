# Game Rules

## The pet

The pet has three stats, each between 0 and 100 (starting at 100):

| Stat | Meaning |
|------|---------|
| `happy` | Happiness |
| `strain` | Energy (high = rested, low = exhausted) |
| `hungry` | Hunger (high = hungry) |

## Sleep

The pet follows a sleep schedule defined by a bedtime and a wake time.

- **Overnight** (bedtime after wake time, e.g. 22:00→8:00): the pet sleeps from bedtime through midnight until wake time.
- **Nap** (bedtime before wake time, e.g. 10:00→14:00): the pet sleeps over a continuous window within the same day.

### Forced sleep

Two situations put the pet to sleep regardless of its schedule:

- **Light off** (`light = false`): the player puts the pet to bed manually.
- **Exhaustion** (`strain = 0`): the pet collapses — a `STRAIN` care miss is created.

Forced sleep ends when `strain` reaches 75 or above.

### Sleeping with the light on

If the pet falls asleep while the light is on (`light = true`), a `LIGHT` care miss is created.

## Care misses

A care miss is an event that requires the player to react within **15 minutes**. Past that deadline, it applies automatically and penalises the pet.

| Type | Cause | Effect if ignored |
|------|-------|-------------------|
| `HUNGRY` | Hunger neglected | `hungry` reset to 100, `happy` −25 |
| `STRAIN` | Total exhaustion | `strain` reset to 100, `happy` −25 |
| `POOP` | Poop not cleaned | `happy` −25 |
| `LIGHT` | Sleeping with light on | `happy` −10 |

The pet can accumulate up to **5 simultaneous care misses**. Reaching this limit causes death.

When a care miss is created, the device (`Device`) emits an alert.
