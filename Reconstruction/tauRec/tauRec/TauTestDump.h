/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUTESTDUMP_H
#define TAUREC_TAUTESTDUMP_H

#include "tauRec/TauToolBase.h"


/**
 * @brief Tau Tool for developing, testing and debugging 
 * 
 *  
 * 
 * @author Felix Friedrich
 */

class TauTestDump : public TauToolBase {
public:
    //-----------------------------------------------------------------
    // Constructor and destructor
    //-----------------------------------------------------------------
    TauTestDump(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauTestDump();
    
    virtual StatusCode initialize();                 
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode finalize();  

};

#endif // TAUREC_TAUTESTDUMP_H
