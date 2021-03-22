/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventFormat_v1.cxx 641134 2015-01-22 19:35:12Z ssnyder $

// STL include(s):
#include <iostream>
#include <iomanip>

// Local include(s):
#include "xAODEventFormat/versions/EventFormat_v1.h"

namespace xAOD {

   /// Helper output operator for printing vector variables
   template< typename T >
   std::ostream& operator<< ( std::ostream& out,
                              const std::vector< T >& vec ) {

      out << "[";
      typename std::vector< T >::const_iterator itr = vec.begin();
      typename std::vector< T >::const_iterator end = vec.end();
      for( ; itr != end; ++itr ) {
         out << *itr;
         if( ++itr != end ) {
            out << ", ";
         }
         --itr;
      }
      out << "]";
      return out;
   }

   EventFormat_v1::EventFormat_v1()
      : m_keyedData(), m_hashedData() {

   }

   /// This function can be used to extend the object with a new element.
   ///
   /// @param element The element to add to the object
   void EventFormat_v1::add( const EventFormatElement& element,
                             bool updatePersistent ) {

      // Set the transient-only variables:
      m_keyedData[ element.branchName() ] = element;
      m_hashedData[ element.hash() ]      = element;

      // Set the persistent variables:
      if( updatePersistent ) {
         m_branchNames.push_back( element.branchName() );
         m_classNames.push_back( element.className() );
         m_parentNames.push_back( element.parentName() );
         m_branchHashes.push_back( element.hash() );
      }
      return;
   }

   /**
    * @param key The name of the branch to check
    * @returns <code>true</code> if the branch is knows,
    *          <code>false</code> if not
    */
   bool EventFormat_v1::exists( const std::string& key ) const {

      return ( m_keyedData.find( key ) != m_keyedData.end() );
   }

   /**
    * @param hash Hashed version of the branch name to check
    * @returns <code>true</code> if the branch is knows,
    *          <code>false</code> if not
    */
   bool EventFormat_v1::exists( uint32_t hash ) const {

      return ( m_hashedData.find( hash ) != m_hashedData.end() );
   }

   /**
    * This function can be used to get access to one element in the
    * object. Notice that the user code should first check if an element
    * exists, and only use this function if it does.
    *
    * @param key The name of the branch to get the information for
    * @param quiet If true, silently return 0 if the hash isn't found;
    *              otherwise, print an error.
    * @returns A pointer to the element describing the requested branch
    */
   const EventFormatElement*
   EventFormat_v1::get( const std::string& key, bool quiet ) const {

      KeyedData_t::const_iterator itr = m_keyedData.find( key );
      if( itr == m_keyedData.end() ) {
         if (!quiet) {
           std::cerr << "<xAOD::EventFormat_v1::get>"
                     << " Information requested about unknown branch ("
                     << key << ")" << std::endl;
         }
         return nullptr;
      }

      return &( itr->second );
   }

   /**
    * This function can return the element describing a given branch.
    * Notice that the user code should first check if an element
    * exists, and only use this function if it does.
    *
    * @param hash The hashed version of the name of the branch
    * @param quiet If true, silently return 0 if the hash isn't found;
    *              otherwise, print an error.
    * @returns A pointer to the element describing the requested branch
    */
    const EventFormatElement* EventFormat_v1::get( uint32_t hash, bool quiet ) const {

      HashedData_t::const_iterator itr = m_hashedData.find( hash );
      if( itr == m_hashedData.end() ) {
         if (!quiet) {
           // Get the current state of the stream:
           const char fillChar = std::cerr.fill();
           const std::ios_base::fmtflags flags = std::cerr.flags();
           const std::streamsize width = std::cerr.width();

           // Do the printout:
           std::cerr << "<xAOD::EventFormat_v1::get>"
                     << " Information requested about unknown hash ("
                     << std::setw( 8 ) << std::hex << std::setfill( '0' )
                     << hash << ")" << std::endl;

           // Restore the original state of the stream:
           std::cerr.fill( fillChar );
           std::cerr.flags( flags );
           std::cerr.width( width );
         }

         return nullptr;
      }

      return &( itr->second );
   }

   void EventFormat_v1::clear() {

      // Clear the transient variables:
      m_keyedData.clear();
      m_hashedData.clear();

      // Clear the persistent variables:
      m_branchNames.clear();
      m_classNames.clear();
      m_parentNames.clear();
      m_branchHashes.clear();
      return;
   }

   EventFormat_v1::const_iterator EventFormat_v1::begin() const {

      return m_keyedData.begin();
   }

   EventFormat_v1::const_iterator EventFormat_v1::end() const {

      return m_keyedData.end();
   }

   void EventFormat_v1::toTransient() {

      // Clear the transient variables:
      m_keyedData.clear();
      m_hashedData.clear();

      // A little sanity check:
      if( ( m_branchNames.size() != m_classNames.size() ) ||
          ( m_branchNames.size() != m_parentNames.size() ) ||
          ( m_branchNames.size() != m_branchHashes.size() ) ) {
         std::cerr << "<xAOD::EventFormat_v1::toTransient> "
                   << "Internal inconsistency in the persistent data"
                   << std::endl;
         std::cerr << "  m_branchNames  = " << m_branchNames << std::endl;
         std::cerr << "  m_classNames   = " << m_classNames << std::endl;
         std::cerr << "  m_parentNames  = " << m_parentNames << std::endl;
         std::cerr << "  m_branchHashes = " << m_branchHashes << std::endl;
         return;
      }

      // Set up the transient variables:
      for( size_t i = 0; i < m_branchNames.size(); ++i ) {
         add( EventFormatElement( m_branchNames[ i ],
                                  m_classNames[ i ],
                                  m_parentNames[ i ],
                                  m_branchHashes[ i ] ),
              false );
      }

      return;
   }

   /**
    * This operator can be used for debugging purposes to print information
    * about an event format object in a user friendly way.
    *
    * @param out An output stream
    * @param format The TEventFormat_v1 to print
    * @returns The same output stream that it received
    */
   std::ostream& operator<<( std::ostream& out,
                             const EventFormat_v1& format ) {

      out << "xAOD::EventFormat:";
      EventFormat_v1::const_iterator itr = format.begin();
      EventFormat_v1::const_iterator end = format.end();
      for( int counter = 1; itr != end; ++itr, ++counter ) {
         out << std::endl << counter << ". element: " << itr->second;
      }

      return out;
   }


   /// Dump the contents of this object.
   void EventFormat_v1::dump() const
   {
     std::cout << *this;
   }

} // namespace xAOD
