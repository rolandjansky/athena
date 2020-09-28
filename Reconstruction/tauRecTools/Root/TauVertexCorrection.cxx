/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauVertexCorrection.h"

#include "xAODCaloEvent/CaloVertexedTopoCluster.h"

TauVertexCorrection::TauVertexCorrection(const std::string& name):
  asg::AsgTool(name) {
  declareProperty("SeedJet", m_seedJet = ""); 
  declareProperty("VertexCorrection", m_doVertexCorrection = true);
  declareProperty("JetVertexCorrection", m_doJetVertexCorrection = true);
}



StatusCode TauVertexCorrection::initialize() {  
  ATH_MSG_INFO("in initialize");

  if (m_seedJet == "AntiKt4LCTopoJets") {
    m_isPFO = false;
    m_clusterState = xAOD::CaloCluster::State::CALIBRATED; 
  }
  else if (m_seedJet == "AntiKt4EMPFlowJets") {
    m_isPFO = true;
    m_clusterState = xAOD::CaloCluster::State::UNCALIBRATED;
  }
  else {
    ATH_MSG_ERROR("Seed jet " << m_seedJet << " not supported !");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("VertexCorrection: " << m_doVertexCorrection);
  ATH_MSG_INFO("JetVertexCorrection: " << m_doJetVertexCorrection);
  ATH_MSG_INFO("SeedJet: " <<  m_seedJet);
  ATH_MSG_INFO("PFO: " << m_isPFO);
  ATH_MSG_INFO("Cluster Calibraction State: " << m_clusterState);

  return StatusCode::SUCCESS;
}


  
const xAOD::Vertex* TauVertexCorrection::getJetVertex(const xAOD::Jet& jet) const {
  
  const xAOD::Vertex* jetVertex = nullptr;
  
  if (m_doJetVertexCorrection) {
    bool isAvailable = jet.getAssociatedObject("OriginVertex", jetVertex);
    if (!isAvailable) {
      ATH_MSG_WARNING("OriginVertex not available !");
      jetVertex = nullptr;
    }
  }

  return jetVertex;
}



TLorentzVector TauVertexCorrection::getTauAxis(const xAOD::TauJet& tau) const {
  TLorentzVector tauAxis;
  if (m_doVertexCorrection) {
    tauAxis = tau.p4(xAOD::TauJetParameters::IntermediateAxis);
  }
  else {
    tauAxis = tau.p4(xAOD::TauJetParameters::DetectorAxis);
  }

  return tauAxis;
}



TLorentzVector TauVertexCorrection::getVertexCorrectedP4(const xAOD::CaloCluster& cluster,
                                                 const Amg::Vector3D& position) const {
  
  TLorentzVector vertexCorrectedP4 = xAOD::CaloVertexedTopoCluster(cluster, m_clusterState, position).p4();
  
  ATH_MSG_DEBUG("Cluster: " << &cluster);
  ATH_MSG_DEBUG("Original cluster four momentum, pt: " << cluster.pt(m_clusterState) << 
                " eta: " << cluster.eta(m_clusterState) << " phi: " << cluster.phi(m_clusterState) << 
                " e: " << cluster.e(m_clusterState));
  ATH_MSG_DEBUG("Vertex corrected four momentum, pt: " << vertexCorrectedP4.Pt() << 
                " eta: " << vertexCorrectedP4.Eta() << " phi: " << vertexCorrectedP4.Phi() << 
                " e: " << vertexCorrectedP4.E());

  return vertexCorrectedP4;
} 



TLorentzVector TauVertexCorrection::getVertexCorrectedP4(const xAOD::PFO& pfo,
                                                 const Amg::Vector3D& position) const {
  TLorentzVector vertexCorrectedP4;
  
  if (!pfo.isCharged()) {
    TVector3 pos(position.x(), position.y(), position.z()); 
    
    // If there is a vertex correction in jet reconstruction, then pfo.p4() is the four momentum 
    // at EM scale. Otherwise, pfo.p4() is at LC scale (not clear), and pfo.p4EM() is the four 
    // momentum at EM scale. 
    // TODO: May need further modifications, depending on how the jet reconstruction fix ATLJETMET-1280
    // The strategy only works for PFlow at EM scale.
    if (m_doJetVertexCorrection) {
      vertexCorrectedP4 = pfo.GetVertexCorrectedFourVec(pos);
    }
    else {
      vertexCorrectedP4 = pfo.GetVertexCorrectedEMFourVec(pos);
    }
  }
  else {
    vertexCorrectedP4 = pfo.p4();  
  }
  
  ATH_MSG_DEBUG("Original pfo four momentum, pt: " << pfo.pt() << 
                  " eta: " << pfo.eta() << " phi: " << pfo.phi() << " e: " << pfo.e());
  ATH_MSG_DEBUG("Vertex corrected four momentum, pt: " << vertexCorrectedP4.Pt() << 
                " eta: " << vertexCorrectedP4.Eta() << " phi: " << vertexCorrectedP4.Phi() << " e: " << vertexCorrectedP4.E());

  return vertexCorrectedP4;
} 



TLorentzVector TauVertexCorrection::getVertexCorrectedP4(const xAOD::CaloCluster& cluster, 
                                                    const xAOD::Vertex* tauVertex,
                                                    const xAOD::Vertex* jetVertex) const { 
  TLorentzVector vertexCorrectedP4 = cluster.p4(m_clusterState);
  Amg::Vector3D position;

  // In jet reconstruction:
  // Clusters in Topo jets are corrected to point at PV0 when m_doJetVertexCorrection is true
  // Clusters in PFlow jets are not corrected in jet reconstruction 
  // In tau reconstruction:
  // If we want to do the vertex correction using tau vertexs, use the position of the tau vertex 
  // for PFlow jets, while the relative position of the tau vertex and default vertex for Topo jets
  // If not, use the default vertex to correct clusters from PFlow jets 
  if (m_doVertexCorrection) {
    if (m_isPFO) {
      // use the position of tau vertex if we want to correct to tau vertex and seed is PFO jets 
      if (tauVertex) {
        position = tauVertex->position();
        vertexCorrectedP4 = getVertexCorrectedP4(cluster, position);
      }
      // use the position of jet vertex if tau vertex not available
      else if (m_doJetVertexCorrection && jetVertex) {
        position = jetVertex->position();
        vertexCorrectedP4 = getVertexCorrectedP4(cluster, position);
      }
    }
    else { 
      // use the relative position of tau vertex and default vertex for Topo jets
      if (tauVertex && tauVertex != jetVertex) {         
        position = tauVertex->position();
        if (m_doJetVertexCorrection && jetVertex) {
          position -= jetVertex->position();
        }
        vertexCorrectedP4 = getVertexCorrectedP4(cluster, position);
      }
    }
  }
  else if (m_doJetVertexCorrection && m_isPFO) {
    // use the position of jet vertex for PFO jets when no tau vertex correction is requried
    if (jetVertex) {
      position = jetVertex->position();
      vertexCorrectedP4 = getVertexCorrectedP4(cluster, position);
    }
  }
  
  return vertexCorrectedP4;
}



TLorentzVector TauVertexCorrection::getVertexCorrectedP4(const xAOD::PFO& pfo,
                                                    const xAOD::Vertex* tauVertex,
                                                    const xAOD::Vertex* jetVertex) const {
  TLorentzVector vertexCorrectedP4 = pfo.p4();
  Amg::Vector3D position;

  // In jet reconstruction:
  // PFOs are corrected to point at PV0 when m_doJetVertexCorrection is true
  
  // In tau reconstruction:
  // If we want to do the vertex correction using tau vertexs, use the relative position 
  // of the tau vertex and default vertex
  
  if (m_doVertexCorrection) {
    // use the relative position of tau vertex and default vertex
    if (tauVertex) { 
      position = tauVertex->position();
      if (m_doJetVertexCorrection && jetVertex) {
        position -= jetVertex->position();
      }
      vertexCorrectedP4 = getVertexCorrectedP4(pfo, position);
    }
  }
  
  return vertexCorrectedP4;
}



TLorentzVector TauVertexCorrection::getVertexCorrectedP4(const xAOD::JetConstituent& constituent,
                                                    const xAOD::Vertex* tauVertex,
                                                    const xAOD::Vertex* jetVertex) const {
  TLorentzVector vertexCorrectedP4;
  
  if (constituent.type() == xAOD::Type::CaloCluster) {
	const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>( constituent.rawConstituent() );
    vertexCorrectedP4 = getVertexCorrectedP4(*cluster, tauVertex, jetVertex);
  }
  else if (constituent.type() == xAOD::Type::ParticleFlow) {
	const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>( constituent.rawConstituent() );
    vertexCorrectedP4 = getVertexCorrectedP4(*pfo, tauVertex, jetVertex); 
  }
  else {
	ATH_MSG_WARNING("Seed jet constituent type not supported, will not do vertex correction !");
    vertexCorrectedP4.SetPtEtaPhiE(constituent.pt(), constituent.eta(), constituent.phi(), constituent.e());
  }
 
  return vertexCorrectedP4; 
}
