/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphys.h"

#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

// Local include(s):
#include "TrigL2BphysContainerCnvTool.h"


namespace xAODMaker {

    TrigL2BphysContainerCnvTool::TrigL2BphysContainerCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
    : AthAlgTool( type, name, parent ),
    m_cnvTool( "xAODMaker::TrigL2BphysCnvTool/TrigL2BphysCnvTool", this ) {
        
    
        // Declare the interface(s) provided by the tool:
        declareInterface< ITrigL2BphysContainerCnvTool >( this );
    }
    
    StatusCode TrigL2BphysContainerCnvTool::initialize() {
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );

        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
    StatusCode TrigL2BphysContainerCnvTool::convert( const TrigL2BphysContainer* aod,
                                            xAOD::TrigBphysContainer* xaod ) const {
        
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
        TrigL2BphysContainer::const_iterator cit_aod    = aod->begin();
        TrigL2BphysContainer::const_iterator cit_aodend = aod->end();
        for (; cit_aod != cit_aodend; ++cit_aod) {
            const TrigL2Bphys * aodObj = *cit_aod;
            xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
            xaod->push_back( xaodObj );
            CHECK( m_cnvTool->convert( aodObj, xaodObj ) );
        }
        ATH_MSG_VERBOSE( "Output Container has now " << xaod->size() << " elements" );
        
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    
} // xAODMaker

