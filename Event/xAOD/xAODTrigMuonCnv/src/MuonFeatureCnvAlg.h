// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_MuonFeatureCnvAlg_H
#define xAODTrigMuonCNV_MuonFeatureCnvAlg_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigMuonCnv/IMuonFeatureCnvTool.h"

namespace xAODMaker {
    
    /**
     *  @short Algorithm for creating xAOD::L2StandAloneMuonContainer from an AOD
     *
     *
     */
    class MuonFeatureCnvAlg : public AthAlgorithm {
        
    public:
        /// Regular Algorithm constructor
        MuonFeatureCnvAlg( const std::string& name, ISvcLocator* svcLoc );
        
        /// Function initialising the algorithm
        virtual StatusCode initialize();
        /// Function executing the algorithm
        virtual StatusCode execute();
        
    private:
        /// StoreGate key of the input container
        std::string m_mfName;
        std::string m_mfdName;
        /// StoreGate key for the output container
        std::string m_xaodName;
        
        /// Handle to the converter tool
        ToolHandle< IMuonFeatureCnvTool > m_converterTool;
        
    }; // class MuonFeatureCnvAlg
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_MuonFeatureCnvAlg_H
