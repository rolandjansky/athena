/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$ //

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODTrigBphys/TrigBphysContainer.h"

// Local include(s):
#include "TrigxAODBphysReaderAlg.h"


namespace xAODMaker {
    
    TrigxAODBphysReaderAlg::TrigxAODBphysReaderAlg( const std::string& name,
                                         ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
    {
        
        declareProperty( "xAODKey", m_xaodKey = "xAODTrigEFBphys" );
    }
    
    
    StatusCode TrigxAODBphysReaderAlg::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
    StatusCode TrigxAODBphysReaderAlg::execute() {
        
        xAOD::TrigBphysContainer * trigContainer(nullptr);
        // Retrieve the AOD container:
        if (evtStore()->retrieve( trigContainer, m_xaodKey ).isFailure()) {
            ATH_MSG_WARNING("Requesting in Alg: xAOD::TrigBphysContainer " <<  m_xaodKey <<
                            ". Input container not found or incorrect type");
            return StatusCode::SUCCESS;
        }
        
        if (!trigContainer) {
            ATH_MSG_WARNING("Null pointer");
            return StatusCode::SUCCESS;
        }
        
        ATH_MSG_INFO("Found container size: " << trigContainer->size() << " with name " << m_xaodKey);
        for (auto obj: *trigContainer) {
            ATH_MSG_INFO("Print for obj: "<< obj << "\n  " <<
                         "roiId:         "<< obj->roiId()  << "\n  " <<
                         "particleType:  "<< obj->particleType() << "\n  " <<
                         "level:         " << obj->level() << "\n  " <<
                         "eta:           " << obj->eta() << "\n  " <<
                         "phi:           " << obj->phi() << "\n  " <<
                         "mass:          " << obj->mass() << "\n  " <<
                         "fitmass:       " << obj->fitmass() << "\n  " <<
                         "fitchi2:       " << obj->fitchi2() << "\n  " <<
                         "fitndof:       " << obj->fitndof() << "\n  " <<
                         "fitx:          " << obj->fitx() << "\n  " <<
                         "fity:          " << obj->fity() << "\n  " <<
                         "fitz:          " << obj->fitz() << "\n  ");
        } // for
        
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
} // namespace xAODMaker



