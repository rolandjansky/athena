// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowCopy.h 766390 2016-08-04 11:18:59Z wlampl $
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

#ifndef XAOD_STANDALONE
#include <GaudiKernel/ThreadLocalContext.h>
#else
class EventContext;
#endif


namespace xAOD {

  /// Function to prepare an object to be stored in a shallow-copy container
  ///
  /// To be used by shallowCopyContainer. The default implementation
  /// here is dummy, just calling the default constructor. Can be
  /// overloaded for types that require special treatment
  /// (like xAOD::CaloCluster)
  /// @param elem: Input object, ignored in this implementation
  /// @returns A uniqe_ptr to the object to be push_back'ed into the
  ///          shallow-copy container.

  template< class T >
  std::unique_ptr<T> prepareElementForShallowCopy(const T* /*elem*/) {
    return std::make_unique<T>();
  }

  namespace detail{
  /// Impl function for shallow copy container
  /// @param cont The container to make a shallow copy of
  /// @param ctx The currentEventContext
  /// @returns A pair of unique_ptr to the created objects.
  template<class T>
  std::pair<std::unique_ptr<T>, std::unique_ptr<ShallowAuxContainer>>
  shallowCopyContainerImpl(const T& cont,
                           DataLink<SG::IConstAuxStore>& link){

    if (!link.cptr()) {
      // This can happen when we read the input file in standalone
      // mode in branch access mode. That's unfortunately not
      // compatible with using a shallow copy auxiliary store.
      std::cout << "xAOD::shallowCopyContainer ERROR Couldn't set up "
                << "DataLink to the original container's auxiliary store"
                << std::endl;
      std::cout << "xAOD::shallowCopyContainer ERROR Are you using the "
                << "xAOD::TEvent::kBranchAccess access mode?" << std::endl;
      return {};
    }

    // Create the new DV container:
    auto newCont = std::make_unique<T>();

    // Add the required number of elements to it:
    newCont->reserve(cont.size());
    for (size_t i = 0; i < cont.size(); ++i) {
      newCont->push_back(prepareElementForShallowCopy(cont[i]));
    }

    // Create a new shallow auxiliary container:
    auto aux = std::make_unique<ShallowAuxContainer>(link);

    // Connect it to the interface container:
    newCont->setStore(aux.get());

    // Return the new objects:
    return std::make_pair(std::move(newCont), std::move(aux));
  }
  }//end namespace detail

  /// Function making a shallow copy of a constant container
  ///
  /// This function can be used to make a shallow copy of an existing
  /// (constant) container. It is most useful when reading an input file,
  /// and/or applying systematic variations on a container.
  ///
  /// @param cont The container to make a shallow copy of
  /// @param ctx The currentEventContext
  /// @returns A pair of unique_ptr to the created objects.
  ///
  /// In Analysis Base it can be still called by using
  /// Gaudi::Hive::currentContext() as 2nd argument,
  /// although in practice is ignored as we do not really have
  /// an actual EventContext type.
  ///
  /// Be aware: If CONT has decorations, then the scheduler won't automatically
  /// known that they are available on the copy.  To make that happen, see
  /// StoreGate/ShallowCopyDecorDeps.h.

  template<class T>
  std::pair<std::unique_ptr<T>, std::unique_ptr<ShallowAuxContainer>>
  shallowCopyContainer(const T& cont,
                       [[maybe_unused]] const EventContext& ctx
  ){
#ifndef XAOD_STANDALONE
    DataLink<SG::IConstAuxStore> link (cont.getConstStore(), ctx);
#else
    //Note that in AnalysisBase
    //- EventContext, is not a complete type, we have no class.
    //- We do not have a DataLink ctor with EventContext
    //- The return value of currentContext() can not be used.
    DataLink<SG::IConstAuxStore> link(cont.getConstStore());
#endif
    return detail::shallowCopyContainerImpl(cont,link);
  }

   /// Function making a shallow copy of a constant container
   //
   ///
   /// This function can be used to make a shallow copy of an existing
   /// (constant) container. It is most useful when reading an input file,
   /// and/or applying systematic variations on a container.
   ///
   /// @param cont The container to make a shallow copy of
   /// @returns A pair of pointers to the created objects. The caller takes
   ///          ownership of the created objects
   ///
   /// Be aware: If CONT has decorations, then the scheduler won't automatically
   /// known that they are available on the copy.  To make that happen, see
   /// StoreGate/ShallowCopyDecorDeps.h.
   ///
   template< class T >
   std::pair< T*, ShallowAuxContainer* > shallowCopyContainer( const T& cont ) {
     DataLink<SG::IConstAuxStore> link (cont.getConstStore());
     auto tmp = detail::shallowCopyContainerImpl(cont, link);
     return std::make_pair(tmp.first.release(), tmp.second.release());
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
   /// Be aware: If CONT has decorations, then the scheduler won't automatically
   /// known that they are available on the copy.  To make that happen, see
   /// StoreGate/ShallowCopyDecorDeps.h.
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
