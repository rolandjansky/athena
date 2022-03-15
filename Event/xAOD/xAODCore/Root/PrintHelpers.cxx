/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <cstdint>
#include <iostream>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/AtomicConstAccessor.h"

// Local include(s):
#include "xAODCore/tools/PrintHelpers.h"

namespace PrintHelpers {

template <class T> struct VecPrintType {
  typedef T type;
};

template <> struct VecPrintType<char> {
  using type = int;
};

template <> struct VecPrintType<int8_t> {
  using type = int;
};

template <> struct VecPrintType<uint8_t> {
  using type = unsigned int;
};

} // namespace PrintHelpers


template< typename T, typename U >
std::ostream& operator<< ( std::ostream& out, const std::pair< T, U >& p ) {
  out << "(";
  out << static_cast<typename PrintHelpers::VecPrintType<T>::type> (p.first);
  out << ", ";
  out << static_cast<typename PrintHelpers::VecPrintType<U>::type> (p.second);
  out << ")";
  return out;
}

/// Helper operator to pretty-print the values of vectors
template< typename T >
std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec ) {

   out << "[";
   for( size_t i = 0; i < vec.size(); ++i ) {
      out << static_cast<typename PrintHelpers::VecPrintType<T>::type> (vec[ i ]);
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
      out << "using standalone store\n";
   } else if( obj.usingPrivateStore() ) {
      out << "using private store\n";
   } else if( obj.container() ) {
      out << "in container\n";
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
      if( reg.getFlags( auxid ) & SG::AuxTypeRegistry::Atomic ) {
        if( *ti == typeid( unsigned int ) ) {
          SG::AtomicConstAccessor<unsigned int> acc( reg.getName( auxid ) );
          if ( acc.isAvailable( obj )) {
            out << acc (obj);
          }
        }
        else {
          out << "(Unsupported atomic type)\n";
        }
      } else if( *ti == typeid( int8_t ) ) {
         if( obj.isAvailable< int8_t >( reg.getName( auxid ) ) ) {
           out << static_cast< int >( obj.auxdata< int8_t >( reg.getName( auxid ) ) );
         }
      } else if( *ti == typeid( uint8_t ) ) {
         if( obj.isAvailable< uint8_t >( reg.getName( auxid ) ) ) {
           out << static_cast< int >( obj.auxdata< uint8_t >( reg.getName( auxid ) ) );
         }
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
      } else if( *ti == typeid( unsigned long long ) ) {
         PRINTER( unsigned long long );
      } else if( *ti == typeid( float ) ) {
         PRINTER( float );
      } else if( *ti == typeid( double ) ) {
         PRINTER( double );
      } else if( *ti == typeid( std::vector< float > ) ) {
         PRINTER( std::vector< float > );
      } else if( *ti == typeid( std::vector< double > ) ) {
         PRINTER( std::vector< double > );
      } else if( *ti == typeid( std::vector< char > ) ) {
         PRINTER( std::vector< char > );
      } else if( *ti == typeid( std::vector< int8_t > ) ) {
         PRINTER( std::vector< int8_t > );
      } else if( *ti == typeid( std::vector< uint8_t > ) ) {
         PRINTER( std::vector< uint8_t > );
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
      } else if( *ti == typeid( std::vector< std::string > ) ) {
         PRINTER( std::vector< std::string > );
      } else if( *ti == typeid( std::vector< std::pair<std::string, std::string> > ) ) {
         using Stringpair_t = std::pair<std::string, std::string>;
         PRINTER( std::vector< Stringpair_t > );
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
