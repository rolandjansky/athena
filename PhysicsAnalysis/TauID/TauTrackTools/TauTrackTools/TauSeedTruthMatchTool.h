/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class TauSeedTruthMatchTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Tool to match TauSeeds to TruthTaus
///////////////////////////////////////////////////////////////////
// sebastian.fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#ifndef TAUTOOLS_TAUSEEDTRUTHMATCHTOOL_H
#define TAUTOOLS_TAUSEEDTRUTHMATCHTOOL_H
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// Interface
#include "TauTrackTools/ITauSeedTruthMatchTool.h"


class StoreGateSvc;

namespace TauID {

/** @class TauSeedTruthMatchTool
    Tool to match TauSeeds to TruthTaus.
    @author Sebastian Fleischmann
*/
class TauSeedTruthMatchTool : public AthAlgTool,
    virtual public ITauSeedTruthMatchTool {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////

public:
    /** AlgTool Constructor */
    TauSeedTruthMatchTool(const std::string&,const std::string&,const IInterface*);
    virtual ~TauSeedTruthMatchTool ();
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

    double       m_matchMaxDR;      //!< maximum delta R for match
    double       m_maxAbsEta;       //!< maximum abs(eta)
    double       m_minPt;           //!< minimum pt
    bool        m_doTruthJetMatching;                       //!< jobOption: match fake tau jets to TruthJets
    std::string m_inputTruthJetContainerName;               //!< SG key of the input Truth Jet Container used for fake matching

};
} // end of namespace PanTau
#endif // PANTAUALGS_TAUSEEDTRUTHMATCHTOOL_H
