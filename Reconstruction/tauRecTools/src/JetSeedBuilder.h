/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_JETSEEDBUILDER_H
#define	TAUREC_JETSEEDBUILDER_H

#include "tauRecTools/TauRecToolBase.h"


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

class JetSeedBuilder : public TauRecToolBase {
public:

    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    ASG_TOOL_CLASS2( JetSeedBuilder, TauRecToolBase, ITauToolBase )
    JetSeedBuilder(const std::string& name);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    virtual ~JetSeedBuilder();

    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual void print() const ;

    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }  
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }

    virtual StatusCode eventFinalize();

    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }

private:
    std::string m_jetCollectionName;
    float m_maxJetdist;
    float m_minJetPt;
    bool m_switch_jets_em_scale;
};

#endif	/* JETSEEDBUILDER_H */

