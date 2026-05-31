#pragma once 

#include "caremiss.h"
#include "world.h"
#include "device.h"

#define CAREMISS_LENGTH 5
#define CAREMISS_DELAY 15 * 60

typedef struct {
    float happyMalus;
    float happyBonus;
    float strainAwake;
    float hungryAwake;
    float strainSleeping;
    float hungrySleeping;
} StateModifier;

class Pet {
    private: 
        float happy = 100;
        float strain = 100;
        float hungry = 100;

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
            for(unsigned int i = 0; i < caremissesSize; i++) {
                if(caremisses[i].getResolved() == false && time - caremisses[i].createdAt > CAREMISS_DELAY) {
                    switch(caremisses[i].type) {
                        case CAREMISS_HUNGRY: {
                            this->setHungry(100);
                            this->setHappy(this->getHappy() - 25);
                            break;
                        }
                        case CAREMISS_STRAIN: {
                            this->setStrain(100);
                            this->setHappy(this->getHappy() - 25);
                            break;
                        }
                        case CAREMISS_POOP: {
                            this->setHappy(this->getHappy() - 25);
                            break;
                        }
                        case CAREMISS_LIGHT: {
                            this->setHappy(this->getHappy() - 10);
                            break;
                        }
                    }
                    caremisses[i].resolved = true;
                }
            }
        }

        void manageStats() {
            if(strain >= 50 && hungry >= 50) {
                happy += modifier.happyBonus;
            } else if(strain < 50 && hungry < 50) {
                happy += modifier.happyMalus;
            }

            if(sleeping) {
                hungry += modifier.hungrySleeping;
                strain += modifier.strainSleeping;
            } else {
                hungry += modifier.hungryAwake;
                strain += modifier.strainAwake;
            }
        }

    public:
        World &world;
        Device &device;
        StateModifier &modifier;

        Pet(World& w, Device& d, StateModifier &m) : world(w), device(d), modifier(m) {}

        // to call every 1 minute
        void update() {
            manageCaremisses();
            manageSleeping();
            manageStats();
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
            unsigned int i = 0;
            bool done = false;
            while (i < caremissesSize) {
                if(caremisses[i].type == type && caremisses[i].getResolved() == false) {
                    done = true;
                    break;
                }
                i += 1;
            }
            if(done == false) return;
            for(; i < caremissesSize - 1; i++) {
                caremisses[i].type = caremisses[i+1].type;
                caremisses[i].createdAt = caremisses[i+1].createdAt;
            }
            caremissesSize -= 1;
        }

        void setHappy(float value) {
            if(value < 0) {
                this->happy = 0;
            } else if(value > 100) {
                this->happy = 100;
            } else {
                this->happy = value;
            }
        }

        void setStrain(float value) {
            if(value < 0) {
                this->strain = 0;
            } else if(value > 100) {
                this->strain = 100;
            } else {
                this->strain = value;
            }
        }

        void setHungry(float value) {
            if(value < 0) {
                this->hungry = 0;
            } else if(value > 100) {
                this->hungry = 100;
            } else {
                this->hungry = value;
            }
        }

        float getHappy() const {
            return this->happy;
        }

        float getStrain() const {
            return this->strain;
        }

        float getHungry() const {
            return this->hungry;
        }

        void setSleepSchedule(unsigned int sh, unsigned int sm, unsigned int wh, unsigned int wm) {
            sleepHour = sh; sleepMin = sm; wakeHour = wh; wakeMin = wm;
        }

        bool getSleeping() const {
            return sleeping;
        }

};