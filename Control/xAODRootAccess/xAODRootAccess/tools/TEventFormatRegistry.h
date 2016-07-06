// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TEventFormatRegistry.h 595278 2014-05-03 09:31:05Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TEVENTFORMATREGISTRY_H
#define XAODROOTACCESS_TOOLS_TEVENTFORMATREGISTRY_H

// EDM include(s):
#include "xAODEventFormat/EventFormat.h"

// Forward declaration(s):
class TFile;

namespace xAOD {

   /// Helper class for managing the event format for output files
   ///
   /// If the user wants to write a single output file with multiple event-level
   /// trees, we need to keep the output EventFormat objects of the multiple
   /// TEvent instances in sync. This singleton class helps out in this.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 595278 $
   /// $Date: 2014-05-03 11:31:05 +0200 (Sat, 03 May 2014) $
   ///
   class TEventFormatRegistry {

   public:
      /// Access the only instance of the object in memory
      static TEventFormatRegistry& instance();

      /// Access the managed EventFormat object
      EventFormat& getEventFormat( const TFile* file );

      /// Merge the contents of another EventFormat object into the managed one
      void merge( const TFile* file, const EventFormat& ef );

   protected:
      /// Hidden constructor
      TEventFormatRegistry();

      /// The process-wide event format object
      std::map< const TFile*, EventFormat > m_eventFormats;

   private:
      /// Pointer to the only object in memory
      static TEventFormatRegistry* s_instance;

   }; // class TEventFormatRegistry

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TEVENTFORMATREGISTRY_H
