/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCOMMONCALCVARS_H
#define TAUREC_TAUCOMMONCALCVARS_H

#include "tauRecTools/TauRecToolBase.h"


/**
 * @brief Calculate variables which rely on tracks and precalculated cell/cluster information.
 * 
 *  All variables here can be recalculated using AODs.
 * 
 * @author Stan Lai
 * @author Felix Friedrich
 */

class TauCommonCalcVars : public TauRecToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauCommonCalcVars(const std::string& name="TauCommonCalcVars");
    ASG_TOOL_CLASS2(TauCommonCalcVars, TauRecToolBase, ITauToolBase)
    ~TauCommonCalcVars();
    
    virtual StatusCode initialize();                 
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode finalize();  

    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:
    std::string m_configPath;

    int m_isolationTrackType;


};

#endif // TAUREC_TAUCOMMONCALCVARS_H
