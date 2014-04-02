/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_PERSISTENCY_PMETADATA_H
#define DQM_PERSISTENCY_PMETADATA_H

#include <string>
#include <map>

namespace dqm_persistency {
  class PMetadata : public TObject {
  public:
    std::map<std::string,std::string> data;
    virtual ~PMetadata() { }

    void Print(const Option_t* opt="") const;
//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef(dqm_persistency::PMetadata, 1)
///\endcond
  };
}

#endif //DQM_PERSISTENCY_PMETADATA_H
