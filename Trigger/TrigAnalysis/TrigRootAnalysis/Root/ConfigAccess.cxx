/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConfigAccess.cxx 502898 2012-05-29 11:15:13Z krasznaa $

// Local include(s):
#include "TrigRootAnalysis/ConfigAccess.h"

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
      IITrigConfigSvcD3PD& ConfigAccess::GetConfigSvc( ::Bool_t update ) {

         // Try to load the appropriate configuration:
         if( update ) {
            if( ! m_configSvc.Load( GetSMK(), GetL1PSK(), GetHLTPSK() ) ) {
               Error( "GetConfigSvc", "Couldn't load configuration with SMK: %i, "
                      "L1PSK: %i, HLTPSK: %i", GetSMK(), GetL1PSK(),
                      GetHLTPSK() );
            }
         }

         // Even in case of an error, we should still return this object:
         return m_configSvc;
      }

      /**
       * Since the Trig::IConfigAccess interface no longer returns a reference
       * to the concrete TrigConfigSvcD3PD object, a new function had to be
       * created here to be able to pass a new configuration tree to an existing
       * TDT object.
       *
       * @param confTree Pointer to a trigger configuration metadata tree
       * @returns <code>kTRUE</code> if the operation was successful,
       *          <code>kFALSE</code> if it wasn't
       */
      ::Bool_t ConfigAccess::SetConfigTree( ::TTree* confTree ) {

         return m_configSvc.SetConfigTree( confTree );
      }

   } // namespace Trig

} // namespace D3PD
