#pragma once 

#include "../engine/bodypart.h"

#define BODYPARTS_LENGTH 2

// Put bodyparts declarations here 
static inline Bodypart BODYPARTS[] = {
    { BodypartType::NONE, "", { 0, 0} }, // NO BODYPART
    { BodypartType::LEG1, "test.png", { 10, 10} },
};