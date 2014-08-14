/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_LOCKTAUCONTAINERS_H
#define	TAUREC_LOCKTAUCONTAINERS_H

#include "tauRec/TauToolBase.h"

/**
 * @brief  Set tau containers to const to prevent downstream modification.
 * 
 * @author Felix Friedrich
 */

class LockTauContainers : public TauToolBase
{
    public: 
        LockTauContainers(const std::string& type,
                const std::string& name,
                const IInterface* parent);

        ~LockTauContainers() { }

        virtual StatusCode initialize()                      { return StatusCode::SUCCESS; }
        virtual StatusCode execute( TauCandidateData* )      { return StatusCode::SUCCESS; }
        virtual StatusCode eventFinalize( TauCandidateData *data );
};

#endif	/* TAUREC_LOCKTAUCONTAINERS_H */

