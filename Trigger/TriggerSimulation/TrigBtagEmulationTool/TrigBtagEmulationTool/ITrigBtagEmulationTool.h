/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef ITrigBtagEmulationTool_H_
#define ITrigBtagEmulationTool_H_

#include "AsgTools/IAsgTool.h"

namespace Trig {

  class ITrigBtagEmulationTool : public virtual asg::IAsgTool {
    ASG_TOOL_INTERFACE(Trig::ITrigBtagEmulationTool)      

    public:
      virtual bool isPassed(const std::string&)=0;
  };

}
#endif
