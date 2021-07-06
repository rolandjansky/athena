/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuon.h"

#include "TrigMuonEvent/CombinedMuonFeature.h"

// Local include(s):
#include "CombinedMuonFeatureContainerCnvTool.h"


namespace xAODMaker {

    CombinedMuonFeatureContainerCnvTool::CombinedMuonFeatureContainerCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
    : AthAlgTool( type, name, parent ),
    m_cnvTool( "xAODMaker::CombinedMuonFeatureCnvTool/CombinedMuonFeatureCnvTool", this ) {
        
    
        // Declare the interface(s) provided by the tool:
        declareInterface< ICombinedMuonFeatureContainerCnvTool >( this );
    }
    
    StatusCode CombinedMuonFeatureContainerCnvTool::initialize() {
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
    StatusCode CombinedMuonFeatureContainerCnvTool::convert( const CombinedMuonFeatureContainer* aod,
                                            xAOD::L2CombinedMuonContainer* xaod ) const {
        
        if (!aod) {
            ATH_MSG_ERROR( "Null pointer for input containter" );
            return StatusCode::FAILURE;
        }
        if (!xaod) {
            ATH_MSG_ERROR( "Null pointer for output containter" );
            return StatusCode::FAILURE;
        }

        ATH_MSG_VERBOSE( "Found " << aod->size() << " input elements" );
        
        // Fill the xAOD container:
        CombinedMuonFeatureContainer::const_iterator cit_aod    = aod->begin();
        CombinedMuonFeatureContainer::const_iterator cit_aodend = aod->end();
        for (; cit_aod != cit_aodend; ++cit_aod) {
            const CombinedMuonFeature * aodObj = *cit_aod;
            xAOD::L2CombinedMuon* xaodObj = new xAOD::L2CombinedMuon();
            xaod->push_back( xaodObj );
            CHECK( m_cnvTool->convert( aodObj, xaodObj ) );
        }
        ATH_MSG_VERBOSE( "Output Container has now " << xaod->size() << " elements" );
        
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
} // xAODMaker
