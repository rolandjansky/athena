/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuon.h"

#include "TrigMuonEvent/IsoMuonFeature.h"

// Local include(s):
#include "IsoMuonFeatureContainerCnvTool.h"


namespace xAODMaker {

    IsoMuonFeatureContainerCnvTool::IsoMuonFeatureContainerCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
    : AthAlgTool( type, name, parent ), m_cnvTool( "xAODMaker::IsoMuonFeatureCnvTool/IsoMuonFeatureCnvTool", this ) {
        // Declare the interface(s) provided by the tool:
        declareInterface< IIsoMuonFeatureContainerCnvTool >( this );
    }
    
    StatusCode IsoMuonFeatureContainerCnvTool::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
    StatusCode IsoMuonFeatureContainerCnvTool::convert( const IsoMuonFeatureContainer* aod,
                                            xAOD::L2IsoMuonContainer* xaod ) {
        
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
        IsoMuonFeatureContainer::const_iterator cit_aod    = aod->begin();
        IsoMuonFeatureContainer::const_iterator cit_aodend = aod->end();
        for (; cit_aod != cit_aodend; ++cit_aod) {
            const IsoMuonFeature * aodObj = *cit_aod;
            xAOD::L2IsoMuon* xaodObj = new xAOD::L2IsoMuon();
            xaod->push_back( xaodObj );
            CHECK( m_cnvTool->convert( aodObj, xaodObj ) );
        }
        ATH_MSG_VERBOSE( "Output Container has now " << xaod->size() << " elements" );
        
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
} // xAODMaker
