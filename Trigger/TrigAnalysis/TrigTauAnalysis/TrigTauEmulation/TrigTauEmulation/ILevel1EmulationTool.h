/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
// Interface class of TauTriggerEmulationTool

// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef LEVEL1EMULATIONTOOL_ILEVEL1EMULATIONTOOL_H
#define LEVEL1EMULATIONTOOL_ILEVEL1EMULATIONTOOL_H

#include <set>

#include "PATInterfaces/CorrectionCode.h"
#include "AsgTools/IAsgTool.h"

// EDM Include
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"

// fwd declarations

namespace TrigTauEmul {
  class ILevel1EmulationTool : virtual public asg::IAsgTool

    {

      // Declare the interface that the class provides 
      ASG_TOOL_INTERFACE(ILevel1EmulationTool)

    public:

      /// virtual destructor
      virtual ~ILevel1EmulationTool() {};
      
      virtual StatusCode calculate(const xAOD::EmTauRoIContainer* l1taus, 
				   const xAOD::JetRoIContainer* l1jets,
				   const xAOD::MuonRoIContainer* l1muons,
				   const xAOD::EnergySumRoI* l1xe) = 0;
      
      virtual bool decision(const std::string & chain_name) = 0;

      virtual void GetChains() = 0;
      virtual StatusCode removeUnusedTools(const std::set<std::string>& usedTools) = 0;

      virtual std::vector<std::string> GetL1Chains() = 0;

      virtual StatusCode PrintReport(const xAOD::EmTauRoIContainer* l1taus, 
				     const xAOD::JetRoIContainer* l1jets,
				     const xAOD::MuonRoIContainer* l1muons,
				     const xAOD::EnergySumRoI* l1xe) = 0;

      virtual StatusCode PrintCounters() = 0;
      
    };
}
#endif
