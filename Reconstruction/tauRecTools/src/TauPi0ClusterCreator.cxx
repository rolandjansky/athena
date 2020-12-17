/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "TauPi0ClusterCreator.h"
#include "tauRecTools/HelperFunctions.h"

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "FourMomUtils/P4Helpers.h"
#include "xAODJet/Jet.h"


TauPi0ClusterCreator::TauPi0ClusterCreator(const std::string& name) :
    TauRecToolBase(name) {
}



StatusCode TauPi0ClusterCreator::executePi0ClusterCreator(xAOD::TauJet& tau, xAOD::PFOContainer& neutralPFOContainer,
							  xAOD::PFOContainer& hadronicPFOContainer,
							  const xAOD::CaloClusterContainer& pi0ClusterContainer) const {
  // Any tau needs to have PFO vectors. Set empty vectors before nTrack cut
  std::vector<ElementLink<xAOD::PFOContainer>> empty;
  tau.setProtoNeutralPFOLinks(empty);
  tau.setHadronicPFOLinks(empty);

  // only run shower subtraction on 1-5 prong taus 
  if (tau.nTracks() == 0 || tau.nTracks() > 5) {
    return StatusCode::SUCCESS;
  }

  // Retrieve Ecal1 shots and match them to clusters
  std::vector<const xAOD::PFO*> shotPFOs;
  
  unsigned nShots = tau.nShotPFOs();
  for (unsigned index=0; index<nShots; ++index) {
    const xAOD::PFO* shotPFO = tau.shotPFO(index);
    shotPFOs.push_back(shotPFO);
  }
  
  // Map shot to the pi0 cluster 
  std::map<unsigned, const xAOD::CaloCluster*> shotToClusterMap = getShotToClusterMap(shotPFOs, pi0ClusterContainer, tau);

  // We will always perform the vertex correction
  const xAOD::Vertex* vertex = tauRecTools::getTauVertex(tau);
  
  // Tau custom PFO reconstruction is only used in offline reconstrution
  TLorentzVector tauAxis = tauRecTools::getTauAxis(tau);

  // Loop over custom pi0 clusters, and create neutral PFOs
  for (const xAOD::CaloCluster* cluster: pi0ClusterContainer) {
    // custom clusters could be correctd directly using the tau vertex
    TLorentzVector clusterP4; 
    if (vertex) {
      xAOD::CaloVertexedTopoCluster vertexedCluster(*cluster, vertex->position());
      clusterP4 = vertexedCluster.p4();
    }
    else {
      clusterP4 = cluster->p4();
    }

    // Clusters must have enough energy, and within 0.4 cone of the tau candidate
    if (clusterP4.Pt() < m_clusterEtCut)   continue;
    if (clusterP4.DeltaR(tauAxis) > 0.4) continue;

    // Create the neutral PFOs
    xAOD::PFO* neutralPFO = new xAOD::PFO();
    neutralPFOContainer.push_back(neutralPFO);
    
    // Add the link to the tau candidate 
    ElementLink<xAOD::PFOContainer> PFOElementLink;
    PFOElementLink.toContainedElement(neutralPFOContainer, neutralPFO);
    tau.addProtoNeutralPFOLink(PFOElementLink);

    ATH_CHECK(configureNeutralPFO(*cluster, pi0ClusterContainer, tau, shotPFOs, shotToClusterMap, *neutralPFO));
  }

  // Loop over clusters from jet seed, and create hadronic PFOs
  std::vector<xAOD::CaloVertexedTopoCluster> vertexedClusterList = tau.vertexedClusters();
  for (const xAOD::CaloVertexedTopoCluster& vertexedCluster : vertexedClusterList){
    TLorentzVector clusterP4 = vertexedCluster.p4();
       
    // Clusters must have positive energy, and within 0.2 cone of the tau candidate 
    if(clusterP4.E()<=0.) continue;
    if(clusterP4.DeltaR(tauAxis) > 0.2) continue;

    double clusterEnergyHad = 0.;
    
    const xAOD::CaloCluster& cluster = vertexedCluster.clust();
    const CaloClusterCellLink* cellLinks = cluster.getCellLinks();
    CaloClusterCellLink::const_iterator cellLink = cellLinks->begin();
	for (; cellLink != cellLinks->end(); ++cellLink) {
	  const CaloCell* cell = static_cast<const CaloCell*>(*cellLink);
       
      int sampling = cell->caloDDE()->getSampling();
      if (sampling < 8) continue;

      // TODO: what is the weight for EMTopo
      double cellEnergy = cell->e() * cellLink.weight();
      clusterEnergyHad += cellEnergy;
    }
    
    // Energy in Had Calorimeter must be positive
    if(clusterEnergyHad <= 0.) continue;
  
    // Create the hadrnic PFO
    xAOD::PFO* hadronicPFO = new xAOD::PFO();
    hadronicPFOContainer.push_back(hadronicPFO);
    
    // Add element link from tau to hadronic PFO
    ElementLink<xAOD::PFOContainer> PFOElementLink;
    PFOElementLink.toContainedElement( hadronicPFOContainer, hadronicPFO );
    tau.addHadronicPFOLink( PFOElementLink );
    
    ATH_CHECK(configureHadronicPFO(cluster, clusterEnergyHad, *hadronicPFO));
  }

  return StatusCode::SUCCESS;
}



std::map<unsigned, const xAOD::CaloCluster*> TauPi0ClusterCreator::getShotToClusterMap(const std::vector<const xAOD::PFO*>& shotPFOs,
										 const xAOD::CaloClusterContainer& pi0ClusterContainer,
										 const xAOD::TauJet &tau) const {
  std::map<unsigned, const xAOD::CaloCluster*> shotToClusterMap;
  for (unsigned index = 0; index < shotPFOs.size(); ++index) {
    const xAOD::PFO* shotPFO = shotPFOs.at(index);

    int seedHashInt = -1;
    if (!shotPFO->attribute(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHashInt)) {
      ATH_MSG_WARNING("Couldn't find seed hash. Set it to -1, no cluster will be associated to shot.");
    }
    const IdentifierHash seedHash = static_cast<const IdentifierHash>(seedHashInt);

    // We will always perform the vertex correction
    const xAOD::Vertex* vertex = tauRecTools::getTauVertex(tau);
  
    // Tau custom PFO reconstruction is only used in offline reconstrution
    TLorentzVector tauAxis = tauRecTools::getTauAxis(tau);
    
    float weightInCluster = -1.;
    float weightInPreviousCluster = -1;
    
    // Loop over custom pi0 clusters, and map shot to the cluster
    for (const xAOD::CaloCluster* cluster: pi0ClusterContainer) {
      // custom clusters could be correctd directly using the tau vertex
      TLorentzVector clusterP4; 
      if (vertex) {
        xAOD::CaloVertexedTopoCluster vertexedCluster(*cluster, vertex->position());
        clusterP4 = vertexedCluster.p4();
      }
      else {
        clusterP4 = cluster->p4();
      }
      
      weightInCluster = -1.;
      if (clusterP4.Et() < m_clusterEtCut) continue;
      if (clusterP4.DeltaR(tauAxis) > 0.4)  continue;
        
      const CaloClusterCellLink* cellLinks = cluster->getCellLinks();
      CaloClusterCellLink::const_iterator cellLink = cellLinks->begin();
	  for (; cellLink != cellLinks->end(); ++cellLink) {
        const CaloCell* cell = static_cast<const CaloCell*>(*cellLink);
        
        // Check if seed cell is in cluster.
        if (cell->caloDDE()->calo_hash() != seedHash) continue;
        
        weightInCluster = cellLink.weight();
        // found cell, no need to loop over other cells
        break;
      }
      
      if (weightInCluster < 0) continue;
        
      // Check if cell was already found in a previous cluster
      if (weightInPreviousCluster < 0) {
        // Cell not found in a previous cluster. 
        // Have to check whether cell is shared with other cluster
        shotToClusterMap[index] = cluster;
        weightInPreviousCluster = weightInCluster;
      }
      else {
        // Cell has been found in a previous cluster
        // assign shot to this cluster if it has larger weight for the cell
        // otherwise the shots keeps assigned to the previous cluster
        if (weightInCluster > weightInPreviousCluster) {
            shotToClusterMap[index] = cluster;
        }
        // FIXME: why break here ? Should loop all the cluster, and find the largest weight
        break;
      }
    }
  }
  
  return shotToClusterMap;
}



std::vector<unsigned> TauPi0ClusterCreator::getShotsMatchedToCluster(const std::vector<const xAOD::PFO*>& shotPFOs,
								                                     const std::map<unsigned, const xAOD::CaloCluster*>& shotToClusterMap, 
								                                     const xAOD::CaloCluster& pi0Cluster) const {
  std::vector<unsigned> shotsMatchedToCluster;
  
  // Loop over the shots, and select those matched to the cluster
  for (unsigned index = 0; index < shotPFOs.size(); ++index) {
    auto iterator = shotToClusterMap.find(index);
    if (iterator == shotToClusterMap.end()) continue;
    if (iterator->second != &pi0Cluster) continue;
    
    shotsMatchedToCluster.push_back(index);
  }
  
  return shotsMatchedToCluster;
}



int TauPi0ClusterCreator::getNPhotons(const std::vector<const xAOD::PFO*>& shotPFOs,
				                      const std::vector<unsigned>& shotsInCluster) const {
  int totalPhotons = 0;
  
  for (unsigned index = 0; index < shotsInCluster.size(); ++index) {
    int nPhotons = 0;
    const xAOD::PFO* shotPFO = shotPFOs.at(shotsInCluster.at(index));
    if (! shotPFO->attribute(xAOD::PFODetails::PFOAttributes::tauShots_nPhotons, nPhotons)) { 
      ATH_MSG_WARNING("Can't find NHitsInEM1. Set it to 0.");
    }
    totalPhotons += nPhotons;
  }
  
  return totalPhotons;
}



std::vector<int> TauPi0ClusterCreator::getNPosECells(const xAOD::CaloCluster& cluster) const {
  std::vector<int> nPosECells(3, 0);

  const CaloClusterCellLink* cellLinks = cluster.getCellLinks();
  CaloClusterCellLink::const_iterator cellLink = cellLinks->begin();
  for (; cellLink != cellLinks->end(); ++cellLink) {
    const CaloCell* cell = static_cast<const CaloCell*>(*cellLink);
    int sampling = cell->caloDDE()->getSampling();
    
    // layer0: PS, layer1: EM1, layer2: EM2
    int layer = sampling%4;  
    if (layer < 3 && cell->e() > 0) {
      ++nPosECells[layer];
    }
  }

  return nPosECells;
}



float TauPi0ClusterCreator::getEM1CoreFrac(const xAOD::CaloCluster& cluster) const {
  float coreEnergyEM1 = 0.;
  float totalEnergyEM1 = 0.;
  
  const CaloClusterCellLink* cellLinks = cluster.getCellLinks();
  CaloClusterCellLink::const_iterator cellLink = cellLinks->begin();
  for (; cellLink != cellLinks->end(); ++cellLink) {
    const CaloCell* cell = static_cast<const CaloCell*>(*cellLink);
    
    // Only consider EM1
    int sampling = cell->caloDDE()->getSampling();
    if (sampling != 1 && sampling != 5) continue;
    
    // Only consider positive cells
    // FIXME: is the weight needed ?
    float cellEnergy = cell->e() * cellLink.weight();
    if (cellEnergy <= 0) continue;
    
    totalEnergyEM1 += cellEnergy;

    float deltaEta = cell->eta() - cluster.eta();
    float deltaPhi = P4Helpers::deltaPhi(cell->phi(), cluster.phi());
    
    // Core region: [0.05, 0.05/8]
    if(std::abs(deltaPhi) > 0.05 || std::abs(deltaEta) > 2 * 0.025/8.) continue;
    
    coreEnergyEM1 += cellEnergy;
  }
  
  if (totalEnergyEM1 <= 0.) return 0.;
  return coreEnergyEM1/totalEnergyEM1;
}



std::vector<float> TauPi0ClusterCreator::get1stEtaMomWRTCluster(const xAOD::CaloCluster& cluster) const {
  std::vector<float> deltaEtaFirstMom (3, 0.);
  std::vector<float> totalEnergy (3, 0.);

  const CaloClusterCellLink* cellLinks = cluster.getCellLinks();
  CaloClusterCellLink::const_iterator cellLink = cellLinks->begin();
  for (; cellLink != cellLinks->end(); ++cellLink) {
    const CaloCell* cell = static_cast<const CaloCell*>(*cellLink);
    
    // Only consider PS, EM1, and EM2
    int sampling = cell->caloDDE()->getSampling();
    int layer = sampling%4;
    if (layer >= 3) continue;

    // Only consider positive cells
    float cellEnergy = cell->e();
    if (cellEnergy <= 0) continue;

    float deltaEta = cell->eta() - cluster.eta();
    deltaEtaFirstMom[layer] += deltaEta * cellEnergy;
    totalEnergy[layer] += cellEnergy;
  }

  for (int layer=0; layer < 3; ++layer) {
    if (totalEnergy[layer] != 0.) {
      deltaEtaFirstMom[layer]/=std::abs(totalEnergy[layer]);
    }
    else {
      deltaEtaFirstMom[layer]=0.;
    }
  }
  
  return deltaEtaFirstMom;
}



std::vector<float> TauPi0ClusterCreator::get2ndEtaMomWRTCluster(const xAOD::CaloCluster& cluster) const {
  std::vector<float> deltaEtaSecondMom (3, 0.);
  std::vector<float> totalEnergy (3, 0.);

  const CaloClusterCellLink* cellLinks = cluster.getCellLinks();
  CaloClusterCellLink::const_iterator cellLink = cellLinks->begin();
  for (; cellLink != cellLinks->end(); ++cellLink) {
    const CaloCell* cell = static_cast<const CaloCell*>(*cellLink);
    
    // Only consider PS, EM1, and EM2
    int sampling = cell->caloDDE()->getSampling();
    int layer = sampling%4;
    if (layer >= 3) continue;

    // Only consider positive cells
    float cellEnergy=cell->e();
    if (cellEnergy <= 0) continue;

    float deltaEta = cell->eta() - cluster.eta();
    deltaEtaSecondMom[layer] += deltaEta * deltaEta * cellEnergy;
    totalEnergy[layer] += cellEnergy;
  }

  for (int layer=0; layer < 3; ++layer) {
    if (totalEnergy[layer] != 0.) {
      deltaEtaSecondMom[layer]/=std::abs(totalEnergy[layer]);
    }
    else {
      deltaEtaSecondMom[layer]=0.;
    }
  }
  
  return deltaEtaSecondMom;
}



StatusCode TauPi0ClusterCreator::configureNeutralPFO(const xAOD::CaloCluster& cluster,
                                                     const xAOD::CaloClusterContainer& pi0ClusterContainer,
                                                     const xAOD::TauJet& tau,
                                                     const std::vector<const xAOD::PFO*>& shotPFOs, 
                                                     const std::map<unsigned, const xAOD::CaloCluster*>& shotToClusterMap,
                                                     xAOD::PFO& neutralPFO) const {
  // Set the property of the PFO
  // -- Four momentum: not corrected yet
  neutralPFO.setP4(cluster.pt(), cluster.eta(), cluster.phi(), cluster.m());
  
  // -- Default value
  neutralPFO.setBDTPi0Score(-9999.);
  neutralPFO.setCharge(0);
  neutralPFO.setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, -1);

  // -- CENTER_MAG
  double CENTER_MAG = 0.0;
  if (!cluster.retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_MAG, CENTER_MAG)) {
    ATH_MSG_WARNING("Couldn't retrieve CENTER_MAG moment. Set it to 0.");
  }
  neutralPFO.setCenterMag( (float) CENTER_MAG);
  
  // -- Number of photons 
  std::vector<unsigned> shotsInCluster = getShotsMatchedToCluster(shotPFOs, shotToClusterMap, cluster);
  int NHitsInEM1 = getNPhotons(shotPFOs, shotsInCluster);
  neutralPFO.setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NHitsInEM1, NHitsInEM1);
  
  // -- Energy at each layer
  float eEM1 = cluster.eSample(CaloSampling::EMB1) + cluster.eSample(CaloSampling::EME1);
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_energy_EM1, eEM1);
  
  float eEM2 = cluster.eSample(CaloSampling::EMB2) + cluster.eSample(CaloSampling::EME2);
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_energy_EM2, eEM2);
  
  // -- Number of positive cells in each layer
  std::vector<int> nPosECells = getNPosECells(cluster);
  neutralPFO.setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_PS,  nPosECells.at(0));
  neutralPFO.setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM1, nPosECells.at(1));
  neutralPFO.setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM2, nPosECells.at(2));
 
  // -- Core Fraction of the energy in EM1 
  float EM1CoreFrac = getEM1CoreFrac(cluster);
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_EM1CoreFrac, EM1CoreFrac);

  // -- First moment of deltaEta(cluster, cell) in EM1 and EM2 
  std::vector<float> deltaEtaFirstMom = get1stEtaMomWRTCluster(cluster);
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM1, deltaEtaFirstMom.at(1));
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM2, deltaEtaFirstMom.at(2));
  
  // -- Second moment of deltaEta(cluster, cell) in EM1 and EM2
  std::vector<float> secondEtaWRTClusterPositionInLayer = get2ndEtaMomWRTCluster(cluster);
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM1, secondEtaWRTClusterPositionInLayer.at(1));
  neutralPFO.setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM2, secondEtaWRTClusterPositionInLayer.at(2));

  // -- Retrieve cluster moments
  using Moment = xAOD::CaloCluster::MomentType;
  using Attribute = xAOD::PFODetails::PFOAttributes; 
  const std::array< std::pair<Moment, Attribute>, 12> momentAttributePairs {{
      {Moment::FIRST_ETA, Attribute::cellBased_FIRST_ETA},
      {Moment::SECOND_R, Attribute::cellBased_SECOND_R}, 
      {Moment::SECOND_LAMBDA, Attribute::cellBased_SECOND_LAMBDA},
      {Moment::DELTA_PHI, Attribute::cellBased_DELTA_PHI},
      {Moment::DELTA_THETA, Attribute::cellBased_DELTA_THETA},
      {Moment::CENTER_LAMBDA, Attribute::cellBased_CENTER_LAMBDA},
      {Moment::LATERAL, Attribute::cellBased_LATERAL},
      {Moment::LONGITUDINAL, Attribute::cellBased_LONGITUDINAL},
      {Moment::ENG_FRAC_EM, Attribute::cellBased_ENG_FRAC_EM},
      {Moment::ENG_FRAC_MAX, Attribute::cellBased_ENG_FRAC_MAX},
      {Moment::ENG_FRAC_CORE, Attribute::cellBased_ENG_FRAC_CORE},
      {Moment::SECOND_ENG_DENS, Attribute::cellBased_SECOND_ENG_DENS}
  }};
  
  for (const auto& [moment, attribute] : momentAttributePairs) {
    double value = 0.0;
    if (! cluster.retrieveMoment(moment, value)) {
      ATH_MSG_WARNING("Cound not retrieve " << moment);
    }
    neutralPFO.setAttribute(attribute, static_cast<float>(value));
  }

  // -- Element link to the cluster 
  ElementLink<xAOD::CaloClusterContainer> clusElementLink;
  clusElementLink.toContainedElement(pi0ClusterContainer, &cluster);
  neutralPFO.setClusterLink( clusElementLink );
 
  // -- Element link to the shots
  std::vector<ElementLink<xAOD::IParticleContainer>> shotlinks;
  for (unsigned index = 0; index < shotsInCluster.size(); ++index) {
    ElementLink<xAOD::PFOContainer> shotPFOElementLink = tau.shotPFOLinks().at(shotsInCluster.at(index));
    ElementLink<xAOD::IParticleContainer> shotElementLink;
    shotPFOElementLink.toPersistent();
    shotElementLink.resetWithKeyAndIndex(shotPFOElementLink.persKey(), shotPFOElementLink.persIndex()); 
    if (!shotElementLink.isValid()) {
      ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
    }
    shotlinks.push_back(shotElementLink);
  }
  if(!neutralPFO.setAssociatedParticleLinks( xAOD::PFODetails::TauShot,shotlinks)) { 
    ATH_MSG_WARNING("Couldn't add shot links to neutral PFO!");
  }

  return StatusCode::SUCCESS;
}



StatusCode TauPi0ClusterCreator::configureHadronicPFO(const xAOD::CaloCluster& cluster, 
                                                      double clusterEnergyHad, 
                                                      xAOD::PFO& hadronicPFO) const {
  double clusterPtHad = clusterEnergyHad/std::cosh(cluster.eta());
  hadronicPFO.setP4(clusterPtHad, cluster.eta(), cluster.phi(), 0.);

  return StatusCode::SUCCESS;
}

#endif
