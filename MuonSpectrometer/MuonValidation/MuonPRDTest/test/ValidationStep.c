/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ValidationStep.h"
#include "NSWHitsTree.h"

#include <iostream>


ValidationStep::ValidationStep(TDirectory* current_dir, const char* name) :
m_name(name)
{
    m_step_directory = current_dir->mkdir(name,"");
}

ValidationStep::~ValidationStep() {}

const char* ValidationStep::name() const { return m_name.Data(); }
TDirectory* ValidationStep::StepDirectory() const { return m_step_directory; }

void ValidationStep::AddStep(ValidationStep* next) {
    m_next_steps.push_back(next);
}

ValidationStep* ValidationStep::Contains( const char* name) {
    for (unsigned int i=0; i<m_next_steps.size(); i++) {
        if ( strcmp(m_next_steps.at(i)->name(),name)==0 ) return m_next_steps[i];
    }
    return 0;
}
