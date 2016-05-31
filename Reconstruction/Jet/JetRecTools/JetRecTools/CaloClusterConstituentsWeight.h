/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Example implementation class for the JetConstsituentModSequence sequencer.
// A daughter class of JetConstituentModifierBase. 
//
// Author: Michael Nelson, University of Oxford, UK
//


#ifndef CALOCLUSTERCONSTITUENTSWEIGHT_H
#define CALOCLUSTERCONSTITUENTSWEIGHT_H

#include "AsgTools/AsgTool.h" 
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class CaloClusterConstituentsWeight: public JetConstituentModifierBase {
ASG_TOOL_CLASS(CaloClusterConstituentsWeight, IJetConstituentModifier) 
public:
	
  CaloClusterConstituentsWeight(const std::string & name); // MEN: constructor 
  StatusCode process(xAOD::IParticleContainer* cont) const; 
  StatusCode process(xAOD::CaloClusterContainer* cont) const; // MEN: Might need to rename this process
  float calculateWeight(xAOD::CaloCluster* cl) const;

};

#endif
