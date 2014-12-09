
#include "TrkTrackSummary/TrackSummary.h"
#include "TrigInDetAnalysisUtils/OfflineObjectSelection.h"


#ifdef  XAODTRACKING_TRACKPARTICLE_H

bool TrigInDetAnalysis::IsGoodOffline(const xAOD::Electron& /*elec*/ ) { return true; }
bool TrigInDetAnalysis::IsGoodOffline(const xAOD::Muon& /*muon*/ ) { return true; } 
bool TrigInDetAnalysis::IsGoodOffline(const xAOD::TauJet& /*tau*/, bool /*doThreeProng*/, double /*tauEtCutOffline*/) { return true; }

#else

bool TrigInDetAnalysis::IsGoodOffline(const Analysis::Electron& elec) {
  if ( elec.trackParticle() && elec.isem(egammaPID::ElectronMediumPP)==0 ) {
    //medium++ ID
    if (elec.author() == 1 || elec.author() == 3) {
      //Author = track or track+calo
      return true;
    }
  }
  return false;
}


bool TrigInDetAnalysis::IsGoodOffline(const Analysis::Muon& muon) {
  if (!muon.isLoose()) {
    //Tight muon
    return false;
  }

  //Based on https://svnweb.cern.ch/trac/atlasgrp/browser/Physics/Common/OSCAR/OSSelectors/trunk/src/OSSelectMuon.cxx
  if (muon.hasInDetTrackParticle()) {
    const Rec::TrackParticle* idTrack = muon.inDetTrackParticle();
    const Trk::TrackSummary* summary = idTrack->trackSummary();
    if (!summary) return false;
    
    /// maybe select all these track based quantitied om the TIDA::Track, so we dont 
    /// have to fuss with is it xAOD/is it not xAOD etc
    // B-Layer hits
    int numberOfBLayerHits = summary->get(Trk::numberOfBLayerHits);
    bool expectBLayerHit = summary->get(Trk::expectBLayerHit);
    if (expectBLayerHit && numberOfBLayerHits<1) return false;
    
    // Pix hits
    int nPixelHits = summary->get(Trk::numberOfPixelHits);
    int numberOfPixelDeadSensors = summary->get(Trk::numberOfPixelDeadSensors);
    nPixelHits += numberOfPixelDeadSensors;
    if (nPixelHits < 2) return false;
    
    // SCT hits
    int nSCTHits = summary->get(Trk::numberOfSCTHits);
    int numberOfSCTDeadSensors   = summary->get(Trk::numberOfSCTDeadSensors);
    nSCTHits += numberOfSCTDeadSensors;
    if (nSCTHits < 6) return false;
    
    // Pix+ + SCT holes
    int numberOfPixelHoles=muon.numberOfPixelHoles();
    int numberOfSCTHoles=muon.numberOfSCTHoles();
    if ( numberOfPixelHoles+numberOfSCTHoles > 2) return false;
    
    float etaTrack = fabs(idTrack->eta());  // Need to extrapolate in principle
    int nTrtHits = summary->get(Trk::numberOfTRTHits);
    int nTrtOutliers = summary->get(Trk::numberOfTRTOutliers);
    
    if (etaTrack < 1.9) {
      if ((nTrtHits + nTrtOutliers) < 5 ) return false;
      if (nTrtOutliers >= (0.9 * (nTrtHits + nTrtOutliers))) return false;
    } else {
      if ((nTrtHits + nTrtOutliers) > 5) {
	if (nTrtOutliers >= (0.9 * (nTrtHits + nTrtOutliers))) return false;
      }
    }
    return true;
  }
  return false;
}



bool TrigInDetAnalysis::IsGoodOffline(const Analysis::TauJet& tau, const bool doThreeProng, const double tauEtCutOffline) {
  //  std::cout << "have offline tau" << std::endl;
  bool id_flag = false;
  if (doThreeProng) { 
    bool threeprong = ( tau.numTrack() == 3 && tau.tauID()->isTau(TauJetParameters::JetBDTSigMedium) ); 
    id_flag = threeprong; 
  } 
  else {	  
    bool oneprong = ( tau.numTrack() == 1 && tau.tauID()->isTau(TauJetParameters::JetBDTSigMedium) ); 
    id_flag = oneprong;
  }

  if (id_flag) {  
    if (tau.et() < tauEtCutOffline) return false;
    if (tau.eta() > 2.47) return false;
    if (tau.tauID()->isTau(TauJetParameters::EleBDTMedium) ) return false;
    if (tau.tauID()->isTau(TauJetParameters::MuonVeto) ) return false;
    if (fabs(tau.charge())!=1) return false;
    return true;
  }
  return false;
}

#endif  
