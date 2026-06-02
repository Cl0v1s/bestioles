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

## DNA

A pet's DNA is its `Shape`: the combination of its bodypart slots (which types of limbs it *can* have) and its bodyparts (which limbs it *actually* has). Two pets with the same slots but different bodyparts assigned look different, and two pets with different slot configurations are a different species.

## Pet body

### Bodyparts

A `Bodypart` is a visual piece of the body. It has:

| Field | Type | Role |
|-------|------|------|
| `type` | `BodypartType` | Category of the piece |
| `filename` | `const char*` | Associated image (string literal) |
| `anchor` | `Point` | Attachment point within the image |
| `revert` | `bool` | Horizontal mirror |

Available types are defined by `BodypartType` (8 values max, `BODYPARTTYPE_COUNT = 8`):

`NONE`, `HAT`, `EYES`, `MOUTH`, `ARM1`, `ARM2`, `LEG1`, `LEG2`, `TAIL`

All bodyparts are declared in the global `BODYPARTS` array (`data/bodyparts.h`). Index `0` is always reserved for "no bodypart" (`NONE`). Bodyparts are referenced by their index in this array (never by pointer) so that shapes remain serializable across builds.

### Shape

A `Shape` describes the body template of a pet: which bodypart **slots** it has and which bodypart occupies each one.

- It holds up to `BODYPARTTYPE_COUNT` slots, each typed (`anchors[]`).
- Each slot is either empty (`parts[i] == 0`) or filled with the index of a bodypart from `BODYPARTS`.
- `count` is the number of slots this shape actually uses.

**Assignment rules:**

- `add(id)` places bodypart `id` into the first free slot of the matching type. Returns `1` if no free slot of that type exists.
- `remove(type)` clears the first occupied slot of the given type.
- A shape can have multiple slots of the same type (e.g. `ARM1` and `ARM2`), but a bodypart can only occupy one slot at a time.

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
