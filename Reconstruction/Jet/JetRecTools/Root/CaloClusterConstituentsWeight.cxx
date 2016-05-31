/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the CaloClusterConstituentsWeight implementation class
// Michael Nelson, CERN & University of Oxford
//
//
#include "JetRecTools/CaloClusterConstituentsWeight.h"
//#include "xAODBase/IParticleContainer.h"
//#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "JetRecTools/JetConstituentModifierBase.h"

CaloClusterConstituentsWeight::CaloClusterConstituentsWeight(const std::string & name): JetConstituentModifierBase(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  
}

StatusCode CaloClusterConstituentsWeight::process(xAOD::IParticleContainer* cont) const {
   xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
   if(clust) return process(clust); 
   return StatusCode::FAILURE;
}

// Apply PU weighting and decorate the CaloCluster container appropriately:
	
StatusCode CaloClusterConstituentsWeight::process(xAOD::CaloClusterContainer* cont) const {
  SG::AuxElement::Accessor<float> weightAcc("PUWeight"); // Handle for PU weighting here
  for(xAOD::CaloCluster* cl : *cont) {
    float w = calculateWeight(cl);
    float clust_e = cl->e(); // Cluster energy
    cl->setE(clust_e * w); 
    //cl *= w; // MEN: Apply scaling to each calo cluster in the CaloCluster container (this won't work for now)
    weightAcc(*cl) = w; // Weight decoration of the container 
  }
  return StatusCode::SUCCESS;
}

float CaloClusterConstituentsWeight::calculateWeight(xAOD::CaloCluster*) const {
  float weight = 10; // Abitrary float for now
  return weight;
}

