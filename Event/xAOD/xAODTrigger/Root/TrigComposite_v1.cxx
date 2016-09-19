/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigComposite_v1.cxx 761887 2016-07-14 13:16:16Z tbold $

// System include(s):
#include <algorithm>
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"

namespace xAOD {

   TrigComposite_v1::TrigComposite_v1() {

   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigComposite_v1, std::string,
                                      name, setName )


   template<>     
   bool TrigComposite_v1::hasDetail<unsigned int>( const std::string& name ) const {
     return hasDetail<int>(name);
   }

   template<>     
   bool TrigComposite_v1::hasDetail<std::vector<unsigned int>>( const std::string& name ) const {
     return hasDetail<std::vector<int>>(name);
   }



   /////////////////////////////////////////////////////////////////////////////
   //
   //                   Simple detail accessor functions
   //

   bool TrigComposite_v1::setDetail( const std::string& name, int value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< int >( name ) = value;
      } catch(...) {
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::setDetail( const std::string& name, unsigned int value ) {
     return setDetail(name, (int)value); // place for conversion if needed
   }

   bool TrigComposite_v1::setDetail( const std::string& name, float value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< float >( name ) = value;
      } catch(...) {
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< int >& value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< std::vector< int > >( name ) = value;
      } catch(...) {
         return false;
      }

      // Return gracefully:
      return true;
   }


   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< unsigned int >& value ) {

      // It should be pretty strange if this should fail:
     std::vector<int> temp(value.begin(), value.end()); //
      try {
         auxdata< std::vector< int > >( name ) = temp;
      } catch(...) {
         return false;
      }

      // Return gracefully:
      return true;
   }


   bool TrigComposite_v1::setDetail( const std::string& name,
                                     const std::vector< float >& value ) {

      // It should be pretty strange if this should fail:
      try {
         auxdata< std::vector< float > >( name ) = value;
      } catch(...) {
         return false;
      }

      // Return gracefully:
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     int& value ) const {

      // Object used to access the auxiliary data:
      Accessor< int > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }


   bool TrigComposite_v1::getDetail( const std::string& name,
                                     unsigned int& value ) const {
     int v;
     const bool status = getDetail(name, v); 
     value = v; // place for cast
     return status;
     
   }


   bool TrigComposite_v1::getDetail( const std::string& name,
                                     float& value ) const {

      // Object used to access the auxiliary data:
      Accessor< float > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }


   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< int >& value ) const {

      // Object used to access the auxiliary data:
      Accessor< std::vector< int > > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< unsigned int >& value ) const {

     std::vector<int> temp;
     const bool status = getDetail(name, temp);
     
     value.reserve(temp.size());
     for ( int i: temp )
       value.push_back(i);
     return status;
   }



   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< float >& value ) const {

      // Object used to access the auxiliary data:
      Accessor< std::vector< float > > acc( name );

      // Enable the check once it will work correctly:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //               Implementation for the link accessor functions
   //

   bool TrigComposite_v1::hasObjectLink( const std::string& name ) const {

      // Since this function shouldn't throw exceptions too easily,
      // let's be super careful here...
      static Accessor< std::vector< std::string > > accNames( "linkColNames" );
      if( ! accNames.isAvailable( *this ) ) {
         return false;
      }

      // The check itself is pretty simple:
      const std::vector< std::string >& names = accNames( *this );
      return ( std::find( names.begin(), names.end(), name ) != names.end() );
   }

   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< std::string >,
                           linkColNames )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint32_t >,
                           linkColKeys )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint16_t >,
                           linkColIndices )
   AUXSTORE_OBJECT_GETTER( TrigComposite_v1, std::vector< uint32_t >,
                           linkColClids )

   std::vector< std::string >& TrigComposite_v1::linkColNamesNC() {

      static Accessor< std::vector< std::string > > acc( "linkColNames" );
      return acc( *this );
   }

   std::vector< uint32_t >& TrigComposite_v1::linkColKeysNC() {

      static Accessor< std::vector< uint32_t > > acc( "linkColKeys" );
      return acc( *this );
   }

   std::vector< uint16_t >& TrigComposite_v1::linkColIndicesNC() {

      static Accessor< std::vector< uint16_t > > acc( "linkColIndices" );
      return acc( *this );
   }

   std::vector< uint32_t >& TrigComposite_v1::linkColClidsNC() {

      static Accessor< std::vector< uint32_t > > acc( "linkColClids" );
      return acc( *this );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
