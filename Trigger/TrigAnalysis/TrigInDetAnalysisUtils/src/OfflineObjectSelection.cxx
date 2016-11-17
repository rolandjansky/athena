
#include "TrkTrackSummary/TrackSummary.h"
#include "TrigInDetAnalysisUtils/OfflineObjectSelection.h"


#ifdef  XAODTRACKING_TRACKPARTICLE_H

// xAOD offline electron selection
// selection: 0 take all; 1,4 tight; 2,5 medium; 3,6 loose; <4 cut based, >3 likelihood
bool TIDA::isGoodOffline(const xAOD::Electron& elec, const unsigned int selection, double ETOffline ) {

  double sintheta = std::sin( 2*std::atan(std::exp(-elec.eta())) );
  double et = elec.e()*sintheta;
  if ( std::fabs(et)<ETOffline ) return false;

  if ( std::fabs(elec.e())<ETOffline ) return false; 
  if      (selection == 1) return elec.passSelection("Tight");
  else if (selection == 2) return elec.passSelection("Medium");
  else if (selection == 3) return elec.passSelection("Loose");
  else if (selection == 4) return elec.passSelection("LHTight");  
  else if (selection == 5) return elec.passSelection("LHMedium");
  else if (selection == 6) return elec.passSelection("LHLoose");
  return true; 
}

// Currently no xAOD offline muon selection implemented
bool TIDA::isGoodOffline(const xAOD::Muon& /*muon*/, double ) { return true; }


// xAOD offline tau selection
// selection: 0 = take all, 1 = tight, 2 = medium, 3 = loose, any other unsigned int = no JetBDTSig requirement
bool TIDA::isGoodOffline(const xAOD::TauJet& tau, bool doThreeProng, const unsigned int selection, double EtCutOffline ) {

  if ( selection== 0) return true;

  /// hmmm	           
  TLorentzVector TauTLV = tau.p4();
  double        eta_Tau = TauTLV.Eta();
  double         et_Tau = TauTLV.Et();

  if ( std::fabs(eta_Tau)>2.47 )           return false;
  if ( std::fabs(et_Tau)<EtCutOffline ) return false;

  int ntrack_Tau = tau.nTracks();

  if      ( doThreeProng==false && ntrack_Tau!=1 ) return false;
  else if ( doThreeProng==true  && ntrack_Tau!=3 ) return false;

  // Could this selection just be replaced by a string that is passed in? Or an enumerate if the function is needed to be called when incrementing an int for the selection?
  bool good_tau = false;

  if      ( selection == 1 ) good_tau = tau.isTau(xAOD::TauJetParameters::JetBDTSigTight);
  else if ( selection == 2 ) good_tau = tau.isTau(xAOD::TauJetParameters::JetBDTSigMedium);
  else if ( selection == 3 ) good_tau = tau.isTau(xAOD::TauJetParameters::JetBDTSigLoose);
  else                       good_tau = true;


  //bool not_a_electron = !( tauisTau(xAOD::TauJetParameters::EleBDTMedium) );
  //bool not_a_muon = !( tauisTau(xAOD::TauJetParameters::MuonVeto) );
  //bool best_tau = good_tau && not_a_electron && not_a_muon;

  //  bool best_tau = good_tau;
  //  if      (!best_tau) return false;
  //  else if (best_tau)  return true;

  //  return false; // Safeguard by defaulting to returning false if the rest of the method does not function correctly and this return point is reached

  return good_tau;
}


#else



bool TIDA::isGoodOffline(const Analysis::Electron& elec) {
  if ( elec.trackParticle() && elec.isem(egammaPID::ElectronMediumPP)==0 ) {
    //medium++ ID
    if (elec.author() == 1 || elec.author() == 3) {
      //Author = track or track+calo
      return true;
    }
  }
  return false;
}


bool TIDA::isGoodOffline(const Analysis::Muon& muon) {
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



bool TIDA::isGoodOffline(const Analysis::TauJet& tau, const bool doThreeProng, const double EtCutOffline) {
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
    if (tau.et() < EtCutOffline) return false;
    if (tau.eta() > 2.47) return false;
    if (tau.tauID()->isTau(TauJetParameters::EleBDTMedium) ) return false;
    if (tau.tauID()->isTau(TauJetParameters::MuonVeto) ) return false;
    if (fabs(tau.charge())!=1) return false;
    return true;
  }
  return false;
}

#endif  
