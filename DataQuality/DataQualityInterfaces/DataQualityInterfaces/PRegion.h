/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_PERSISTENCY_PREGION_H
#define DQM_PERSISTENCY_PREGION_H

#include <string>
#include <vector>
#include "PParameter.h"

namespace dqm_persistency {
  //char* thunk(void * in);

  class PRegion : public PParameter {
  public:
    PRegion();
    virtual ~PRegion() { };

    std::vector<int> subparameters; // this is an object ID!
    TObject* layout;

    void Print(const Option_t* opt="") const;

    void AddSubparameter(PParameter& daughter);

    PParameter* GetSubparameter(TDirectory& topdir, unsigned int i) const;

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef(dqm_persistency::PRegion, 1)
///\endcond
  };
}

#endif //DQM_PERSISTENCY_PREGION_H
