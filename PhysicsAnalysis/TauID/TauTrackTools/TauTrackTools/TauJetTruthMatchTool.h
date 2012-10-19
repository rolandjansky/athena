/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TauJetTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauJets to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TAUTOOLS_TAUJETTRUTHMATCHTOOL_H
#define TAUTOOLS_TAUJETTRUTHMATCHTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// interface
#include "TauTrackTools/ITauJetTruthMatchTool.h"



namespace TauID {

/** @class TauJetTruthMatchTool
    Tool to match TauJets to TruthTaus based on a delta R matching.
    @author Sebastian Fleischmann
*/
class TauJetTruthMatchTool : public AthAlgTool,
    virtual public TauID::ITauJetTruthMatchTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    TauJetTruthMatchTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TauJetTruthMatchTool ();
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
    bool        m_doTruthJetMatching;                       //!< jobOption: match fake tau jets to TruthJets
    std::string m_inputTruthJetContainerName;               //!< SG key of the input Truth Jet Container used for fake matching

};
} // end of namespace TauID
#endif // TAUTOOLS_TAUJETTRUTHMATCHTOOL_H
