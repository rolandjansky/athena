/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_PERSISTENCY_PRESULT_H
#define DQM_PERSISTENCY_PRESULT_H

#include <string>
#include <vector>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include "TObject.h"
#include "dqm_core/Result.h"

namespace dqm_persistency {
  class PResult {
  public:
    PResult();
    virtual ~PResult() {} ;

    std::vector<std::pair<std::string,float> > results;
    TObject* resultobj;
    int status;
    time_t timestamp;

    void Print(const Option_t* opt="") const;
//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef(dqm_persistency::PResult, 1)
///\endcond
  };
}

#endif //DQM_PERSISTENCY_PRESULT_H
