/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerRingsCnvTool.cxx 592334 2014-04-10 11:01:56Z krasznaa $

// EDM include(s):
#include "TrigCaloEvent/RingerRings.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

// Local include(s):
#include "TrigRingerRingsCnvTool.h"

namespace xAODMaker {

   TrigRingerRingsCnvTool::TrigRingerRingsCnvTool( const std::string& type,
						   const std::string& name,
						   const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITrigRingerRingsCnvTool >( this );
   }

   StatusCode TrigRingerRingsCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes the RingerRings object
    * from a RingerRings container, and fills an xAOD::RingerRings with it.
    *
    * @param aod The AOD object to take the RingerRings from
    * @param xaod The xAOD object to fill with information
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode
   TrigRingerRingsCnvTool::convert( const RingerRingsContainer* aod,
                                    xAOD::TrigRingerRingsContainer* xaod ) {

      ATH_MSG_DEBUG( "Size of the xAOD container before loop: "
                     << xaod->size() );

      // Loop over the AOD objects:
      RingerRingsContainer::const_iterator aod_itr = aod->begin();
      RingerRingsContainer::const_iterator aod_end = aod->end();

      for( ; aod_itr != aod_end; ++aod_itr ) {

         // Create the xAOD object:
         xAOD::TrigRingerRings* rings = new xAOD::TrigRingerRings();
         xaod->push_back( rings );

         rings->setRings(( *aod_itr )->rings() );
      }

      ATH_MSG_DEBUG( "Size of the xAOD container after loop: "
                     << xaod->size() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
