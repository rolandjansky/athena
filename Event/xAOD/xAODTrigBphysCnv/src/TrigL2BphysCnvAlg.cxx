/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$ //

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

// Local include(s):
#include "TrigL2BphysCnvAlg.h"


namespace xAODMaker {

    TrigL2BphysCnvAlg::TrigL2BphysCnvAlg( const std::string& name,
                                       ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
    m_cnvTool( "xAODMaker::TrigL2BphysContainerCnvTool/TrigL2BphysContainerCnvTool", this ) {
        
        declareProperty( "AODKey", m_aodKey = "TrigL2Bphys" );
        declareProperty( "xAODKey", m_xaodKey = "L2TrigBphys" );
        declareProperty( "CnvTool", m_cnvTool );
    }

    
    StatusCode TrigL2BphysCnvAlg::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        ATH_MSG_INFO( " AOD Key: " << m_aodKey );
        ATH_MSG_INFO( "xAOD Key: " << m_xaodKey );
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode TrigL2BphysCnvAlg::execute() {
        
        // Retrieve the AOD container:
        const TrigL2BphysContainer* aod = nullptr;
        if (evtStore()->retrieve( aod, m_aodKey ).isFailure()) {
            ATH_MSG_WARNING("Requesting in Alg: TrigL2BphysCnvAlg to convert AOD Key: " << m_aodKey <<
                            " into xAOD " << m_xaodKey << ". Input container not found or incorrect type");
            return StatusCode::SUCCESS;
        }
        
        //        // Create the xAOD container and its auxiliary store:
        //        xAOD::TrigBphysAuxContainer* aux = new xAOD::TrigBphysAuxContainer();
        //        xAOD::TrigBphysContainer* xaod = new xAOD::TrigBphysContainer();
        //        xaod->setStore( aux );
        //
        //        // Fill the xAOD container:
        //        TrigL2BphysContainer::const_iterator cit_aod    = aod->begin();
        //        TrigL2BphysContainer::const_iterator cit_aodend = aod->end();
        //        for (; cit_aod != cit_aodend; ++cit_aod) {
        //            const TrigL2Bphys * aodObj = *cit_aod;
        //            // Create the xAOD object:
        //            xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
        //            xaod->push_back( xaodObj );
        //
        //            CHECK( m_cnvTool->convert( aodObj, xaodObj ) );
        //        }
        
        xAOD::TrigBphysContainer* xaod = new xAOD::TrigBphysContainer();
        xAOD::TrigBphysAuxContainer* aux = new xAOD::TrigBphysAuxContainer();
        xaod->setStore( aux );
        
        CHECK( m_cnvTool->convert(aod, xaod) );

        
        // Record the xAOD containers:
        CHECK( evtStore()->record( aux, m_xaodKey + "Aux" ) );
        CHECK( evtStore()->record( xaod, m_xaodKey ) );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
} // namespace xAODMaker

    
