/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_JETSEEDBUILDER_H
#define	TAUREC_JETSEEDBUILDER_H

#include "tauRec/TauToolBase.h"


/**
 * @brief Class to build tauRec seeds from topojets.
 * 
 *  Sets the jet ElementLink and basic kinematic variables in TauJet by searching a matching jet to the tau direction.
 *  Also the mass of the tau is set to 0.
 *  With tauRec4 the JetSeedBuilder method is the only one to search for a tau candidates. 
 *  The author of the tau candidate is set 1 (former calo-only seeded) and 3 (former calo+track-seeded) to keep backwards compatibility.
 *  
 * @author N.Meyer <nicom@cern.ch>
 * @author Felix Friedrich
*/

class JetSeedBuilder : public TauToolBase {
public:

    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    JetSeedBuilder(const std::string& type,
            const std::string& name,
            const IInterface * parent);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    virtual ~JetSeedBuilder();

    virtual StatusCode initialize();

    virtual StatusCode execute(TauCandidateData * data);

    virtual StatusCode eventFinalize(TauCandidateData *data);

    virtual void cleanup(TauCandidateData *) { }

private:
    std::string m_jetCollectionName;
    float m_maxJetdist;
    float m_minJetPt;
    bool m_switch_jets_em_scale;
};

#endif	/* JETSEEDBUILDER_H */

