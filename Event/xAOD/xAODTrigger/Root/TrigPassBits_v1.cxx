/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <functional>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/TrigPassBits_v1.h"

namespace xAOD {

   TrigPassBits_v1::TrigPassBits_v1()
      : SG::AuxElement(), m_container( nullptr ) {

   }

   /// Non-template function for marking an element with the specified
   /// index as passing/non-passing. Performs fewer checks than the
   /// template function, so should be used with care.
   ///
   /// @param index  The index of the object to set the state of
   /// @param passed The "passing state" of the object with the specified index
   ///
   void TrigPassBits_v1::markPassing( size_t index, bool passed ) {

      // A sanity check:
      if( index >= size() ) {
         throw std::runtime_error( "Index out of range" );
      }

      // Access the serialised bits:
      static const SG::AuxElement::Accessor< std::vector< uint32_t > >
         accBits( "passBits" );
      std::vector< uint32_t >& bits = accBits( *this );

      // Calculate which bit to modify:
      const size_t vec_element = index / 32;
      const size_t vec_index   = index % 32;

      // Another sanity check:
      if( vec_element >= bits.size() ) {
         throw std::runtime_error( "The object is in an inconsistent state" );
      }

      // Set the bit:
      const uint32_t bit = ( 0x1 << vec_index );
      uint32_t mask = bits[ vec_element ];
      if( passed ) {
         mask |= bit;
      } else {
         mask &= ~bit;
      }
      bits[ vec_element ] = mask;

      return;
   }

   /// Non-template function for checking the state of an object with a given
   /// index inside the container.
   ///
   /// @param index The index of the object to check the state of
   /// @returns The "passing state" of the object with the specified index
   ///
   bool TrigPassBits_v1::isPassing( size_t index ) const {

      // A sanity check:
      if( index >= size() ) {
         throw std::runtime_error( "Index out of range" );
      }

      // Access the serialised bits:
      const std::vector< uint32_t >& bits = passBits();

      // Calculate the bit to extract:
      const size_t vec_element = index / 32;
      const size_t vec_index   = index % 32;

      // Another sanity check:
      if( vec_element >= bits.size() ) {
         throw std::runtime_error( "The object is in an inconsistent state" );
      }

      // Extract the bit:
      const uint32_t bit = ( 0x1 << vec_index );
      const uint32_t mask = bits[ vec_element ];
      return ( mask & bit );
   }

   /// Helper function used to create a well defined hash of a string key.
   /// Used to create the containerKey value stored in the object.
   ///
   /// @param key The string key to create a hash out of
   /// @returns The hashed value for the specified key
   ///
   uint32_t TrigPassBits_v1::hash( const std::string& key ) {

      // The helper object:
      static const std::hash< std::string > helper;

      // The mask comes from how StoreGate creates hashes from keys. Could use
      // some other value here as well though.
      return ( static_cast< uint32_t >( helper( key ) ) & 0x3fffffff );
   }

   // Implementation of the raw accessor functions:

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPassBits_v1, uint32_t,
                                         size, setSize )
   AUXSTORE_OBJECT_SETTER_AND_GETTER( TrigPassBits_v1, std::vector< uint32_t >,
                                      passBits, setPassBits )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPassBits_v1, uint32_t,
                                         containerKey, setContainerKey )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( TrigPassBits_v1, uint32_t,
                                         containerClid, setContainerClid )

} // namespace xAOD
