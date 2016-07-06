/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEventFormatRegistry.cxx 595278 2014-05-03 09:31:05Z krasznaa $

// ROOT include(s):
#include <TFile.h>

// Local include(s):
#include "xAODRootAccess/tools/TEventFormatRegistry.h"

namespace xAOD {

   /// Initialise the static variable(s)
   TEventFormatRegistry* TEventFormatRegistry::s_instance = 0;

   TEventFormatRegistry& TEventFormatRegistry::instance() {

      if( ! s_instance ) {
         s_instance = new TEventFormatRegistry();
      }

      return *s_instance;
   }

   EventFormat& TEventFormatRegistry::getEventFormat( const TFile* file ) {

      return m_eventFormats[ file ];
   }

   void TEventFormatRegistry::merge( const TFile* file,
                                     const EventFormat& ef ) {

      // Get the local EventFormat object:
      EventFormat& localEf = m_eventFormats[ file ];

      // Loop over the contents of the new object:
      EventFormat::const_iterator itr = ef.begin();
      EventFormat::const_iterator end = ef.end();
      for( ; itr != end; ++itr ) {
         // If it's already in the output, continue:
         if( localEf.exists( itr->first ) ) continue;
         // If not, let's add it:
         localEf.add( *( ef.get( itr->first ) ) );
      }

      return;
   }

   TEventFormatRegistry::TEventFormatRegistry()
      : m_eventFormats() {

   }

} // namespace xAOD
