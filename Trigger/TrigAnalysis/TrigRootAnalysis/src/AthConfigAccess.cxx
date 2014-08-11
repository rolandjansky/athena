/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AthConfigAccess.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "AthConfigAccess.h"

namespace D3PD {

   AthConfigAccess::AthConfigAccess()
      : m_service( "D3PD::TrigConfigSvcAthD3PD", "D3PD::AthConfigAccess" ) {

   }

   StatusCode AthConfigAccess::initialize() {

      // Retrieve the configuration service:
      CHECK( m_service.retrieve() );

      return StatusCode::SUCCESS;
   }

   /**
    * Since the Athena version of IITrigDecisionToolD3PD is always configured
    * according to the current event, the "update" property is just ignored here.
    *
    * @param update Ignored here
    * @returns A reference to an instance of D3PD::TrigConfigSvcAthD3PD
    */
   D3PD::IITrigConfigSvcD3PD&
   AthConfigAccess::GetConfigSvc( ::Bool_t /*update*/ ) {

      return ( *m_service );
   }

} // namespace D3PD
