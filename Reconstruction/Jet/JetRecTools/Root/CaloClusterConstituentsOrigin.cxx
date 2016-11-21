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
  declareProperty("UseEMScale",m_useEMScale=false);
  
}

StatusCode CaloClusterConstituentsOrigin::process(xAOD::IParticleContainer* cont) const {
   xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container

   if(clust)
     {
       const xAOD::VertexContainer* vertexContainer=0;

       //get vertexcontainer from eventstore
       ATH_CHECK( evtStore()->retrieve(vertexContainer, m_vertexContName) );

       return process(clust, vertexContainer->at(0)); 
     }
   return StatusCode::FAILURE;
}

// Apply PU origining and decorate the CaloCluster container appropriately:
	
StatusCode CaloClusterConstituentsOrigin::process(xAOD::CaloClusterContainer* cont, const xAOD::Vertex *vert) const {
  if (!vert)
    {
      ATH_MSG_ERROR( " Unable to find vertex " << vert);
      return StatusCode::FAILURE;
    }
  if(m_useEMScale) return processEM(cont,vert);
  else return processLC(cont,vert);
}

StatusCode CaloClusterConstituentsOrigin::processLC(xAOD::CaloClusterContainer* cont, const xAOD::Vertex *vert) const {


  for(xAOD::CaloCluster* cl : *cont) {
    if(cl->calE()>1e-9) {
      xAOD::CaloVertexedTopoCluster corrCL( *cl,vert->position());
      cl->setEta(corrCL.eta());
      cl->setPhi(corrCL.phi());
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterConstituentsOrigin::processEM(xAOD::CaloClusterContainer* cont, const xAOD::Vertex *vert) const {


  for(xAOD::CaloCluster* cl : *cont) {
    if(cl->rawE()>1e-9) {
      xAOD::CaloVertexedTopoCluster corrCL( *cl,xAOD::CaloCluster::UNCALIBRATED, vert->position());
      cl->setE(corrCL.e());
      cl->setEta(corrCL.eta());
      cl->setPhi(corrCL.phi());
    }
  }
  return StatusCode::SUCCESS;
}


