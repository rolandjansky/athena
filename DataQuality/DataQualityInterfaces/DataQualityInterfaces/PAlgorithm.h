/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_PERSISTENCY_PALGORITHM_H
#define DQM_PERSISTENCY_PALGORITHM_H

#include <string>
#include <vector>
#include <map>
#include "TObject.h"

namespace dqm_persistency {
  class PAlgorithm {
  public:
    virtual ~PAlgorithm() { };

    std::string name;
    std::string library;
    std::map<std::string, std::vector<float> > parameters;
    std::map<std::string, float> redThresholds;
    std::map<std::string, float> greenThresholds;

    void Print(const Option_t* opt="") const;
//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef(dqm_persistency::PAlgorithm, 1)
///\endcond
  };
}

#endif //DQM_PERSISTENCY_PALGORITHM_H
