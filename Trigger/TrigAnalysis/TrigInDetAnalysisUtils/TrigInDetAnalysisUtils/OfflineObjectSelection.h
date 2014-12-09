#ifndef TrigInDetAnalysisExample_OfflineObjectSelection_H
#define TrigInDetAnalysisExample_OfflineObjectSelection_H

// xAOD
// not actual XAOD, but this file safely includes 
// the xAOD::TrackParticle header if it exists
// which we use to decide whether this code should 
// include all the xAOD classes or not
#include "TrkParticleCreator/TrackParticleCreatorTool.h"

/// Test for xAOD
#ifdef XAODTRACKING_TRACKPARTICLE_H

#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"
       
namespace TrigInDetAnalysis {
  bool IsGoodOffline(const xAOD::Electron& elec); 
  bool IsGoodOffline(const xAOD::Muon& muon); 
  bool IsGoodOffline(const xAOD::TauJet& tau, bool doThreeProng = false, double tauEtCutOffline = 0.0);
}

#else 

#include "egammaEvent/Electron.h"
#include "muonEvent/Muon.h"
#include "tauEvent/TauJet.h"
namespace TrigInDetAnalysis {	
  bool IsGoodOffline(const Analysis::Electron& elec); 
  bool IsGoodOffline(const Analysis::Muon& muon); 	
  bool IsGoodOffline(const Analysis::TauJet& tau, bool doThreeProng = false, double tauEtCutOffline = 0.0);
}
#endif

#endif  // TrigInDetAnalysisExample_OfflineObjectSelection_H
