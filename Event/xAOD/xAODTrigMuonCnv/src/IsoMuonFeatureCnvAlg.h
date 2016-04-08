// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMuonCNV_IsoMuonFeatureCnvAlg_H
#define xAODTrigMuonCNV_IsoMuonFeatureCnvAlg_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local include(s):
#include "xAODTrigMuonCnv/IIsoMuonFeatureContainerCnvTool.h"

namespace xAODMaker {
    
    /**
     *  @short Algorithm for creating xAOD::L2IsoMuonContainer from an AOD
     *
     *
     */
    class IsoMuonFeatureCnvAlg : public AthAlgorithm {
        
    public:
        /// Regular Algorithm constructor
        IsoMuonFeatureCnvAlg( const std::string& name, ISvcLocator* svcLoc );
        
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
        ToolHandle< IIsoMuonFeatureContainerCnvTool > m_cnvTool;
        
    }; // class IsoMuonFeatureCnvAlg
    
} // namespace xAODMaker

#endif // xAODTrigMuonCNV_IsoMuonFeatureCnvAlg_H
