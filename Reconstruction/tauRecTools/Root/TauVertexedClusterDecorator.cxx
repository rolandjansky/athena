/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauVertexedClusterDecorator.h"
#include "tauRecTools/HelperFunctions.h"

#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

TauVertexedClusterDecorator::TauVertexedClusterDecorator(const std::string& name):
  TauRecToolBase(name) {
  declareProperty("SeedJet", m_seedJet = ""); 
  declareProperty("VertexCorrection", m_doVertexCorrection = true);
}



StatusCode TauVertexedClusterDecorator::initialize() {  
  
  if (std::string(m_seedJet).find("LC") != std::string::npos) {
    ATH_MSG_INFO("Set the cluster state to CALIBRATED");
    m_clusterState = xAOD::CaloCluster::State::CALIBRATED; 
  }
  else if (std::string(m_seedJet).find("EM") != std::string::npos) {
    ATH_MSG_INFO("Set the cluster state to UNCALIBRATED");
    m_clusterState = xAOD::CaloCluster::State::UNCALIBRATED;
  } 
  else if (! inTrigger()) {
    ATH_MSG_ERROR("Seed jet " << m_seedJet << " not supported !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


  
StatusCode TauVertexedClusterDecorator::execute(xAOD::TauJet& tau) const {
  if (! tau.jetLink().isValid()) {
    ATH_MSG_WARNING("Link to the seed jet is invalid");
    return StatusCode::FAILURE;
  }
  
  // Obtain the vertex to correct the cluster
  const xAOD::Vertex* vertex = tauRecTools::getTauVertex(tau, inTrigger());

  std::vector<const xAOD::IParticle*> particleList = tau.clusters();
  
  std::vector<xAOD::CaloVertexedTopoCluster> vertexedClusterList;
  for (const xAOD::IParticle* particle : particleList) {
    const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>(particle);
   
    if (inTrigger()) { // In trigger, we use the default calibration state
      if (vertex) {
        vertexedClusterList.emplace_back(*cluster, vertex->position());
      }
      else {
        vertexedClusterList.emplace_back(*cluster);
      }
    }
    else { // In offline reconstruction, the calibration state is based on the name of seed jet
      if (vertex) {
        vertexedClusterList.emplace_back(*cluster, m_clusterState, vertex->position());
      }
      else {
        vertexedClusterList.emplace_back(*cluster, m_clusterState);
      }
    }
  }

  static const SG::AuxElement::Accessor<std::vector<xAOD::CaloVertexedTopoCluster>> vertexedClustersAcc("VertexedClusters");
  vertexedClustersAcc(tau) = vertexedClusterList;

  return StatusCode::SUCCESS;
} 
