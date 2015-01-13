/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///ASG PileupReweightingTool interface
///author: will@cern.ch
#ifndef PILEUPREWEIGHTING_IPILEUPREWEIGHTINGTOOL_H
#define PILEUPREWEIGHTING_IPILEUPREWEIGHTINGTOOL_H

#include "AsgTools/IAsgTool.h"
#include "TString.h"
#include "xAODEventInfo/EventInfo.h"

namespace Root {
class TPileupReweighting;
}

namespace CP {

   class IPileupReweightingTool : public virtual asg::IAsgTool {

      ASG_TOOL_INTERFACE( CP::IPileupReweightingTool )

      public:
         /// Decorates with: PileupWeight , RandomRunNumber, RandomLumiBlockNumber
         virtual void apply ( const xAOD::EventInfo* eventInfo ) = 0;

         /// Get the dataWeight used to 'unprescale' data collected from a given trigger combination
         virtual double dataWeight ( const xAOD::EventInfo* eventInfo, TString trigger, bool mu_dependent=true ) = 0;

         /// Get pointer to the underlying tool - expert use only
         virtual Root::TPileupReweighting* get() = 0;

         /// execute method - retrieves the eventInfo from the storegate and decorates it for you 
         virtual StatusCode execute() = 0;
   };

}



#endif