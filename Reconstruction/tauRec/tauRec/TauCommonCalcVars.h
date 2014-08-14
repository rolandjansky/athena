/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCOMMONCALCVARS_H
#define TAUREC_TAUCOMMONCALCVARS_H

#include "tauRec/TauToolBase.h"


/**
 * @brief Calculate variables which rely on tracks and precalculated cell/cluster information.
 * 
 *  All variables here can be recalculated using AODs.
 * 
 * @author Stan Lai
 * @author Felix Friedrich
 */

class TauCommonCalcVars : public TauToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauCommonCalcVars(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauCommonCalcVars();
    
    virtual StatusCode initialize();                 
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode finalize();  

};

#endif // TAUREC_TAUCOMMONCALCVARS_H
