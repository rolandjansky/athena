/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRECTOOLS_CLUSTERATEMSCALETOOL_H
#define JETRECTOOLS_CLUSTERATEMSCALETOOL_H

// \class ClusterAtEMScaleTool
// \author Jennifer Roloff
// \date October 2016
//


#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <string>

class ClusterAtEMScaleTool : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(ClusterAtEMScaleTool, IJetConstituentModifier)

  public:
  
  ClusterAtEMScaleTool(const std::string& name);
  ~ClusterAtEMScaleTool();
  StatusCode process(xAOD::IParticleContainer* cont) const; 
  StatusCode process(xAOD::CaloClusterContainer* cont) const; // MEN: Might need to rename this process


		
};


#endif
