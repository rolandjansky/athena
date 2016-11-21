/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "JetRecTools/ClusterAtEMScaleTool.h"

using namespace std;


ClusterAtEMScaleTool::ClusterAtEMScaleTool(const std::string& name) : JetConstituentModifierBase(name)
{
#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
}


StatusCode ClusterAtEMScaleTool::process(xAOD::CaloClusterContainer* cont) const {
  xAOD::CaloClusterContainer::iterator cl = cont->begin();
  xAOD::CaloClusterContainer::iterator cl_end = cont->end();

  for( ; cl != cl_end; ++cl ) {
		(*cl)->setCalE( (*cl)->rawE() );
		(*cl)->setCalPhi( (*cl)->rawPhi() );
		(*cl)->setCalEta( (*cl)->rawEta() );
  }

  return StatusCode::SUCCESS;
}

StatusCode ClusterAtEMScaleTool::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}


ClusterAtEMScaleTool::~ClusterAtEMScaleTool(){
}


