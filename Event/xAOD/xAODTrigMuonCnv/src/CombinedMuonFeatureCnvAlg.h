// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_CombinedMuonFeatureCnvAlg_H
#define xAODTrigMuonCNV_CombinedMuonFeatureCnvAlg_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigMuonCnv/ICombinedMuonFeatureContainerCnvTool.h"

namespace xAODMaker {
    
    /**
     *  @short Algorithm for creating xAOD::L2CombinedMuonContainer from an AOD
     *
     *
     */
    class CombinedMuonFeatureCnvAlg : public AthAlgorithm {
        
    public:
        /// Regular Algorithm constructor
        CombinedMuonFeatureCnvAlg( const std::string& name, ISvcLocator* svcLoc );
        
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
        ToolHandle< ICombinedMuonFeatureContainerCnvTool > m_cnvTool;
        
    }; // class CombinedMuonFeatureCnvAlg
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_CombinedMuonFeatureCnvAlg_H
