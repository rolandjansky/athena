// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBits_v1.h 773869 2016-09-19 15:27:05Z krasznaa $
#ifndef XAODTRIGGER_VERSIONS_TRIGPASSBITS_V1_H
#define XAODTRIGGER_VERSIONS_TRIGPASSBITS_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <memory>
#include <string>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/DataLink.h"

namespace xAOD {

   /// Type describing which elements in a container passed a trigger chain
   ///
   /// This type allows us to attach simple pass/fail information to the
   /// containers that we reconstructed in Fex algorithms, in Hypo algorithms.
   /// In this case we can't just decorate the original objects, as we may want
   /// to evaluate a lot of hypotheses on the exact same objects.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   /// @author Tomasz Bold <Tomasz.Bold@cern.ch>
   ///
   /// $Revision: 773869 $
   /// $Date: 2016-09-19 17:27:05 +0200 (Mon, 19 Sep 2016) $
   ///
   class TrigPassBits_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      TrigPassBits_v1();

      /// Default hashed container key, in case no explicit value was specified
      static const uint32_t DEFAULT_KEY = 0xffff;

      /// @name Helper functions for easier usage
      /// @{

      /// Function configuring and resetting the object for a new target
      template< class CONT >
      void reset( const CONT* container, uint32_t containerKey = 0xffff );

      /// Mark one of the elements of the target container as passing/failing
      template< class OBJ, class CONT >
      void markPassing( const OBJ* obj, const CONT* container,
                        bool passed = true );
      /// Mark one of the elements of the target container as passing/failing
      void markPassing( size_t index, bool passed = true );

      /// Check if an element of a container is passing/failing
      template< class OBJ, class CONT >
      bool isPassing( const OBJ* obj, const CONT* container ) const;
      /// Check if an element of a container is passing/failing
      template< class OBJ, class CONT >
      bool isPassing( const OBJ* obj, const CONT* container,
                      uint32_t containerKey ) const;
      /// Check if an element of a container is passing/failing
      template< class OBJ, class CONT >
      bool isPassing( const OBJ* obj, const CONT* container,
                      const std::string& containerKey ) const;
      /// Check if an element of a container is passing/failing
      bool isPassing( size_t index ) const;

      /// Function calculating a "hash" out of a string
      static uint32_t hash( const std::string& key );

      /// @}

      /// @name Raw data accessors
      /// @{

      /// The size of the target container
      uint32_t size() const;
      /// Set the size of the target container
      void setSize( uint32_t value );

      /// The internal bitmap created with the hypo decisions
      const std::vector< uint32_t >& passBits() const;
      /// Set the internal bitmap directly (not recommended)
      void setPassBits( const std::vector< uint32_t >& value );

      /// Hashed SG key of the target container
      uint32_t containerKey() const;
      /// Set the hashed SG key of the target container
      void setContainerKey( uint32_t value );

      /// CLID of the target container
      uint32_t containerClid() const;
      /// Set the CLID of the target container
      void setContainerClid( uint32_t value );

      /// @}

   private:
      /// Transient pointer to the target container
      const void* m_container;

   }; // class TrigPassBits_v1

   /// Helper function creating a new object to describe a target container
   template< class CONT >
   std::unique_ptr< TrigPassBits_v1 >
   makeTrigPassBits( const CONT* container, uint32_t containerKey =
         xAOD::TrigPassBits_v1::DEFAULT_KEY );

   /// Helper function creating a new object to describe a target container
   template< class CONT >
   std::unique_ptr< TrigPassBits_v1 >
   makeTrigPassBits( const CONT* container, const std::string& containerKey );

} // namespace xAOD

// Include the template implementation:
#include "TrigPassBits_v1.icc"

#endif // XAODTRIGGER_VERSIONS_TRIGPASSBITS_V1_H
