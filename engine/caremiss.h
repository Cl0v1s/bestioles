#pragma once

#define CAREMISS_HUNGRY 0x01
#define CAREMISS_STRAIN 0x02
#define CAREMISS_LIGHT 0x03
#define CAREMISS_POOP 0x04

class Pet;

class Caremiss {
    friend Pet;

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
};
