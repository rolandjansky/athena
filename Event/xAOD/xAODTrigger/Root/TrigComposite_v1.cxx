/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TrigComposite_v1.h"

namespace xAOD {

   TrigComposite_v1::TrigComposite_v1() {

   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigComposite_v1, std::string,
                                      name, setName )

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
      /*
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }
      */

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     float& value ) const {

      // Object used to access the auxiliary data:
      Accessor< float > acc( name );

      // Enable the check once it will work correctly:
      /*
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }
      */

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< int >& value ) const {

      // Object used to access the auxiliary data:
      Accessor< std::vector< int > > acc( name );

      // Enable the check once it will work correctly:
      /*
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }
      */

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   bool TrigComposite_v1::getDetail( const std::string& name,
                                     std::vector< float >& value ) const {

      // Object used to access the auxiliary data:
      Accessor< std::vector< float > > acc( name );

      // Enable the check once it will work correctly:
      /*
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }
      */

      // Retrieve this detail:
      value = acc( *this );
      return true;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
