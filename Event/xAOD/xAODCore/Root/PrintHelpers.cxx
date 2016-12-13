/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PrintHelpers.cxx 780624 2016-10-26 22:41:13Z ssnyder $

// System include(s):
#include <iostream>
#include <stdint.h>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxTypeRegistry.h"

// Local include(s):
#include "xAODCore/tools/PrintHelpers.h"

/// Helper operator to pretty-print the values of vectors
template< typename T >
std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec ) {

   out << "[";
   for( size_t i = 0; i < vec.size(); ++i ) {
      out << vec[ i ];
      if( i < vec.size() - 1 ) {
         out << ", ";
      }
   }
   out << "]";

   // Return the same stream:
   return out;
}

std::ostream& operator<< ( std::ostream& out, const SG::AuxElement& obj ) {

   // Start off with some basic information:
   out << "SG::AuxElement\n";
   out << "   ";
   if( obj.usingStandaloneStore() ) {
      out << "using standalone store " << obj.getConstStore() << "\n";
   } else if( obj.usingPrivateStore() ) {
      out << "using private store " << obj.getConstStore() << "\n";
   } else if( obj.container() ) {
      out << "in container " << obj.container() << "\n";
   } else {
      // If there's no auxiliary store, bail now:
      out << "standalone object, without auxiliary store";
      return out;
   }

   // If the container doesn't have an auxiliary store, bail now:
   if( ! obj.container()->getConstStore() ) {
      return out;
   }

   out << "   Variables:";

   // Get a convenience pointer to the aux type registry:
   SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

   // Get the auxiliary IDs of the object:
   const SG::auxid_set_t& auxids =
      ( obj.container()->getConstStore() ?
        obj.container()->getConstStore()->getAuxIDs() :
        obj.getConstStore()->getAuxIDs() );

   // Sort auxids to get predictable ordering.
   std::vector<SG::auxid_t> auxids_v (auxids.begin(), auxids.end());
   std::sort (auxids_v.begin(), auxids_v.end());

   for( SG::auxid_t auxid : auxids_v ) {

      out << "\n    - type: " << reg.getTypeName( auxid )
          << ", \tname: \"" << reg.getName( auxid );
      out << "\", \tvalue: ";

/// Helper macro to make the code slightly shorter
#define PRINTER( TYPE )                                           \
      do {                                                        \
         if( obj.isAvailable< TYPE >( reg.getName( auxid ) ) ) {  \
            out << obj.auxdata< TYPE >( reg.getName( auxid ) );   \
         }                                                        \
      } while( 0 )

      // The type of the variable:
      const std::type_info* ti = reg.getType( auxid );
      if( *ti == typeid( int8_t ) ) {
         out << static_cast< int >( obj.auxdata< int8_t >( reg.getName( auxid ) ) );
      } else if( *ti == typeid( uint8_t ) ) {
         out << static_cast< int >( obj.auxdata< uint8_t >( reg.getName( auxid ) ) );
      } else if( *ti == typeid( int16_t ) ) {
         PRINTER( int16_t );
      } else if( *ti == typeid( uint16_t ) ) {
         PRINTER( uint16_t );
      } else if( *ti == typeid( int32_t ) ) {
         PRINTER( int32_t );
      } else if( *ti == typeid( uint32_t ) ) {
         PRINTER( uint32_t );
      } else if( *ti == typeid( int64_t ) ) {
         PRINTER( int64_t );
      } else if( *ti == typeid( uint64_t ) ) {
         PRINTER( uint64_t );
      } else if( *ti == typeid( long ) ) {
         PRINTER( long );
      } else if( *ti == typeid( unsigned long ) ) {
         PRINTER( unsigned long );
      } else if( *ti == typeid( float ) ) {
         PRINTER( float );
      } else if( *ti == typeid( double ) ) {
         PRINTER( double );
      } else if( *ti == typeid( std::vector< float > ) ) {
         PRINTER( std::vector< float > );
      } else if( *ti == typeid( std::vector< double > ) ) {
         PRINTER( std::vector< double > );
      } else if( *ti == typeid( std::vector< int16_t > ) ) {
         PRINTER( std::vector< int16_t > );
      } else if( *ti == typeid( std::vector< uint16_t > ) ) {
         PRINTER( std::vector< uint16_t > );
      } else if( *ti == typeid( std::vector< int32_t > ) ) {
         PRINTER( std::vector< int32_t > );
      } else if( *ti == typeid( std::vector< uint32_t > ) ) {
         PRINTER( std::vector< uint32_t > );
      } else if( *ti == typeid( std::vector< int64_t > ) ) {
         PRINTER( std::vector< int64_t > );
      } else if( *ti == typeid( std::vector< uint64_t > ) ) {
         PRINTER( std::vector< uint64_t > );
      } else {
         out << "N/A";
      }

// Get rid of the temporary macro...
#undef PRINTER
   }

   // Return the same stream:
   return out;
}

namespace xAOD {

   void dump( const SG::AuxElement& obj ) {

      // Do the printout simply using the operator:
      std::cout << obj << std::endl;
      return;
   }

} // namespace xAOD
