/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_CHARGEDHADRONSUBTRACTIONTOOL_H
#define JETRECTOOLS_CHARGEDHADRONSUBTRACTIONTOOL_H

////////////////////////////////////////////
/// \class ChargedHadronSubtractionTool
///
/// Removes charged PFO not associated to the PV
///
/// \author John Stupak and Jennifer Roloff
//////////////////////////////////////////////////

#include <string>
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "xAODPFlow/PFOContainer.h"
#include "PFlowUtils/IRetrievePFOTool.h"
#include "AsgTools/ToolHandle.h"

class ChargedHadronSubtractionTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(ChargedHadronSubtractionTool, IJetConstituentModifier)

  public:
  
  ChargedHadronSubtractionTool(const std::string& name);

  // Check that the configuration is reasonable
  StatusCode initialize();

  private:
  // Implement the correction
  StatusCode process_impl(xAOD::IParticleContainer* cont) const; 
  StatusCode removePileupChargedHadrons(xAOD::PFOContainer& cont) const;
	
};

#endif
