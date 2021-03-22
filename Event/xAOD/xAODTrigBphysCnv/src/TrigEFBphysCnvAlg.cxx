/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$ //

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "TrigParticle/TrigEFBphysContainer.h"
#include "TrigParticle/TrigEFBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

// Local include(s):
#include "TrigEFBphysCnvAlg.h"


namespace xAODMaker {

    TrigEFBphysCnvAlg::TrigEFBphysCnvAlg( const std::string& name,
                                         ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
    m_cnvTool( "xAODMaker::TrigEFBphysContainerCnvTool/TrigEFBphysContainerCnvTool", this ) {
        
        declareProperty( "AODKey", m_aodKey = "TrigEFBphys" );
        declareProperty( "xAODKey", m_xaodKey = "xAODTrigEFBphys" );
        declareProperty( "CnvTool", m_cnvTool );
    }

    
    StatusCode TrigEFBphysCnvAlg::initialize() {
        
        // Greet the user:
        ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
        ATH_MSG_DEBUG( " AOD Key: " << m_aodKey );
        ATH_MSG_DEBUG( "xAOD Key: " << m_xaodKey );
        
        // Retrieve the converter tool:
        CHECK( m_cnvTool.retrieve() );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    StatusCode TrigEFBphysCnvAlg::execute() {
        
        // Retrieve the AOD container:
        const TrigEFBphysContainer* aod = nullptr;
        if (evtStore()->retrieve( aod, m_aodKey ).isFailure()) {
            ATH_MSG_WARNING("Requesting in Alg: TrigEFBphysCnvAlg to convert AOD Key: " << m_aodKey <<
                            " into xAOD " << m_xaodKey << ". Input container not found or incorrect type");
            return StatusCode::SUCCESS;
        }
        
        //        // Create the xAOD container and its auxiliary store:
        //        xAOD::TrigBphysAuxContainer* aux = new xAOD::TrigBphysAuxContainer();
        //        xAOD::TrigBphysContainer* xaod = new xAOD::TrigBphysContainer();
        //        xaod->setStore( aux );
        //
        //        // Fill the xAOD container:
        //        TrigEFBphysContainer::const_iterator cit_aod    = aod->begin();
        //        TrigEFBphysContainer::const_iterator cit_aodend = aod->end();
        //        for (; cit_aod != cit_aodend; ++cit_aod) {
        //            const TrigEFBphys * aodObj = *cit_aod;
        //            xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
        //            xaod->push_back( xaodObj );
        //            CHECK( m_cnvTool->convert( aodObj, xaodObj ) );
        //        }

        xAOD::TrigBphysContainer* xaod = new xAOD::TrigBphysContainer();
        xAOD::TrigBphysAuxContainer* aux = new xAOD::TrigBphysAuxContainer();
        xaod->setStore( aux );

        CHECK( m_cnvTool->convert(aod, xaod) );
        
        // Record the xAOD containers:
        CHECK( evtStore()->record( xaod, m_xaodKey ) );
        CHECK( evtStore()->record( aux, m_xaodKey + "Aux" ) );
        
        // Return gracefully:
        return StatusCode::SUCCESS;
    }
    
} // namespace xAODMaker

    
    
