/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TauJetTrackBasedTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauJets to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TAUTOOLS_TAUJETTRACKBASEDTRUTHMATCHTOOL_H
#define TAUTOOLS_TAUJETTRACKBASEDTRUTHMATCHTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// interface
#include "TauTrackTools/ITauJetTruthMatchTool.h"

namespace Rec{
class TrackParticle;
class TrackParticleContainer;
}
class TrackParticleTruthCollection;

namespace TauID {
class TrackBasedTruthMatchUtility;

/** @class TauJetTrackBasedTruthMatchTool
    Tool to match TauJets to TruthTaus. Does the matching based
          on the TrackParticleTruth and selects the tau jet with highest number
          of matching tracks. Delta R matching is done on non-matched
          truth taus to recover taus without any reco track (esp. 1-prong with
          hadronic interaction).
    @author Sebastian Fleischmann
*/
class TauJetTrackBasedTruthMatchTool : public AthAlgTool,
    virtual public TauID::ITauJetTruthMatchTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    TauJetTrackBasedTruthMatchTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TauJetTrackBasedTruthMatchTool ();
    /** AlgTool init */
    virtual StatusCode initialize();
    /** AlgTool finalize */
    virtual StatusCode finalize  ();

    /** create a tau jet to truth map. The map belongs to
        you, the user, and so you must take care of deletion of it.*/
    virtual const TauID::TauJetTruthMap* matchCandidatesToTruth(
                const TruthParticleContainer& truthTauCont,
                const Analysis::TauJetContainer& tauJetCont ) const;
private:

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
    
    double      m_matchMaxDR;      //!< maximum delta R for match
    double      m_maxAbsEta;       //!< maximum abs(eta)
    double      m_minPt;           //!< minimum pt
    double      m_minTrackMatchProbability;
    std::string m_inputTrackParticleTruthCollectionName;    //!< SG key of the input track particle truth collection
    std::string m_inputTrackParticleContainerName;          //!< SG key of the input track particle collection
    bool        m_doTruthJetMatching;                       //!< jobOption: match fake tau jets to TruthJets
    std::string m_inputTruthJetContainerName;               //!< SG key of the input Truth Jet Container used for fake matching

    const TrackBasedTruthMatchUtility*     m_truthMatchUtil;

};
} // end of namespace TauID
#endif // TAUTOOLS_TAUJETTRACKBASEDTRUTHMATCHTOOL_H
