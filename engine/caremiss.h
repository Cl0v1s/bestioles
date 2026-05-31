#pragma once 

#include "pet.h"

#define CAREMISS_HUNGRY 0x01
#define CAREMISS_STRAIN 0x02
#define CAREMISS_LIGHT 0x03
#define CAREMISS_POOP 0x04

class Caremiss {
    private:
        bool resolved = false;

    public: 
        unsigned char type;
        /**
         * Datetime of when it occured
         */
        unsigned int createdAt;

        Caremiss() = default;

        Caremiss(unsigned char type, unsigned int createdAt) {
            this->type = type;
            this->createdAt = createdAt;
        }

        bool getResolved() const {
            return resolved;
        }

        void apply(Pet* pet) {
            if(resolved) return;
            switch(type) {
                case CAREMISS_HUNGRY: {
                    pet->setHungry(100);
                    pet->setHappy(pet->getHappy() - 25);
                    break;
                }
                case CAREMISS_STRAIN: {
                    pet->setStrain(100);
                    pet->setHappy(pet->getHappy() - 25);
                    break;
                }
                case CAREMISS_POOP: {
                    pet->setHappy(pet->getHappy() - 25);
                    break;
                }
                case CAREMISS_LIGHT: {
                    pet->setHappy(pet->getHappy() - 10);
                    break;
                }
            }
            resolved = true;
        }
};