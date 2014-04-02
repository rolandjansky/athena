/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_PERSISTENCY_PPARAMETER_H
#define DQM_PERSISTENCY_PPARAMETER_H

#include <string>
#include "TNamed.h"

class TObject;
class TDirectory;

namespace dqm_persistency {
  class PAlgorithm;
  class PMetadata;
  class PRegion;
  class PResult;

  class PParameter : public TNamed {
  public:
    PParameter();
    virtual ~PParameter() { };

    std::string name;
    TObject* input;
    PResult* result;
    PAlgorithm* algorithm;
    TObject* reference;
    float weight;
    bool disableChildren;
    PMetadata* metadata;

    TObject* shape; 

    int parentRegion;

    void Print(const Option_t* opt="") const;
    
    PRegion* GetParentRegion(TDirectory& topdir);

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef(dqm_persistency::PParameter, 1)
///\endcond
  };
}

#endif //DQM_PERSISTENCY_PPARAMETER_H
