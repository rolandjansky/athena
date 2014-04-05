/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormatElement.cxx 586453 2014-03-06 10:24:33Z krasznaa $

// STL include(s):
#include <iostream>
#include <iomanip>

// Local include(s):
#include "xAODEventFormat/EventFormatElement.h"
 
namespace xAOD {

   /// @param branchName Name of the branch that is described by the object
   /// @param className  Name of the transient class to be used for reading
   /// @param parentName Name of the parent auxiliary object
   /// @param hash       A hashed version of the branch name
   EventFormatElement::EventFormatElement( const std::string& branchName,
                                           const std::string& className,
                                           const std::string& parentName,
                                           uint32_t hash )
      : m_branchName( branchName ), m_className( className ),
        m_parentName( parentName ), m_hash( hash ) {

   }

   /// @returns The name of the branch that this object describes
   const std::string& EventFormatElement::branchName() const {

      return m_branchName;
   }

   /// @returns The name of the transient class to be read from the branch
   const std::string& EventFormatElement::className() const {

      return m_className;
   }

   /// @returns The name of the parent auxiliary class (or "" if there is none)
   const std::string& EventFormatElement::parentName() const {

      return m_parentName;
   }

   /// @returns A hashed version of the branch's name
   uint32_t EventFormatElement::hash() const {

      return m_hash;
   }

   /// This operator can be used to print information about a given
   /// event format element in a user friendly way.
   ///
   /// @param out An output stream
   /// @param element The EventFormatElement to print
   /// @returns The same output stream that it received
   std::ostream& operator<<( std::ostream& out,
                             const EventFormatElement& element ) {

      // Get the current state of the stream:
      const char fillChar = out.fill();
      const std::ios_base::fmtflags flags = out.flags();
      const std::streamsize width = out.width();

      // Do the printout:
      out << "Branch name: " << std::setw( 30 ) << element.branchName()
          << ", Class name: " << std::setw( 30 ) << element.className()
          << ", Hash: 0x" << std::setw( 8 ) << std::hex << std::setfill( '0' )
          << element.hash();

      // Restore the original state of the stream:
      out.fill( fillChar );
      out.flags( flags );
      out.width( width );

      // Return the stream:
      return out;
   }

} // namespace xAOD
