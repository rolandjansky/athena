/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPROCESSORALG_H
#define TAUREC_TAUPROCESSORALG_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "tauRecTools/ITauToolExecBase.h"

/**
 * @brief       Main class for tau candidate processing.
 */

class TauProcessorAlg: public AthAlgorithm
{
    public:
        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauProcessorAlg( const std::string &name, ISvcLocator *pSvcLocator );
        ~TauProcessorAlg();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

    private:
        ToolHandleArray<ITauToolExecBase>  m_tools;
};

#endif // TAUREC_TAUPROCESSORALG_H
