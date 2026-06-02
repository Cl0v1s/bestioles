#pragma once 

#include "../utils/point.h"

#define BODYPARTTYPE_COUNT 8

enum class BodypartType {
    NONE = 0,
    HAT = 1,
    EYES = 2,
    MOUTH = 3,
    ARM1 = 4,
    ARM2 = 5,
    LEG1 = 6,
    LEG2 = 7,
    TAIL = 8,
};

class Bodypart {
    private:
        const char *filename;
        Point anchor;
    public: 
        BodypartType type;
        bool revert;

        Bodypart(): anchor(0,0) {
            reset();
        }

        /** f must be a string literal ! */
        Bodypart(BodypartType t, char* f, Point a): type(t), filename(f), anchor(a), revert(false) {}

        void reset() {
            type = BodypartType::NONE;
            filename = 0;
            anchor = Point(0,0);
            revert = false;
        }
};

