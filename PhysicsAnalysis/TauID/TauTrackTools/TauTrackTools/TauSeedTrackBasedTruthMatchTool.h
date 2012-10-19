/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TauSeedTrackBasedTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TAUTOOLS_TAUSEEDTRACKBASEDTRUTHMATCHTOOL_H
#define TAUTOOLS_TAUSEEDTRACKBASEDTRUTHMATCHTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// interface
#include "TauTrackTools/ITauSeedTruthMatchTool.h"

#include <string>

class StoreGateSvc;

namespace Rec{
class TrackParticle;
class TrackParticleContainer;
}
class TrackParticleTruthCollection;

namespace TauID{
class TrackBasedTruthMatchUtility;


/** @class TauSeedTrackBasedTruthMatchTool
    Tool to match TauSeeds to TruthTaus. Do the matching based
          on the TrackParticleTruth and select the seed with highest number
          of matching tracks. Delta R matching is done on non-matched
          truth taus to recover taus without any reco track (esp. 1-prong with
          hadronic interaction).

    @author Sebastian Fleischmann
*/
class TauSeedTrackBasedTruthMatchTool : public AthAlgTool,
    virtual public ITauSeedTruthMatchTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    TauSeedTrackBasedTruthMatchTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TauSeedTrackBasedTruthMatchTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** create a tau seed to truth map. The map belongs to
        you, the user, and so you must take care of deletion of it.*/
    virtual const PanTau::SeedTruthMap* matchSeedsToTruth(
                const TruthParticleContainer& truthTauCont,
                const PanTau::TauSeedCollection& tauSeedColl ) const;

private:
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
    
    /** class member version of retrieving StoreGate */
    StoreGateSvc*  m_storeGate;

    double      m_matchMaxDR;      //!< maximum delta R for match
    double      m_maxAbsEta;       //!< maximum abs(eta)
    double      m_minPt;           //!< minimum pt
    double      m_minTrackMatchProbability;
    std::string m_inputTrackParticleTruthCollectionName;    //!< SG key of the input track particle truth collection
    std::string m_inputTrackParticleContainerName;          //!< SG key of the input track particle collection
    bool        m_doTruthJetMatching;                       //!< jobOption: match fake tau jets to TruthJets
    std::string m_inputTruthJetContainerName;               //!< SG key of the input Truth Jet Container used for fake matching

    const       TrackBasedTruthMatchUtility*     m_truthMatchUtil;
};
} // end of namespace PanTau
#endif // PANTAUALGS_TAUSEEDTRACKBASEDTRUTHMATCHTOOL_H
