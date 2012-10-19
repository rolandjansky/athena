/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TrackBasedTruthMatchUtility
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Utililty class to match TauJets to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TAUTOOLS_TRACKBASEDTRUTHMATCHUTILITY_H
#define TAUTOOLS_TRACKBASEDTRUTHMATCHUTILITY_H

#include <map>
//#include "TauTrackEvent/TruthTau.h"
#include "DataModel/DataVector.h"
#include "GaudiKernel/MsgStream.h"

class MsgStream;
class StoreGateSvc;
class I4Momentum;
class TruthParticleContainer;
class JetCollection;
namespace Rec{
class TrackParticle;
class TrackParticleContainer;
}
class TrackParticleTruthCollection;

namespace HepMC {
   class GenParticle;
}
namespace TauID {
class TruthTau;

typedef std::multimap<int, unsigned int> BarcodeIndexMap;
typedef BarcodeIndexMap::const_iterator BarcodeIndexMapIterator;

class TruthTau;

/** @class TrackBasedTruthMatchUtility
    Utility to match reconstructed taus to TruthTaus. Do the matching based
          on the TrackParticleTruth and select the reconstructed tau with highest number
          of matching tracks. Delta R matching is done on non-matched
          truth taus to recover taus without any reco track (esp. 1-prong with
          hadronic interaction).

    @author Sebastian Fleischmann
*/
class TrackBasedTruthMatchUtility {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    TrackBasedTruthMatchUtility(double matchMaxDR, double maxAbsEta, double minPt, double minTrackMatchProbability, MsgStream log);
    ~TrackBasedTruthMatchUtility ();
    /** create a tau seed to truth map. The map belongs to
        you, the user, and so you must take care of deletion of it.*/
    const std::multimap<const I4Momentum*, const TauID::TruthTau*>* matchToTruth(
            const DataVector<const TauID::TruthTau>& selectTruthTaus,
            const BarcodeIndexMap& barcodeSeedMap,
            const DataVector<const I4Momentum>& tauSeedColl,
            const std::vector<unsigned int>& nTracksInSeed,
            const JetCollection* truthJetCont ) const;

    const HepMC::GenParticle* getTruth(const Rec::TrackParticle* trackParticle,
                                              const TrackParticleTruthCollection* mcpartColl,
                                              const Rec::TrackParticleContainer* trackColl) const;

    const DataVector<const TauID::TruthTau>* selectTruthTaus(const TruthParticleContainer& truthTauCont) const;

private:
    mutable MsgStream               m_log;
    double      m_matchMaxDR;      //!< maximum delta R for match
    double      m_maxAbsEta;       //!< maximum abs(eta)
    double      m_minPt;           //!< minimum pt
    double      m_minTrackMatchProbability;
};
} // end of namespace TauID
#endif // TAUTOOLS_TRACKBASEDTRUTHMATCHUTILITY_H
