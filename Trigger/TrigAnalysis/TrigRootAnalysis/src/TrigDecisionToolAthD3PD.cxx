/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionToolAthD3PD.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "TrigDecisionToolAthD3PD.h"

namespace D3PD {

   TrigDecisionToolAthD3PD::TrigDecisionToolAthD3PD( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        AthDataAccess(), AthConfigAccess() {

      declareInterface< ITrigDecisionToolD3PD >( this );
   }

   StatusCode TrigDecisionToolAthD3PD::initialize() {

      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      CHECK( AthConfigAccess::initialize() );

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
