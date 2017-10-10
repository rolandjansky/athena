/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileMetaData_v1.cxx 683694 2015-07-17 09:03:52Z krasznaa $

// System include(s):
#include <iostream>
//#include <array>
#include <cmath>
#include <cstdlib>

// Core include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"

// Local include(s):
#include "xAODMetaData/versions/FileMetaData_v1.h"
#include "FileMetaDataAccessors_v1.h"

namespace xAOD {

   FileMetaData_v1::FileMetaData_v1()
      : SG::AuxElement() {

   }

   bool FileMetaData_v1::operator==( const FileMetaData_v1& rhs ) {

      // Get the variable types from both objects:
      const SG::auxid_set_t& auxids1 = this->getAuxIDs();
      const SG::auxid_set_t& auxids2 = rhs.getAuxIDs();

      // They need to be the same. If the two objects have different variables,
      // that's bad. Unfortunately there's no equivalency operator for
      // auxid_set_t, so this check needs to be spelled out. :-(
      if( auxids1.size() != auxids2.size() ) {
         return false;
      }
      for( SG::auxid_t auxid : auxids1 ) {
         if( auxids2.find( auxid ) == auxids2.end() ) {
            return false;
         }
      }

      // Now, compare all elements:
      for( SG::auxid_t auxid : auxids1 ) {

         // Check the type of the variable:
         SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
         const std::type_info* ti = reg.getType( auxid );
         if( ! ti ) {
            // This is weird, but there's not much that we can do about it
            // here...
            continue;
         }

         // Only compare the string and float properties for now:
         if( ( *ti != typeid( std::string ) ) &&
             ( *ti != typeid( float ) ) ) {
            continue;
         }

         // The variable name:
         const std::string name = reg.getName( auxid );

         // Treat the string and float options separately:
         if( *ti == typeid( std::string ) ) {

            // Retrieve the values:
            const std::string& value1 = this->auxdata< std::string >( name );
            const std::string& value2 = rhs.auxdata< std::string >( name );
            // And simply compare them:
            if( value1 != value2 ) {
               return false;
            }

         } else if( *ti == typeid( float ) ) {

            // Retrieve the values:
            const float& value1 = this->auxdata< float >( name );
            const float& value2 = rhs.auxdata< float >( name );
            // And (not so simply) compare them:
            if( std::abs( value1 - value2 ) > 0.001 ) {
               return false;
            }

         } else {
            // We should really never end up here unless a coding mistake was
            // made upstream.
            std::abort();
         }
      }

      // The two objects were found to be equivalent:
      return true;
   }

   bool FileMetaData_v1::operator!=( const FileMetaData_v1& rhs ) {

      return !( this->operator==( rhs ) );
   }

   bool FileMetaData_v1::value( MetaDataType type, std::string& val ) const {

      // Get the accessor for this type:
      Accessor< std::string >* acc = metaDataTypeStringAccessorV1( type );
      if( ! acc ) {
         return false;
      }

      // Check if the variable is available:
      if( ! acc->isAvailable( *this ) ) {
         return false;
      }

      // Read the value:
      val = ( *acc )( *this );

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::value( const std::string& type,
                                std::string& val ) const {

      // Create an accessor object:
      Accessor< std::string > acc( type );

      // Check if this variable is available:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Read the value:
      val = acc( *this );

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::setValue( MetaDataType type, const std::string& val ) {

      // Get the accessor for this type:
      Accessor< std::string >* acc = metaDataTypeStringAccessorV1( type );
      if( ! acc ) {
         return false;
      }

      // Set the value:
      ( *acc )( *this ) = val;

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::setValue( const std::string& type,
                                   const std::string& val ) {

      // Create the accessor object:
      Accessor< std::string > acc( type );

      // Set the value:
      acc( *this ) = val;

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::value( MetaDataType type, float& val ) const {

      // Get the accessor for this type:
      Accessor< float >* acc = metaDataTypeFloatAccessorV1( type );
      if( ! acc ) {
         return false;
      }

      // Check if the variable is available:
      if( ! acc->isAvailable( *this ) ) {
         return false;
      }

      // Read the value:
      val = ( *acc )( *this );

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::value( const std::string& type,
                                float& val ) const {

      // Create an accessor object:
      Accessor< float > acc( type );

      // Check if this variable is available:
      if( ! acc.isAvailable( *this ) ) {
         return false;
      }

      // Read the value:
      val = acc( *this );

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::setValue( MetaDataType type, float val ) {

      // Get the accessor for this type:
      Accessor< float >* acc = metaDataTypeFloatAccessorV1( type );
      if( ! acc ) {
         return false;
      }

      // Set the value:
      ( *acc )( *this ) = val;

      // We were successful:
      return true;
   }

   bool FileMetaData_v1::setValue( const std::string& type, float val ) {

      // Create the accessor object:
      Accessor< float > acc( type );

      // Set the value:
      acc( *this ) = val;

      // We were successful:
      return true;
   }

} // namespace xAOD

/// Helper macro used to print MetaDataType values
#define PRINT_TYPE( TYPE )                      \
   case xAOD::FileMetaData_v1::TYPE:            \
   out << "xAOD::FileMetaData::" << #TYPE;      \
   break

/// This can be used in user code to conveniently print the values of
/// MetaDataType variables in a user friendly manner.
///
/// @param out The output stream to print to
/// @param type The value to print
/// @returns The same stream object that the operator printed to
///
std::ostream& operator<< ( std::ostream& out,
                           xAOD::FileMetaData_v1::MetaDataType type ) {

   switch( type ) {

      PRINT_TYPE( productionRelease );
      PRINT_TYPE( AODFixVersion );
      PRINT_TYPE( AODCalibVersion );
      PRINT_TYPE( dataType );
      PRINT_TYPE( geometryVersion );
      PRINT_TYPE( conditionsTag );
      PRINT_TYPE( beamEnergy );
      PRINT_TYPE( beamType );

   default:
      out << "UNKNOWN (" << static_cast< int >( type ) << ")";
      break;
   }

   // Return the same stream:
   return out;
}

