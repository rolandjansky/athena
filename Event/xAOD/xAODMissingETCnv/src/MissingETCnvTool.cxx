/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MissingETCnvTool.cxx 586841 2014-03-08 15:14:37Z khoo $

// EDM include(s):
#include "MissingETEvent/MissingET.h"

// Local include(s):
#include "MissingETCnvTool.h"

namespace xAODMaker {

   MissingETCnvTool::MissingETCnvTool( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
      : AthAlgTool( type, name, parent ) {

      declareInterface< IMissingETCnvTool >( this );
   }

   StatusCode MissingETCnvTool::initialize() {

      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode MissingETCnvTool::add( const MissingET* aod,
                                     xAOD::MissingETContainer* xaod ) {

      // Add a new MET object to the container:
      xAOD::MissingET* met = new xAOD::MissingET();
      xaod->push_back( met );

      ATH_MSG_VERBOSE( "Filling xAOD::MissingET object:" );

      // Copy some of the properties:
      met->setMpx( aod->etx() );
      ATH_MSG_VERBOSE( " - px    = " << met->mpx() );
      met->setMpy( aod->ety() );
      ATH_MSG_VERBOSE( " - py    = " << met->mpy() );
      met->setSumet( aod->sumet() );
      ATH_MSG_VERBOSE( " - sumet = " << met->sumet() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
