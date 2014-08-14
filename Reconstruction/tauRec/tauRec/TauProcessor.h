/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPROCESSOR_H
#define TAUREC_TAUPROCESSOR_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "tauRec/TauToolBase.h"

/**
 * @brief       Main class for tau candidate building and processing.
 * 
 *  This class loop over tau candidates in the tau container placed there by the TauBuilder
 *  and runs the given tau tools on the tau candidates. The tau objects are !!modified!!.
 *  If a tool fails, the tau reconstruction will be aborted.
 *  This algorithm has an AOD mode to skip TauExtraDetailsContainer, that is not available in AODs.
 * 
 *  (This algorithm was inspired by TauBuilder.)
 * 
 * @authors  Felix Friedrich
 */

class TauProcessor: public AthAlgorithm
{
    public:
        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauProcessor( const std::string &name, ISvcLocator *pSvcLocator );
        ~TauProcessor();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

    private:
        std :: string                 m_tauContainerName;
	std :: string                 m_tauAuxContainerName; 
        bool                          m_AODmode;
        ToolHandleArray<TauToolBase>  m_tools;
};

#endif // TAUREC_TAUPROCESSOR_H
