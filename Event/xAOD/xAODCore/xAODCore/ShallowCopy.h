// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowCopy.h 687601 2015-08-05 10:23:25Z krasznaa $
#ifndef XAODCORE_SHALLOWCOPY_H
#define XAODCORE_SHALLOWCOPY_H

// System include(s):
#include <map>
#include <iostream>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthContainersInterfaces/IConstAuxStore.h"

// Local include(s):
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCore/ShallowAuxInfo.h"

namespace xAOD {

   /// Function making a shallow copy of a constant container
   ///
   /// This function can be used to make a shallow copy of an existing
   /// (constant) container. It is most useful when reading an input file,
   /// and/or applying systematic variations on a container.
   ///
   /// @param cont The container to make a shallow copy of
   /// @returns A pair of pointers to the created objects. The caller takes
   ///          ownership of the created objects
   ///
   template< class T >
   std::pair< T*, ShallowAuxContainer* > shallowCopyContainer( const T& cont ) {

      // Create a DataLink, to check if we'll be successful:
      DataLink< SG::IConstAuxStore > link( cont.getConstStore() );
      if( ! link.cptr() ) {
         // This can happen when we read the input file in standalone
         // mode in branch access mode. That's unfortunately not
         // compatible with using a shallow copy auxiliary store.
         std::cout << "xAOD::shallowCopyContainer ERROR Couldn't set up "
                   << "DataLink to the original container's auxiliary store"
                   << std::endl;
         std::cout << "xAOD::shallowCopyContainer ERROR Are you using the "
                   << "xAOD::TEvent::kBranchAccess access mode?"
                   << std::endl;
         std::pair< T*, ShallowAuxContainer* > dummy;
         return dummy;
      }

      // Create the new DV container:
      T* newCont = new T();

      // Add the required number of elements to it:
      newCont->reserve( cont.size() );
      for( size_t i = 0; i < cont.size(); ++i ) {
         newCont->push_back( new typename T::base_value_type() );
      }

      // Create a new shallow auxiliary container:
      ShallowAuxContainer* aux = new ShallowAuxContainer( link );

      // Connect it to the interface container:
      newCont->setStore( aux );

      // Return the new objects:
      return std::make_pair( newCont, aux );
   }

   /// Function making a shallow copy of a constant standalone object
   ///
   /// This function can be used to make a shallow copy of an existing
   /// (constant) object. It is most useful when reading an input file,
   /// and/or applying systematic variations on a standalone object. Not
   /// entirely clear yet how much use case there will be for it though...
   ///
   /// @param obj The object to make a shallow copy of
   /// @returns A pair of pointers to the created objects. The caller takes
   ///          ownership of the created objects
   ///
   template< class T >
   std::pair< T*, ShallowAuxInfo* > shallowCopyObject( const T& obj ) {

      // Create a DataLink, to check if we'll be successful:
      DataLink< SG::IConstAuxStore > link( obj.getConstStore() );
      if( ! link.cptr() ) {
         // This can happen when we read the input file in standalone
         // mode in branch access mode. That's unfortunately not
         // compatible with using a shallow copy auxiliary store.
         std::cout << "xAOD::shallowCopyObject ERROR Couldn't set up "
                   << "DataLink to the original object's auxiliary store"
                   << std::endl;
         std::cout << "xAOD::shallowCopyObject ERROR Are you using the "
                   << "xAOD::TEvent::kBranchAccess access mode?"
                   << std::endl;
         std::pair< T*, ShallowAuxInfo* > dummy;
         return dummy;
      }

      // Create a new object using its default constructor:
      T* newObj = new T();

      // Create a new shallow auxiliary store:
      ShallowAuxInfo* aux = new ShallowAuxInfo( link );

      // Connect it to the interface object:
      newObj->setStore( aux );

      // Return the new objects:
      return std::make_pair( newObj, aux );
   }

} // namespace xAOD

#endif // XAODCORE_SHALLOWCOPY_H
