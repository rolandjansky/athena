// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatElement.h 568357 2013-11-01 16:21:30Z krasznaa $
#ifndef XAODEVENTFORMAT_EVENTFORMATELEMENT_H
#define XAODEVENTFORMAT_EVENTFORMATELEMENT_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

// STL include(s):
#include <string>
#include <iosfwd>

namespace xAOD {

   /// Class describing one branch of the ROOT file
   ///
   /// This class describes a single, ROOT-readable branch of an xAOD
   /// file. It is used to figure out the hash <-> branch name associations,
   /// and which auxiliary branches belong to which auxiliary object.
   ///
   /// This class doesn't have a "versioned name", as it is not meant to
   /// be written to a file directly, it should just be used as a
   /// transient means of storing data.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 568357 $
   /// $Date: 2013-11-01 17:21:30 +0100 (Fri, 01 Nov 2013) $
   ///
   class EventFormatElement {

   public:
      /// Constructor with all members specified
      EventFormatElement( const std::string& branchName = "",
                          const std::string& className = "",
                          const std::string& parentName = "",
                          uint32_t hash = 0 );

      /// Get the branch/key name
      const std::string& branchName() const;
      /// Get the class name of this branch/key
      const std::string& className() const;
      /// Get the name of the parent auxiliary object
      const std::string& parentName() const;
      /// Get the hash belonging to this branch/key
      uint32_t hash() const;

   private:
      /// The branch/key name
      std::string m_branchName;
      /// The class name belonging to this branch/key
      std::string m_className;
      /// The name of the parent auxiliary object
      std::string m_parentName;
      /// The hash belonging to this branch/key
      uint32_t m_hash;

   }; // class EventFormatElement_v1

   /// Print operator for an event format element
   std::ostream& operator<<( std::ostream& out,
                             const EventFormatElement& element );

} // namespace xAOD

#endif // XAODEVENTFORMAT_EVENTFORMATELEMENT_H
