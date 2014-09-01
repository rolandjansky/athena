#ifndef TrigInDetAnalysisExample_OfflineObjectSelection_H
#define TrigInDetAnalysisExample_OfflineObjectSelection_H
#include "egammaEvent/Electron.h"

#include "muonEvent/Muon.h"

#include "tauEvent/TauJet.h"

namespace TrigInDetAnalysis {
	bool IsGoodOffline(const Analysis::Electron& elec); 
	bool IsGoodOffline(const Analysis::Muon& muon); 
	bool IsGoodOffline(const Analysis::TauJet& tau, const bool doThreeProng = false, const double tauEtCutOffline = 0.0);
}
#endif
