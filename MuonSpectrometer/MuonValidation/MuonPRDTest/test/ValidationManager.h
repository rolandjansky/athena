/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALIDATIONMANAGER_H
#define VALIDATIONMANAGER_H

#include "TDirectory.h"

#include "stdint.h"
#include <map>

using namespace std;

class ValidationStep;

class ValidationManager {
    public:
    static ValidationManager* instance(void);

    ValidationStep* get_step(const char* name);
    ValidationStep* insert(ValidationStep* step, const char* parent=0);
/*
    ValidationStep* get_or_create(const char* name,TDirectory* dir = 0);
    ValidationStep* get_or_create(const char* name, const char* parent);
*/

    private:
    static ValidationManager* s_instance;
    ValidationManager();
    ~ValidationManager();

    map<uint32_t,ValidationStep*> m_steps;
};

#endif
