/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUTRACKEVENT_TAUJETTRUTHMAP_H
#define TAUTRACKEVENT_TAUJETTRUTHMAP_H

#include <map>
//#include "TauTrackEvent/TruthTau.h"
//#include "tauEvent/TauJet.h"
namespace Analysis {
class TauJet;
}

namespace TauID {
    class TruthTau;

 /**
     * @brief Map between Analysis::TauJet and TauID::TruthTau used for truth matching
     *
     * TauJets without truth match are mapped to NULL.
     * This is a multimap even though one TauJet should be matched to exactly one
     * TruthTau. Multimap is needed, because all TruthTaus without matched TauJet
     * (missing reconstructed taus) are matched to NULL, i.e. several NULL TauJet pointer
     * may exist which map to different TruthTaus.
     *
     * @author Sebastian Fleischmann
     */
  typedef std::multimap<const Analysis::TauJet*, const TauID::TruthTau*> TauJetTruthMap;
}


#endif //TAUTRACKEVENT_TAUJETTRUTHMAP_H
