// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigBPhysCNV_TrigEFBphysReaderAlg_H
#define xAODTrigBPhysCNV_TrigEFBphysReaderAlg_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):

namespace xAODMaker {
    
    /**
     *  @short Algorithm for creating xAOD::TrigxAODBphysReaderAlg from an AOD
     *
     *
     */
    class TrigxAODBphysReaderAlg : public AthAlgorithm {
        
    public:
        /// Regular Algorithm constructor
        TrigxAODBphysReaderAlg( const std::string& name, ISvcLocator* svcLoc );
        
        /// Function initialising the algorithm
        virtual StatusCode initialize();
        /// Function executing the algorithm
        virtual StatusCode execute();
        
    private:
        /// StoreGate key for the output container
        std::string m_xaodKey;
        
    }; // class TrigEFBphysReaderAlg
    
} // namespace xAODMaker

#endif // xAODTrigBPhysCNV_TrigEFBphysReaderAlg_H

