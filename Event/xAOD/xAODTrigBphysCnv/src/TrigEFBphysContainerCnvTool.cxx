/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"

#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

// Local include(s):
#include "TrigEFBphysContainerCnvTool.h"


namespace xAODMaker {

    TrigEFBphysContainerCnvTool::TrigEFBphysContainerCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
    : AthAlgTool( type, name, parent ),
    m_cnvTool( "xAODMaker::TrigEFBphysCnvTool/TrigEFBphysCnvTool", this ) {
        
    
        // Declare the interface(s) provided by the tool:
        declareInterface< ITrigEFBphysContainerCnvTool >( this );
    }
    
    StatusCode TrigEFBphysContainerCnvTool::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
    StatusCode TrigEFBphysContainerCnvTool::convert( const TrigEFBphysContainer* aod,
                                            xAOD::TrigBphysContainer* xaod ) {
        
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
        TrigEFBphysContainer::const_iterator cit_aod    = aod->begin();
        TrigEFBphysContainer::const_iterator cit_aodend = aod->end();
        for (; cit_aod != cit_aodend; ++cit_aod) {
            const TrigEFBphys * aodObj = *cit_aod;
            xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
            xaod->push_back( xaodObj );
            CHECK( m_cnvTool->convert( aodObj, xaodObj ) );
        }
        ATH_MSG_VERBOSE( "Output Container has now " << xaod->size() << " elements" );
        
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
} // xAODMaker

