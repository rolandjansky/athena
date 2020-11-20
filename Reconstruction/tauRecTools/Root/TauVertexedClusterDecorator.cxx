/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauVertexedClusterDecorator.h"

#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

TauVertexedClusterDecorator::TauVertexedClusterDecorator(const std::string& name):
  TauRecToolBase(name) {
  declareProperty("SeedJet", m_seedJet = ""); 
  declareProperty("VertexCorrection", m_doVertexCorrection = true);
  declareProperty("JetVertexCorrection", m_doJetVertexCorrection = true);
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
  else {
    ATH_MSG_ERROR("Seed jet " << m_seedJet << " not supported !");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


  
const xAOD::Vertex* TauVertexedClusterDecorator::getJetVertex(const xAOD::Jet& jet) const {
  
  const xAOD::Vertex* jetVertex = nullptr;
  
  if (m_doJetVertexCorrection && !inTrigger()) {
    bool isAvailable = jet.getAssociatedObject("OriginVertex", jetVertex);
    if (!isAvailable) {
      ATH_MSG_WARNING("OriginVertex not available !");
      jetVertex = nullptr;
    }
  }

  return jetVertex;
}



StatusCode TauVertexedClusterDecorator::execute(xAOD::TauJet& tau) const {
  if (! tau.jetLink().isValid()) {
    ATH_MSG_WARNING("Link to the seed jet is invalid");
    return StatusCode::FAILURE;
  }
  
  // Obtain the vertex to correct the cluster
  const xAOD::Vertex* vertex = nullptr;
  if (m_doVertexCorrection) {
    if (tau.vertexLink().isValid()) {
      vertex = tau.vertex();
    }
  }
  else {
    const xAOD::Jet* jetSeed = tau.jet();
    vertex = getJetVertex(*jetSeed);
  }

  std::vector<const xAOD::IParticle*> particleList = tau.clusters();
  
  std::vector<xAOD::CaloVertexedTopoCluster> vertexedClusterList;
  for (const xAOD::IParticle* particle : particleList) {
    const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>(particle);

    if (vertex) {
      vertexedClusterList.emplace_back(*cluster, m_clusterState, vertex->position());
    }
    else {
      vertexedClusterList.emplace_back(*cluster, m_clusterState);
    }
  }

  SG::AuxElement::Accessor<std::vector<xAOD::CaloVertexedTopoCluster>> vertexedClustersAcc("VertexedClusters");
  vertexedClustersAcc(tau) = vertexedClusterList;

  return StatusCode::SUCCESS;
} 
