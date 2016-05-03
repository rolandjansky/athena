// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TVirtualEvent.h 682043 2015-07-10 09:21:12Z krasznaa $
#ifndef XAODROOTACCESSINTERFACES_TVIRTUALEVENT_H
#define XAODROOTACCESSINTERFACES_TVIRTUALEVENT_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

// Forward declaration(s):
namespace std {
   class type_info;
}

namespace xAOD {

   /// @short Base interface for getting objects out of the input file
   ///
   /// In order for the smart pointers not to depend on the full xAOD::TEvent
   /// class, this interface collects all the functionality that is needed
   /// by the smart pointers to access the input objects.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 682043 $
   /// $Date: 2015-07-10 11:21:12 +0200 (Fri, 10 Jul 2015) $
   ///
   class TVirtualEvent {

   public:
      /// Virtual destructor to make vtable happy
      virtual ~TVirtualEvent() {}

      /// Key for retrieving the "default" object of a given type
      static const uint32_t DEFAULT_KEY = 0xffffffff;
      /// Mask for the keys, used mostly internally
      static const uint32_t KEY_MASK = 0x3fffffff;

      /// Function retrieving an object from the event (constant version)
      template< typename T >
      bool retrieve( const T*& obj, uint32_t key = DEFAULT_KEY,
                     bool silent = false );
      /// Function retrieving an object from the event (constant version)
      template< typename T >
      bool retrieve( const T*& obj, const std::string& key,
                     bool silent = false );

      /// Function returning the hash describing an object's name/key
      virtual uint32_t getHash( const std::string& key ) const = 0;
      /// Function returning the hash describing a known object
      virtual uint32_t getKey( const void* obj ) const = 0;
      /// Function returning the key describing a known object
      virtual const std::string& getName( const void* obj ) const = 0;
      /// Function returning the key describing a known object
      virtual const std::string& getName( uint32_t hash ) const = 0;

   protected:
      /// Function for retrieving an output object in a non-template way
      virtual void* getOutputObject( uint32_t key,
                                     const std::type_info& ti ) = 0;
      /// Function for retrieving an input object in a non-template way
      virtual const void* getInputObject( uint32_t key,
                                          const std::type_info& ti,
                                          bool silent = false ) = 0;

   }; // class TVirtualEvent

} // namespace xAOD

// Include the template implementation:
#include "TVirtualEvent.icc"

#endif // XAODROOTACCESSINTERFACES_TVIRTUALEVENT_H
