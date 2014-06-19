// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBPhysCNV_TrigEFBphysCnvAlg_H
#define xAODTrigBPhysCNV_TrigEFBphysCnvAlg_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigBphysCnv/ITrigEFBphysContainerCnvTool.h"

namespace xAODMaker {
    
    /**
     *  @short Algorithm for creating xAOD::TrigEFBphysContainer from an AOD
     *
     *
     */
    class TrigEFBphysCnvAlg : public AthAlgorithm {
        
    public:
        /// Regular Algorithm constructor
        TrigEFBphysCnvAlg( const std::string& name, ISvcLocator* svcLoc );
        
        /// Function initialising the algorithm
        virtual StatusCode initialize();
        /// Function executing the algorithm
        virtual StatusCode execute();
        
    private:
        /// StoreGate key of the input container
        std::string m_aodKey;
        /// StoreGate key for the output container
        std::string m_xaodKey;
        
        /// Handle to the converter tool
        ToolHandle< ITrigEFBphysContainerCnvTool > m_cnvTool;
        
    }; // class TrigBphysCnvAlg
    
} // namespace xAODMaker

#endif // xAODTrigBPhysCNV_TrigEFBphysCnvAlg_H

