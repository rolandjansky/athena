/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConfigAccess.cxx 318255 2010-08-27 11:46:34Z krasznaa $

// Local include(s):
#include "ConfigAccess.h"
#include "TrigConfigSvcD3PD.h"

ClassImp( D3PD::Trig::ConfigAccess )

namespace D3PD {

   namespace Trig {

      /**
       * The constructor needs the configuration tree in order to create the trigger
       * configurations service object. But it doesn't need to do anything else.
       *
       * @param confTree Pointer to the trigger configuration metadata tree
       */
      ConfigAccess::ConfigAccess( TTree* confTree )
         : TNamed( "ConfigAccess", "Trigger configuration accessor class" ),
           m_configSvc( confTree ) {

      }

      /**
       * This function is used by the other parts of the TDT to access the trigger
       * configuration service.
       *
       * @returns A reference to the trigger configuration service object
       */
      TrigConfigSvcD3PD& ConfigAccess::GetConfigSvc() {

         return m_configSvc;
      }

   } // namespace Trig

} // namespace D3PD
