/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUTESTDUMP_H
#define TAUREC_TAUTESTDUMP_H

#include "tauRecTools/TauRecToolBase.h"


/**
 * @brief Tau Tool for developing, testing and debugging 
 * 
 *  
 * 
 * @author Felix Friedrich
 */

class TauTestDump : public TauRecToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauTestDump(const std::string& name);
    ASG_TOOL_CLASS2(TauTestDump, TauRecToolBase, ITauToolBase);
    ~TauTestDump();
    
    virtual StatusCode initialize();                 
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode finalize();  
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

};

#endif // TAUREC_TAUTESTDUMP_H
