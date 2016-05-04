/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the CaloClusterConstituentsOrigin implementation class
// Michael Nelson, CERN & University of Oxford
//
//
#include "JetRecTools/CaloClusterConstituentsOrigin.h"
//#include "xAODBase/IParticleContainer.h"
//#include "xAODCaloEvent/CaloClusterContainer.h"
//#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"



CaloClusterConstituentsOrigin::CaloClusterConstituentsOrigin(const std::string & name): JetConstituentModifierBase(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("VertexContainer",m_vertexContName="PrimaryVertices");
  
}

StatusCode CaloClusterConstituentsOrigin::process(xAOD::IParticleContainer* cont) const {
   xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
   std::cout << " processing " << cont << std::endl;
   if(clust)
     {
       const xAOD::VertexContainer* vertexContainer=0;

       //get vertexcontainer from eventstore
       CHECK( evtStore()->retrieve(vertexContainer, m_vertexContName) );

       return process(clust, vertexContainer->at(0)); 
     }
   return StatusCode::FAILURE;
}

// Apply PU origining and decorate the CaloCluster container appropriately:
	
StatusCode CaloClusterConstituentsOrigin::process(xAOD::CaloClusterContainer* cont, const xAOD::Vertex *vert) const {
  if (!vert)
    {
      std::cout << " Unable to find vertex " << vert << std::endl;
      return StatusCode::FAILURE;
    }



  for(xAOD::CaloCluster* cl : *cont) {
    xAOD::CaloVertexedTopoCluster corrCL( *cl,vert->position());
    cl->setEta(corrCL.eta());
    cl->setPhi(corrCL.phi());
  }
  return StatusCode::SUCCESS;
}


