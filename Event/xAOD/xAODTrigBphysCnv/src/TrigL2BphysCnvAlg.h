// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBPhysCNV_TrigL2BphysCnvAlg_H
#define xAODTrigBPhysCNV_TrigL2BphysCnvAlg_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigBphysCnv/ITrigL2BphysContainerCnvTool.h"

namespace xAODMaker {
    
    /**
     *  @short Algorithm for creating xAOD::TrigBphysContainer from an AOD
     *
     *
     */
    class TrigL2BphysCnvAlg : public AthAlgorithm {
        
    public:
        /// Regular Algorithm constructor
        TrigL2BphysCnvAlg( const std::string& name, ISvcLocator* svcLoc );
        
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
        ToolHandle< ITrigL2BphysContainerCnvTool > m_cnvTool;
        
    }; // class TrigL2BphysCnvAlg
    
} // namespace xAODMaker

#endif // xAODTrigBPhysCNV_TrigL2BphysCnvAlg_H

