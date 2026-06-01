#pragma once 

#include "memory.h"

inline unsigned int strlen(const char* data) {
    if(data == 0) return 0;
    unsigned int i = 0;
    while(data[i] != '\0') {
        i+=1;
    }
    return i;
}


// Yeah this may seem dumb, but I actually want to be able to run this anywhere without depending on libc
class String {
    private: 
        Ptr* ptr;
        
    public:
        String() {
            ptr = 0;
        }

        ~String() {
            if(ptr == 0) return;
            pool_free(ptr);
        }
        
        String(const char* d) {
            unsigned int len = strlen(d);
            if(len == 0) {
                ptr = 0;
                return;
            }
            ptr = pool_alloc(len);
            for(unsigned int i = 0; i < len; i++) {
                ((char*)ptr->data)[i] = d[i];
            }
        }
        
        unsigned int length() {
            if(ptr == 0) return 0;
            return ptr->size;
        }
};