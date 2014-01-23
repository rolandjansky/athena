/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ValidationManager.h"
#include "ValidationStep.h"
#include "Utils.h"

#include "TROOT.h"

#include <iostream>


ValidationManager* ValidationManager::s_instance = 0;

ValidationManager::ValidationManager() {}

ValidationManager::~ValidationManager() {
    m_steps.clear();
}

ValidationManager* ValidationManager::instance() {
    if (s_instance==0) s_instance = new ValidationManager();
    return s_instance;
}

ValidationStep* ValidationManager::get_step(const char* name) {
    uint32_t key = HASH(name);
    std::map <uint32_t,ValidationStep*>::iterator it =  m_steps.find(key);
    if (it != m_steps.end()) return (*it).second;
    return 0;
}

ValidationStep* ValidationManager::insert(ValidationStep* step, const char* parent) {

    // get parent if requested
    ValidationStep* Parent = 0;
    if (parent!=0) {
        Parent = get_step(parent);
        if (Parent==0) {
            std::cout << "cannot find parent " << parent << ". Terminating ..." << std::endl; 
        }
    }

    // Insert step ino the map
    uint32_t key = HASH(step->name());
    std::pair < std::map<uint32_t,ValidationStep*>::iterator, bool> r =
            m_steps.insert(std::pair<uint32_t,ValidationStep*>(key,step));

    if (!r.second) {
        std::cout << "NSWGeomAnalysis: failed to insert " << step->name() << " step. Terminating ..." << std::endl;
        return 0;
    }

    std::map<uint32_t,ValidationStep*>::iterator in = r.first;
    if (Parent) Parent->AddStep( (*in).second );
    return (*in).second;
}
/*
ValidationStep* ValidationManager::get_or_create(const char* name, TDirectory* dir ) {
    uint32_t key = HASH(name);
    std::map <uint32_t,ValidationStep*>::iterator it =  m_steps.find(key);
    if ( it == m_steps.end() ) {

        ValidationStep* step = STEP_FACTORY(name,dir);
        if (step==0) return step;

        std::pair < std::map<uint32_t,ValidationStep*>::iterator, bool> r =
            m_steps.insert(std::pair<uint32_t,ValidationStep*>(key,step));

        if (r.second) {
            std::map<uint32_t,ValidationStep*>::iterator found = r.first;
            return (*found).second;
        } else {
            std::cout << "NSWGeomAnalysis: failed to insert " << name << " step. Terminating ..." << std::endl;
        }
    }
    return (*it).second;
}

ValidationStep* ValidationManager::get_or_create(const char* name, const char* parent) {
    ValidationStep* p = get_step(parent);
    if ( p!=0 ) {
        ValidationStep* step = get_or_create( name, p->StepDirectory() );
        if (step!=0) {
            p->AddStep(step);
        }
        return step;
    } else {
        std::cout << "ValidationManager: could not find parent " << parent << ". ValidationStep " << name << " not created." << std::endl;
    }
    return 0;
}
*/
