// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxContainerBase.h 611401 2014-08-12 12:24:53Z krasznaa $
#ifndef XAODCORE_AUXCONTAINERBASE_H
#define XAODCORE_AUXCONTAINERBASE_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/tools/threading.h"
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/ILockable.h"
#endif // not XAOD_STANDALONE

// Local include(s):
#include "xAODCore/AuxSelection.h"

// Forward declaration(s):
namespace SG {
   class IAuxTypeVector;
}

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

   /// Common base class for the auxiliary containers
   ///
   /// To make the development of auxiliary containers simpler,
   /// they can all inherit from this one class. Then all they
   /// need to do is just to declare their data members, everything
   /// else is taken care of by this transient base class.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 611401 $
   /// $Date: 2014-08-12 14:24:53 +0200 (Tue, 12 Aug 2014) $
   ///
   class AuxContainerBase : public SG::IAuxStore,
                            public SG::IAuxStoreIO,
                            public SG::IAuxStoreHolder
#ifndef XAOD_STANDALONE
                          , public ILockable
#endif // not XAOD_STANDALONE
   {

   public:
      /// The aux ID type definition
      typedef SG::auxid_t auxid_t;
      /// The aux ID set type definition
      typedef SG::auxid_set_t auxid_set_t;

      /// Default constructor
      AuxContainerBase( bool allowDynamicVars = true );
      /// Copy constructor
      AuxContainerBase( const AuxContainerBase& parent );
      /// Constructor receiving a "dynamic auxiliary store"
      AuxContainerBase( const SG::IAuxStore* store );
      /// Destructor
      ~AuxContainerBase();

      /// Assignment operator
      AuxContainerBase& operator=( const AuxContainerBase& rhs );

      /// @name Functions implementing the SG::IAuxStoreHolder interface
      /// @{

      /// Get the currently used internal store object
      virtual SG::IAuxStore* getStore() const;
      /// Set a different internal store object
      virtual void setStore( SG::IAuxStore* store );
      /// Return the type of the store object
      virtual AuxStoreType getStoreType() const { return AST_ContainerStore; }

      /// @}

      /// @name Functions implementing the SG::IConstAuxStore interface
      /// @{

      /// Get a pointer to a given array
      virtual const void* getData( auxid_t auxid ) const;

      /// Get the types(names) of variables handled by this container
      virtual const auxid_set_t& getAuxIDs() const;

      /// Get a pointer to a given array, as a decoration.
      virtual void* getDecoration (auxid_t auxid, size_t size, size_t capacity);

      /// Lock the container.
      virtual void lock();

      /// Clear all decorations.
      virtual void clearDecorations();

      /// Get the size of the container.
      virtual size_t size() const;

      /// @}

      /// @name Functions implementing the SG::IAuxStore interface
      /// @{

      /// Get a pointer to a given array, creating the array if necessary
      virtual void* getData( auxid_t auxid, size_t size,
                             size_t capacity );

      /// Return a set of writable data identifiers
      virtual const auxid_set_t& getWritableAuxIDs() const;

      /// Resize the arrays to a given size
      virtual void resize( size_t size );
      /// Reserve a given size for the arrays
      virtual void reserve( size_t size );
      /// Shift the contents of the stored arrays
      virtual void shift( size_t pos, ptrdiff_t offs );

      /// @}

      /// @name Functions implementing the SG::IAuxStoreIO interface
      /// @{

      /// Get a pointer to the data being stored for one aux data item
      virtual const void* getIOData( auxid_t auxid ) const;

      /// Return the type of the data to be stored for one aux data item
      virtual const std::type_info* getIOType( auxid_t auxid ) const;

      /// Get the types(names) of variables created dynamically
      virtual const auxid_set_t& getDynamicAuxIDs() const;

      /// Select dynamic Aux attributes by name (for writing)
      virtual void selectAux( const std::set< std::string >& attributes );

      /// Get the IDs of the selected dynamic Aux variables (for writing)
      virtual const SG::auxid_set_t& getSelectedAuxIDs() const;

      /// @}

   protected:
      /// Register one of the persistent variables internally
      template< typename T >
      void regAuxVar( const std::string& name,
                      std::vector< T >& vec );

   private:
      /// Dynamic attributes selection implementation
      AuxSelection  m_selection;
      /// Internal list of all available variables
      mutable auxid_set_t m_auxids;
      /// Internal list of all managed variables
      std::vector< SG::IAuxTypeVector* > m_vecs;

      /// Internal dynamic auxiliary store object
      SG::IAuxStore* m_store;
      /// The IO interface to the internal auxiliary store
      SG::IAuxStoreIO* m_storeIO;
      /// Flag deciding if the object owns the dynamic store or not
      bool m_ownsStore;
      /// Has the container been locked?
      bool m_locked;

      /// Count changes to @c m_auxids.
      mutable size_t m_tick;

      /// Mutex for multithread synchronization.
      typedef AthContainers_detail::mutex mutex_t;
      typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
      mutable mutex_t m_mutex;

      /// Thread-local versions of the auxid set.
      struct TSAuxidSet
      {
        size_t m_tick;
        auxid_set_t m_set;
        TSAuxidSet (size_t tick, const auxid_set_t& set)
          : m_tick (tick), m_set (set) {}
      };
      mutable AthContainers_detail::thread_specific_ptr<TSAuxidSet> m_tsAuxids;

   }; // class AuxContainerBase

} // namespace xAOD

// Declare a class ID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::AuxContainerBase, 1225080690, 1 )

// Describe the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASES3( xAOD::AuxContainerBase, SG::IAuxStore, SG::IAuxStoreIO,
           SG::IAuxStoreHolder );

// Include the template implementation:
#include "AuxContainerBase.icc"

#endif // XAODCORE_AUXCONTAINERBASE_H
