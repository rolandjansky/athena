/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */

#ifndef ITrigBtagEmulationTool_H_
#define ITrigBtagEmulationTool_H_

#include "AsgTools/IAsgTool.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "PATCore/TAccept.h"

namespace Trig {

  class ITrigBtagEmulationTool : public virtual asg::IAsgTool {
    ASG_TOOL_INTERFACE(Trig::ITrigBtagEmulationTool)
      
    public:

      virtual StatusCode initialize(void)=0;
      virtual StatusCode execute()=0;
      virtual StatusCode finalize()=0;
    
      virtual void addEmulatedChain(const std::vector<std::string>&)=0;
      virtual std::vector<std::string> addEmulatedChain(const std::string)=0;
      virtual bool isPassed(const std::string&)=0;
    
  };

}
#endif
