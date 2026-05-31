#pragma once 


class World {
    private:
        /**
         * current unix timestamp
         */
        unsigned int time;
    
    public: 
        bool light = true;

        void setTime(unsigned int t) {
            this->time = t;
        }

        unsigned int getTime() const {
            return this->time;
        }
};