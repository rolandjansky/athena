/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string.h>
#include <stdint.h>

#include "TDirectory.h"


#define HASH(str) generateHash(str, strlen(str))

inline uint32_t generateHash(const char *string, size_t len)
{
    uint32_t hash = 0;
    for(size_t i = 0; i < len; ++i)
        hash = 65599 * hash + string[i];
    return hash ^ (hash >> 16);
}

//class ValidationStep;

// **  Step factory implementation
//ValidationStep* STEP_FACTORY(const char* name, TDirectory* dir = 0) 
