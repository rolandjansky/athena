/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


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
