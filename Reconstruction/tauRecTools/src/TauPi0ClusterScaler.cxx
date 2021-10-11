/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TauPi0ClusterScaler.h"

#include "xAODTau/TauJet.h"
#include "xAODPFlow/PFO.h"
#include "xAODCaloEvent/CaloVertexedTopoCluster.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include <vector>
#include <map>


TauPi0ClusterScaler::TauPi0ClusterScaler(const std::string& name) :
    TauRecToolBase(name) {
}



StatusCode TauPi0ClusterScaler::executePi0ClusterScaler(xAOD::TauJet& tau, 
                                                        xAOD::PFOContainer& neutralPFOContainer, 
                                                        xAOD::PFOContainer& chargedPFOContainer) const {
  // Clear vector of cell-based charged PFO Links, which are required when running xAOD 
  tau.clearProtoChargedPFOLinks();
 
  // Only run on 1-5 prong taus 
  if (tau.nTracks() == 0 or tau.nTracks() >5) { 
    return StatusCode::SUCCESS;
  }
 
  ATH_MSG_DEBUG("Process a new tau candidate, addreess " << &tau
                  << ", e: " << tau.pt()
                  << ", eta: " << tau.eta()
                  << ", pt: " << tau.pt());

  // Create new proto charged PFOs
  createChargedPFOs(tau, chargedPFOContainer);
  
  // Associate hadronic PFOs to charged PFOs using extrapolated positions in HCal
  associateHadronicToChargedPFOs(tau, chargedPFOContainer);
  
  // Associate charged PFOs to neutral PFOs using extrapolated positions in ECal
  associateChargedToNeutralPFOs(tau, neutralPFOContainer);
  
  // Estimate charged PFO EM energy and subtract from neutral PFOs
  subtractChargedEnergyFromNeutralPFOs(tau, neutralPFOContainer);

  for (xAOD::PFO* pfo : neutralPFOContainer) {
    ATH_MSG_DEBUG("Final Neutral PFO, address " << pfo
                  << ", e: " << pfo->pt()
                  << ", eta: " << pfo->eta()
                  << ", pt: " << pfo->pt());
  }

  return StatusCode::SUCCESS;
}



void TauPi0ClusterScaler::clearAssociatedParticleLinks(xAOD::PFOContainer& pfoContainer, xAOD::PFODetails::PFOParticleType type) const {
  std::vector<ElementLink<xAOD::IParticleContainer>> emptyLinks;
  
  for (xAOD::PFO* pfo : pfoContainer) {
    pfo->setAssociatedParticleLinks(type, emptyLinks);
  }
}



void TauPi0ClusterScaler::createChargedPFOs(xAOD::TauJet& tau, xAOD::PFOContainer& chargedPFOContainer) const {
  for (const auto& tauTrackLink : tau.tauTrackLinks(xAOD::TauJetParameters::classifiedCharged)) {
    if (not tauTrackLink.isValid()) {
      ATH_MSG_WARNING("Invalid tauTrackLink");
      continue;
    }
    const xAOD::TauTrack* tauTrack = (*tauTrackLink);
    
    // Create charged PFO
    xAOD::PFO* chargedPFO = new xAOD::PFO();
    chargedPFOContainer.push_back(chargedPFO);

    // Set properties
    chargedPFO->setCharge(tauTrack->track()->charge());
    chargedPFO->setP4(tauTrack->p4());
    
    // Link to track
    if (not chargedPFO->setTrackLink(tauTrack->trackLinks().at(0))) {
      ATH_MSG_WARNING("Could not add Track to PFO");
    }

    // FIXME: Better to change xAOD::PFODetails::CaloCluster, it is confusing 
    if (not chargedPFO->setAssociatedParticleLink(xAOD::PFODetails::CaloCluster, tauTrackLink)) {
      ATH_MSG_WARNING("Could not add TauTrack to PFO");
    }

    tau.addProtoChargedPFOLink(ElementLink<xAOD::PFOContainer>(chargedPFO, chargedPFOContainer));
  }
}



float TauPi0ClusterScaler::getExtrapolatedPosition(const xAOD::PFO& chargedPFO, xAOD::TauJetParameters::TrackDetail detail) const {
  float position = -10.0;
  
  // Obtain the associated TauTrack  
  std::vector<const xAOD::IParticle*> tauTrackParticles;
  // FIXME: The type here is confusing
  chargedPFO.associatedParticles(xAOD::PFODetails::CaloCluster, tauTrackParticles);
  if (tauTrackParticles.empty()) {
    ATH_MSG_WARNING("ChargedPFO has no associated TauTrack, will set -10.0 to " << detail);
    return -10.0;
  }

  const xAOD::TauTrack* tauTrack = dynamic_cast<const xAOD::TauTrack*>(tauTrackParticles.at(0));
  if (not tauTrack) {
    ATH_MSG_WARNING("Failed to retrieve TauTrack from ChargedPFO, will set -10.0 to " << detail);
    return -10.0;
  }
  
  if( not tauTrack->detail(detail, position)) {
    ATH_MSG_WARNING("Failed to retrieve extrapolated chargedPFO position, will set -10.0 to " << detail);
    return -10.0;
  }
  
  return position; 
}



void TauPi0ClusterScaler::associateHadronicToChargedPFOs(const xAOD::TauJet& tau, xAOD::PFOContainer& chargedPFOContainer) const {
  std::map< xAOD::PFO*,std::vector< ElementLink< xAOD::IParticleContainer > > > linkMap;
 
  // For each hadronic PFO, associate it to the closest charged PFO. It assumes that one hadronic PFO comes from at 
  // most one charged PFO.
  for (const auto& hadPFOLink : tau.hadronicPFOLinks()) {
    if (not hadPFOLink.isValid()) {
      ATH_MSG_WARNING("Invalid hadPFOLink");
      continue;
    }
    ATH_MSG_DEBUG("hadPFO " << hadPFOLink.index() << ", eta: " << (*hadPFOLink)->eta() << ", phi: " << (*hadPFOLink)->phi() );
    
    // Assign hadPFO to closest extrapolated chargedPFO track within dR < 0.4
    xAOD::PFO* chargedPFOMatch = nullptr;
    float dRmin = 0.4;
    
    for(size_t i=0; i<tau.nProtoChargedPFOs(); i++) {
      xAOD::PFO* chargedPFO = chargedPFOContainer.at( tau.protoChargedPFO(i)->index() );
      
      float etaCalo = getExtrapolatedPosition(*chargedPFO, xAOD::TauJetParameters::CaloSamplingEtaHad);
      float phiCalo = getExtrapolatedPosition(*chargedPFO, xAOD::TauJetParameters::CaloSamplingPhiHad);
    
      float dR = xAOD::P4Helpers::deltaR((**hadPFOLink), etaCalo, phiCalo, false);
      if (dR < dRmin){
        dRmin = dR;
        chargedPFOMatch = chargedPFO;
      }
    }
    
    if( not chargedPFOMatch ){
      ATH_MSG_DEBUG("Unassigned Hadronic PFO");
      continue; 
    }

    // create link to had PFO (add to chargedPFO later)
    ElementLink< xAOD::IParticleContainer > newHadLink( hadPFOLink.dataID(), hadPFOLink.index() );
    if (not newHadLink.isValid()){
        ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
        continue;
    }
    
    if( not linkMap.count(chargedPFOMatch) ) {
        linkMap[chargedPFOMatch] = std::vector< ElementLink< xAOD::IParticleContainer > >();
    }

    linkMap[chargedPFOMatch].push_back(newHadLink);
  }

  // finally set hadronic PFO links (note: we use existing TauShot enum)
  for (auto [k,v] : linkMap) {
    if(not k->setAssociatedParticleLinks(xAOD::PFODetails::TauShot, v))
      ATH_MSG_WARNING("Couldn't add hadronic PFO links to charged PFO!");
  }
}



void TauPi0ClusterScaler::associateChargedToNeutralPFOs(const xAOD::TauJet& tau, xAOD::PFOContainer& neutralPFOContainer) const {
  std::map< xAOD::PFO*,std::vector< ElementLink< xAOD::IParticleContainer > > > linkMap;
  for (const auto& chargedPFOLink : tau.protoChargedPFOLinks()) {
    if (not chargedPFOLink.isValid()) {
      ATH_MSG_WARNING("Invalid protoChargedPFOLink");
      continue;
    }
    const xAOD::PFO* chargedPFO = (*chargedPFOLink);
    
    float etaCalo = getExtrapolatedPosition(*chargedPFO, xAOD::TauJetParameters::CaloSamplingEtaEM);
    float phiCalo = getExtrapolatedPosition(*chargedPFO, xAOD::TauJetParameters::CaloSamplingPhiEM);
    
    // Assign extrapolated chargedPFO to closest neutralPFO within dR<0.04
    xAOD::PFO* neutralPFOMatch = nullptr;
    
    float dRmin = 0.04; 
    for(size_t i=0; i<tau.nProtoNeutralPFOs(); i++) {
      xAOD::PFO* neutralPFO = neutralPFOContainer.at( tau.protoNeutralPFO(i)->index() );

      // FIXME: cluster p4 is not corrected to the tau axis 
      float dR = xAOD::P4Helpers::deltaR((*neutralPFO->cluster(0)), etaCalo, phiCalo, false);
      if (dR < dRmin){
        dRmin = dR;
        neutralPFOMatch = neutralPFO;
      }
    }
    
    if (not neutralPFOMatch){
      ATH_MSG_DEBUG("Unassigned Charged PFO");
      continue; 
    }

    // create link to charged PFO 
    ElementLink<xAOD::IParticleContainer> newChargedLink( chargedPFOLink.dataID(), chargedPFOLink.index() );
    if (not newChargedLink.isValid()){
      ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
      continue;
    }
        
    if( not linkMap.count(neutralPFOMatch) ) {
      linkMap[neutralPFOMatch] = std::vector< ElementLink< xAOD::IParticleContainer > >();
    }

    linkMap[neutralPFOMatch].push_back(newChargedLink);
  }
    
  // Finally set charged PFO links,
  for (auto [k,v] : linkMap) {
    if(not k->setAssociatedParticleLinks(xAOD::PFODetails::Track,v)) {
      ATH_MSG_WARNING("Couldn't add charged PFO links to neutral PFO!");
    }
  }
}



void TauPi0ClusterScaler::subtractChargedEnergyFromNeutralPFOs(const xAOD::TauJet& tau, xAOD::PFOContainer& neutralPFOContainer) const {

  for(size_t i=0; i<tau.nProtoNeutralPFOs(); i++) {
    xAOD::PFO* neutralPFO = neutralPFOContainer.at( tau.protoNeutralPFO(i)->index() );
  
    // Get associated charged PFOs
    std::vector<const xAOD::IParticle*> chargedPFOs;
    neutralPFO->associatedParticles(xAOD::PFODetails::Track, chargedPFOs);
    if (chargedPFOs.empty()) {
      ATH_MSG_DEBUG("No associated charged to subtract"); 
      continue;
    }
    ATH_MSG_DEBUG("Associated charged PFOs: " << chargedPFOs.size() );

    // estimate charged EM energy and subtract
    float neutralEnergy = neutralPFO->e();
    for (const xAOD::IParticle* chargedParticle : chargedPFOs) {
      const xAOD::PFO* chargedPFO = dynamic_cast<const xAOD::PFO*>(chargedParticle);
      if( not chargedPFO ){
          ATH_MSG_WARNING("Failed to downcast IParticle ptr: " << chargedParticle << ", to ChargedPFO! " );
          continue;
      }
      float chargedEMEnergy = chargedPFO->e();
      
      std::vector<const xAOD::IParticle*> hadPFOs;
      chargedPFO->associatedParticles(xAOD::PFODetails::TauShot, hadPFOs);
      for (const auto *hadPFO : hadPFOs) {
          chargedEMEnergy -= hadPFO->e();
      }

      if( chargedEMEnergy < 0.0 ) chargedEMEnergy = 0.0;
      neutralEnergy -= chargedEMEnergy;
      ATH_MSG_DEBUG("Subtracting charged energy: " << chargedEMEnergy );
    } 
    float neutralPt = neutralEnergy / std::cosh(neutralPFO->eta());
    if (neutralPt <= 100.) neutralPt = 100.0;
    
    ATH_MSG_DEBUG("Neutral PFO pt, original: " << neutralPFO->pt() << "  subtracted: " << neutralPt); 
    neutralPFO->setP4(neutralPt , neutralPFO->eta(), neutralPFO->phi(), neutralPFO->m());
  }
}
