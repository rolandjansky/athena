/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MissingETCnvAlg.cxx 586557 2014-03-06 17:56:05Z krasznaa $

// EDM include(s):
#include "MissingETEvent/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

// Only here temporarily, for I/O testing purposes!!!
#include "xAODMissingET/MissingETComponentMap.h"
#include "xAODMissingET/MissingETAuxComponentMap.h"

// Local include(s):
#include "MissingETCnvAlg.h"

namespace xAODMaker {

   MissingETCnvAlg::MissingETCnvAlg( const std::string& name,
                                     ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_cnvTool( "xAODMaker::MissingETCnvTool/MissingETCnvTool", this ) {

      declareProperty( "AODKeys", m_aodKeys );
      declareProperty( "xAODKey", m_xaodKey = "MET_Ref" );
      declareProperty( "CnvTool", m_cnvTool );
   }

   StatusCode MissingETCnvAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "  AOD keys = " << m_aodKeys );
      ATH_MSG_DEBUG( "  xAOD key = " << m_xaodKey );

      // Retieve the needed tool(s):
      ATH_CHECK( m_cnvTool.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode MissingETCnvAlg::execute() {

      // Create the xAOD containers:
      xAOD::MissingETContainer* xaod = new xAOD::MissingETContainer();
      xAOD::MissingETAuxContainer* aux = new xAOD::MissingETAuxContainer();
      xaod->setStore( aux );

      // Loop over the AOD objects:
      std::vector< std::string >::const_iterator key_itr = m_aodKeys.begin();
      std::vector< std::string >::const_iterator key_end = m_aodKeys.end();
      for( ; key_itr != key_end; ++key_itr ) {

         // Try to retrieve the object:
         const MissingET* met = nullptr;
         if( evtStore()->retrieve( met, *key_itr ).isFailure() ) {
            ATH_MSG_WARNING( "Couldn't retrieve MET object with key: "
                             << *key_itr );
            continue;
         }
         ATH_MSG_DEBUG( "Retrieved MET object with key: " << *key_itr );

         // Do the conversion:
         ATH_CHECK( m_cnvTool->add( met, xaod ) );
      }

      // Record the xAOD containers:
      ATH_CHECK( evtStore()->record( xaod, m_xaodKey ) );
      ATH_CHECK( evtStore()->record( aux, m_xaodKey + "Aux." ) );
      ATH_MSG_DEBUG( "Recorded xAOD MET container(s) with key: " << m_xaodKey );

      // Only here temporarily, for I/O testing purposes!!!
      // Record an empty composition map:
      xAOD::MissingETComponentMap* map = new xAOD::MissingETComponentMap();
      xAOD::MissingETAuxComponentMap* mapAux =
         new xAOD::MissingETAuxComponentMap();
      map->setStore( mapAux );

      ATH_CHECK( evtStore()->record( map, m_xaodKey + "Map" ) );
      ATH_CHECK( evtStore()->record( mapAux, m_xaodKey + "MapAux." ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
