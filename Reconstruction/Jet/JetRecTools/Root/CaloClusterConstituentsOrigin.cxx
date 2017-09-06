/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source code for the CaloClusterConstituentsOrigin implementation class
// Michael Nelson, CERN & University of Oxford
//
//
#include "JetRecTools/CaloClusterConstituentsOrigin.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"



CaloClusterConstituentsOrigin::CaloClusterConstituentsOrigin(const std::string & name): JetConstituentModifierBase(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty ("VertexContainer",  
                   m_readVertexContainer_key="PrimaryVertices");
  declareProperty("UseEMScale",m_useEMScale=false);
  declareProperty("SaveDetectorEta",m_saveDetectorEta=false);
}

StatusCode CaloClusterConstituentsOrigin::initialize() {
  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_MSG_DEBUG("initializing version with data handles");

  ATH_CHECK(m_readVertexContainer_key.initialize());
  return StatusCode::SUCCESS;
}


StatusCode CaloClusterConstituentsOrigin::process(xAOD::IParticleContainer* cont) const {
   xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container

   if(clust)
     {

       auto handle = SG::makeHandle(m_readVertexContainer_key);
       ATH_CHECK(handle.isValid());
       auto vertexContainer = handle.cptr();
         
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
    float eta_det = cl->eta();
    if(cl->calE()>1e-9) {
      xAOD::CaloVertexedTopoCluster corrCL( *cl,vert->position());
      cl->setEta(corrCL.eta());
      cl->setPhi(corrCL.phi());
    }
    if(m_saveDetectorEta)
      cl->auxdecor<float>("DetectorEta") = eta_det;
  }
  return StatusCode::SUCCESS;
}

StatusCode CaloClusterConstituentsOrigin::processEM(xAOD::CaloClusterContainer* cont, const xAOD::Vertex *vert) const {


  for(xAOD::CaloCluster* cl : *cont) {
    float eta_det = cl->eta();
    if(cl->rawE()>1e-9) {      
      xAOD::CaloVertexedTopoCluster corrCL( *cl,xAOD::CaloCluster::UNCALIBRATED, vert->position());
      cl->setE(corrCL.e());
      cl->setEta(corrCL.eta());
      cl->setPhi(corrCL.phi());
    }
    if(m_saveDetectorEta)
      cl->auxdecor<float>("DetectorEta") = eta_det;
  }
  return StatusCode::SUCCESS;
}


