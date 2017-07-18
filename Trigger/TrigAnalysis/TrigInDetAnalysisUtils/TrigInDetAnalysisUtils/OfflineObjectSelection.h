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

       
namespace TIDA {
  bool isGoodOffline(const xAOD::Electron& elec, const unsigned int selection=0, double ETOffline=0 ); 
  bool isGoodOffline(const xAOD::Muon&     muon, const unsigned int selection=0, double ETOffline=0 );
  bool isGoodOffline(const xAOD::TauJet&   tau,  const unsigned int selection=0, int requireNtracks=0, double EtOffline=0.0 ); 
}

#else 

#include "egammaEvent/Electron.h"
#include "muonEvent/Muon.h"
#include "tauEvent/TauJet.h"

namespace TIDA {	
  bool isGoodOffline(const Analysis::Electron& elec); 
  bool isGoodOffline(const Analysis::Muon& muon); 	
  bool isGoodOffline(const Analysis::TauJet& tau, bool doThreeProng=false, double EtCutOffline=0.0);
}
#endif

#endif  // TrigInDetAnalysisExample_OfflineObjectSelection_H
