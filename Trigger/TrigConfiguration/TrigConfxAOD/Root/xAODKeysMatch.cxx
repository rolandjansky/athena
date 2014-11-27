/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODKeysMatch.cxx 612011 2014-08-15 13:43:57Z krasznaa $

// Local include(s):
#include "TrigConfxAOD/tools/xAODKeysMatch.h"

namespace TrigConf {

   /// Since this is used in a couple of places, it seemed to make
   /// sense to put this simple code into a central place.
   ///
   /// @param keys The event identifier
   /// @param menu The trigger menu
   /// @returns <code>true</code> if the two objects match each other,
   ///          <code>false</code> otherwise
   ///
   bool xAODKeysMatch( const xAOD::TrigConfKeys* keys,
                       const xAOD::TriggerMenu* menu ) {

      return ( ( keys->smk() == menu->smk() ) &&
               ( keys->l1psk() == menu->l1psk() ) &&
               ( keys->hltpsk() == menu->hltpsk() ) );
   }

} // namespace TrigConf
