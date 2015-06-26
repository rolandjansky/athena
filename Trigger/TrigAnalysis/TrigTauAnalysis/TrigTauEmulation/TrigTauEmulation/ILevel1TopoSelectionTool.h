/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILEVEL1TOPOSELECTIONTOOL_LEVEL1TOPOSELECTIONTOOL_H
#define ILEVEL1TOPOSELECTIONTOOL_LEVEL1TOPOSELECTIONTOOL_H

#include "AsgTools/IAsgTool.h"

//EDM include
// EDM includes
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"


class ILevel1TopoSelectionTool : public virtual asg::IAsgTool
{

   ASG_TOOL_INTERFACE(ILevel1TopoSelectionTool)
    
 public:

  virtual ~ILevel1TopoSelectionTool() {};

  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="")  = 0;
  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::JetRoIContainer * c2,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="") = 0;

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::MuonRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="") = 0;

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::EnergySumRoI * met,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="") = 0;

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::EmTauRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const xAOD::JetRoIContainer * c3,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="",
			     const std::string & sel3="") = 0;

  // Get the decision for a specific Level1Topo
  virtual StatusCode execute(const xAOD::MuonRoIContainer * c1,
			     const xAOD::EmTauRoIContainer * c2,
			     const xAOD::JetRoIContainer * c3,
			     const std::string & decor,
			     const std::string & sel1="",
			     const std::string & sel2="",
			     const std::string & sel3="") = 0;

  virtual bool decision(const std::string & item) = 0;


};

#endif
