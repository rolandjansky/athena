// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormat_v1.h 641134 2015-01-22 19:35:12Z ssnyder $
#ifndef XAODEVENTFORMAT_VERSIONS_EVENTFORMAT_V1_H
#define XAODEVENTFORMAT_VERSIONS_EVENTFORMAT_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

// STL include(s):
#include <vector>
#include <string>
#include <map>
#include <iosfwd>

// Local include(s):
#include "xAODEventFormat/EventFormatElement.h"

namespace xAOD {

   /// Event format metadata for xAOD files
   ///
   /// This class is used to save file-level metadata about the format of
   /// xAOD files. It is not meant to be used directly by analysers, but
   /// just by the code that they use, behind the scenes.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 641134 $
   /// $Date: 2015-01-22 20:35:12 +0100 (Thu, 22 Jan 2015) $
   ///
   class EventFormat_v1 {

      /// Type of the key->data in-memory object
      typedef std::map< std::string, EventFormatElement > KeyedData_t;
      /// Type of the hash->data in-memory object
      typedef std::map< uint32_t, EventFormatElement > HashedData_t;

   public:
      /// Default constructor
      EventFormat_v1();

      /// Add the description of a new branch
      void add( const EventFormatElement& element,
                bool updatePersistent = true );

      /// Check if a description exists about a given branch
      bool exists( const std::string& key ) const;
      /// Check if a description exists about a given branch
      bool exists( uint32_t hash ) const;

      /// Get the description of a given branch
      const EventFormatElement* get( const std::string& key, bool quiet = false ) const;
      /// Get the description of a given branch
      const EventFormatElement* get( uint32_t hash, bool quiet = false ) const;

      /// Clear the object
      void clear();

      /// Iterator for looping over the elements of the object
      typedef KeyedData_t::const_iterator const_iterator;
      /// STL-like function for getting the beginning of the container
      const_iterator begin() const;
      /// STL-like function for getting the end of the container
      const_iterator end() const;

      /// Interpret the persistent members of the object
      void toTransient();

      /// Dump the contents of this object.
      void dump() const;

   private:
      /// Object associating string keys with the descriptions
      KeyedData_t m_keyedData;
      /// Object associating  hash keys with the descriptions
      HashedData_t m_hashedData;

      /// Names of the branches that we are describing
      std::vector< std::string > m_branchNames;
      /// Names of the transient objects belonging to the branch names
      std::vector< std::string > m_classNames;
      /// Names of the auxiliary parents of the branches
      std::vector< std::string > m_parentNames;
      /// Hashed versions of the branch names
      std::vector< uint32_t > m_branchHashes;

   }; // class EventFormat_v1

   /// Print operator for the event format
   std::ostream& operator<<( std::ostream& out,
                             const EventFormat_v1& format );

} // namespace xAOD

#endif // XAODEVENTFORMAT_VERSIONS_EVENTFORMAT_V1_H
