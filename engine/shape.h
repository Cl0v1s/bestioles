#pragma once 

#include "bodypart.h"
#include "../data/bodyparts.h"

class Shape {
    private:
        // will always be the max number of bodyparts even if there is less
        BodypartType anchors[BODYPARTTYPE_COUNT];
        // we dont want to use pointers here because we want to be able to keep Shapes serialized ACROSS BUILDS
        unsigned int parts[BODYPARTTYPE_COUNT];
        unsigned int count;

    public:
        Shape(): Shape(0, 0) {}

        /**
         * len must be <= BODYPARTTYPE_COUNT
         */
        Shape(const BodypartType* a, unsigned int len) {
            count = len;
            int i = 0;
            for(i = 0; i < len; i++) {
                anchors[i] = a[i];
                parts[i] = 0;
            }
            for(; i < BODYPARTTYPE_COUNT; i++) {
                anchors[i] = BodypartType::NONE;
                parts[i] = 0;
            }
        }

        /**
         * Must be only used with BODYPARTS array
         */
        unsigned char add(Bodypart* part) {
            unsigned int i = 0;
            while(part != &BODYPARTS[i] && i < BODYPARTS_LENGTH) {
                i++;
            }
            if(i >= BODYPARTS_LENGTH) return 1;
            return add(i);
        }

        unsigned char add(unsigned int id) {
            unsigned int i = 0;
            while(!(anchors[i] == BODYPARTS[id].type && parts[i] == 0) && i < count) {
                i++;
            }
            // we didnt find any free spot of type
            if(i >= count) {
                return 1;
            }
            parts[i] = id;
            return 0;
        }

        void remove(BodypartType type) {
            unsigned int i = 0;
            while(BODYPARTS[parts[i]].type != type && i < count) {
                i++;
            }
            // we didnt find any free spot of type
            if(i >= count) {
                return;
            }
            parts[i] = 0;
        }
};