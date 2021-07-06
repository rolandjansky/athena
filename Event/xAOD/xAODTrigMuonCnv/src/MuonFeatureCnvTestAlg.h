/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
//  MuonFeatureCnvTestAlg.h
//
//  Created by Masaki Ishitsuka on 20/02/2015.
//  Referring TrigMuonEFInfoCnvTestAlg created by Mark Owen
//

#ifndef xAODTrigMuonCnv_MuonFeatureCnvTestAlg_h
#define xAODTrigMuonCnv_MuonFeatureCnvTestAlg_h

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// xAOD includes
#include "xAODTrigMuon/L2StandAloneMuon.h"

// fwd declare some clases
class MuonFeature;
class MuonFeatureDetails;

class MuonFeatureCnvTestAlg : public AthAlgorithm {
        
    public:
        /// Regular algorithm constructor
        MuonFeatureCnvTestAlg( const std::string& name, ISvcLocator* svcLoc );
        
        /// Function initialising the algorithm
        virtual StatusCode initialize();
        /// Function executing the algorithm
        virtual StatusCode execute();

    private:
        /// The key of the MuonFeatureContainer and MuonFeatureDetailsContainer
        std::string m_aodMuonFeatureContainerName;
        std::string m_aodMuonFeatureDetailsContainerName;
        /// The key for the xAOD::L2StandAloneMuonContainer
        std::string m_xaodContainerName;
};

#endif
