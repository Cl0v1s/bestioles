#pragma once 

#include "caremiss.h"
#include "world.h"
#include "device.h"

#define CAREMISS_LENGTH 5
#define CAREMISS_DELAY 15 * 60

class Pet {
    private: 
        unsigned int happy = 100;
        unsigned int strain = 100;
        unsigned int hungry = 100;

        unsigned int sleepHour = 0;
        unsigned int sleepMin = 0;
        unsigned int wakeHour = 0;
        unsigned int wakeMin = 0;

        bool sleeping = false;
        bool forceSleeping = false;

        Caremiss caremisses[CAREMISS_LENGTH];
        unsigned int caremissesSize = 0;

        void manageSleeping() {
            bool light = world.light;
            const unsigned int time = world.getTime();
            // allows player to put pet to bed 
            if(light == false) {
                sleeping = true;
                forceSleeping = true;
            }

            if(strain <= 0) {
                Caremiss sleep(CAREMISS_STRAIN, time);
                addCaremiss(sleep);
                sleeping = true;
                forceSleeping = true;
            } else if(strain >= 75) { // We cant force a pet with a rather good eneergy level to sleep
                forceSleeping = false;
            }

            // if we are sleeping out of fatigue we dont check for time
            if(!forceSleeping) {
                unsigned int secondsForDay = time % (24 * 60 * 60);
                unsigned int sleepSeconds = sleepHour * 3600 + sleepMin * 60;
                unsigned int wakeSeconds = wakeHour * 3600 + wakeMin * 60;

                if (sleepSeconds <= wakeSeconds) {
                    // same-day nap: e.g. sleep at 10h, wake at 14h
                    sleeping = (secondsForDay >= sleepSeconds && secondsForDay < wakeSeconds);
                } else {
                    // overnight: e.g. sleep at 22h, wake at 8h
                    sleeping = (secondsForDay >= sleepSeconds || secondsForDay < wakeSeconds);
                }
            }
            
            if(!sleeping) return;

            if(light) { // went to sleep with light on
                Caremiss light(CAREMISS_LIGHT, time);
                addCaremiss(light);
            } else {
                removeCaremiss(CAREMISS_LIGHT); // we try to remove a possible light caremiss if not resolved
            }
        }

        void manageCaremisses() {
            const unsigned int time = world.getTime();
            for(int i = 0; i < caremissesSize; i++) {
                if(caremisses[i].getResolved() == false && time - caremisses[i].createdAt > CAREMISS_DELAY) {
                    caremisses[i].apply(this);
                }
            }
        }

    public:
        World &world;
        Device &device;

        Pet(World& w, Device& d) : world(w), device(d) {}

        void update() {
            manageCaremisses();
            manageSleeping();
        }

        void death() {
            // TODO
        }

        void addCaremiss(Caremiss& value) {
            if(caremissesSize >= CAREMISS_LENGTH) {
                death();
                return;
            }
            caremisses[caremissesSize] = value;
            caremissesSize += 1;
            device.alert();
        }

        void removeCaremiss(unsigned int type) {
            int i = 0;
            bool done = false;
            do { 
                if(caremisses[i].type == type && caremisses[i].getResolved() == false) { // we can only remove caremiss of a given type if they arent already resolved 
                    done = true;
                } else {
                    i += 1;
                }
            } while (done == false && i < caremissesSize);
            if(done == false) return;
            for(; i < caremissesSize - 1; i++) {
                caremisses[i].type = caremisses[i+1].type;
                caremisses[i].createdAt = caremisses[i+1].createdAt;
            }
            caremissesSize -= 1;
        }

        void setHappy(int value) {
            if(value < 0) {
                this->happy = 0;
            } else if(value > 100) {
                this->happy = 100;
            } else {
                this->happy = value;
            }
        }

        void setStrain(int value) {
            if(value < 0) {
                this->strain = 0;
            } else if(value > 100) {
                this->strain = 100;
            } else {
                this->strain = value;
            }
        }

        void setHungry(int value) {
            if(value < 0) {
                this->hungry = 0;
            } else if(value > 100) {
                this->hungry = 100;
            } else {
                this->hungry = value;
            }
        }

        unsigned int getHappy() const {
            return this->happy;
        }

        unsigned int getStrain() const {
            return this->strain;
        }

        unsigned int getHungry() const {
            return this->hungry;
        }

        void setSleepSchedule(unsigned int sh, unsigned int sm, unsigned int wh, unsigned int wm) {
            sleepHour = sh; sleepMin = sm; wakeHour = wh; wakeMin = wm;
        }

        bool getSleeping() const {
            return sleeping;
        }

};