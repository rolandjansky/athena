// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ShallowAuxContainer.h 793737 2017-01-24 20:11:10Z ssnyder $
#ifndef XAODCORE_SHALLOWAUXCONTAINER_H
#define XAODCORE_SHALLOWAUXCONTAINER_H

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/tools/threading.h"
#ifndef XAOD_STANDALONE
#   include "AthenaKernel/ILockable.h"
#endif // not XAOD_STANDALONE
#include "CxxUtils/checker_macros.h"

// Local include(s):
#include "xAODCore/AuxSelection.h"

namespace xAOD {

   /// Class creating a shallow copy of an existing auxiliary container
   ///
   /// During analysis, in order to create objects that override just some
   /// of the properties of an original, it's wasteful to make full copies
   /// of the objects. Not to mention that the full copies have generally
   /// issues with dynamic variables.
   ///
   /// The behaviour of this class is that it takes all the properties that
   /// it doesn't know about from the parent auxiliary store, but the ones
   /// that it has an override for, it provides itself.
   ///
   /// The object is persistifiable. The user can choose whether to only
   /// write out the variables that were modified wrt. the original container,
   /// or possibly all the variables. (In case the original container is
   /// not getting written out.)
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 793737 $
   /// $Date: 2017-01-24 21:11:10 +0100 (Tue, 24 Jan 2017) $
   ///
   class ShallowAuxContainer : public SG::IAuxStore,
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
      ShallowAuxContainer( bool standalone = false );
      /// Copy constructor
      ShallowAuxContainer( const ShallowAuxContainer& parent );
      /// Constructor with a parent object
      ShallowAuxContainer( const DataLink< SG::IConstAuxStore >& parent,
                           bool standalone = false );
      /// Destructor
      ~ShallowAuxContainer();

      /// Assignment operator
      ShallowAuxContainer& operator= ( const ShallowAuxContainer& rhs );

      /// Get the parent store
      const DataLink< SG::IConstAuxStore >& parent() const;
      /// Set the parent store
      void setParent( const DataLink< SG::IConstAuxStore >& link );

      /// Check whether only the overriden parameters will be written out or not
      bool shallowIO() const;
      /// Set whether only the overriden parameters should be written out
      void setShallowIO( bool value );

      /// @name Functions implementing the SG::IAuxStoreHolder interface
      /// @{

      /// Get the currently used internal store object
      virtual SG::IAuxStore* getStore() override;
      virtual const SG::IAuxStore* getStore() const override;
      /// Set a different internal store object
      virtual void setStore( SG::IAuxStore* store ) override;
      /// Return the type of the store object
      virtual AuxStoreType getStoreType() const override { return AST_ContainerStore; }

      /// @}

      /// @name Functions implementing the SG::IConstAuxStore interface
      /// @{

      /// Get a pointer to a given array
      virtual const void* getData( auxid_t auxid ) const override;

      /// Get the types(names) of variables handled by this container
      virtual const auxid_set_t& getAuxIDs() const override;

      /// Test if a variable is a decoration.
      virtual bool isDecoration (auxid_t auxid) const override;

      /// Return the data vector for one aux data decoration item.
      virtual void* getDecoration (auxid_t auxid, size_t size, size_t capacity) override;
      /// Lock the container.
      virtual void lock() override;
      /// Clear all decorations.
      virtual bool clearDecorations() override;
      /// Get the size of the container.
      virtual size_t size() const override;
      /// Lock a decoration.
      virtual void lockDecoration (SG::auxid_t auxid) override;
      /// @}

      /// @name Functions implementing the SG::IAuxStore interface
      /// @{

      /// Get a pointer to a given array, creating the array if necessary
      virtual void* getData( auxid_t auxid, size_t size,
                             size_t capacity ) override;

      /// Return a set of writable data identifiers
      virtual const auxid_set_t& getWritableAuxIDs() const override;

      /// Resize the arrays to a given size
      virtual bool resize( size_t size ) override;
      /// Reserve a given size for the arrays
      virtual void reserve( size_t size ) override;
      /// Shift the contents of the stored arrays
      virtual void shift( size_t pos, ptrdiff_t offs ) override;
      /// Insert contents of another store via move.
      virtual bool insertMove (size_t pos,
                               IAuxStore& other,
                               const SG::auxid_set_t& ignore) override;
 
      /// @}

      /// @name Functions implementing the SG::IAuxStoreIO interface
      /// @{

      /// Get a pointer to the data being stored for one aux data item
      virtual const void* getIOData( auxid_t auxid ) const override;

      /// Return the type of the data to be stored for one aux data item
      virtual const std::type_info* getIOType( auxid_t auxid ) const override;

      /// Get the types(names) of variables created dynamically
      virtual const auxid_set_t& getDynamicAuxIDs() const override;

      /// Select dynamic Aux attributes by name (for writing)
      virtual void selectAux( const std::set< std::string >& attributes ) override;

      /// Get the IDs of the selected dynamic Aux variables (for writing)
      virtual auxid_set_t getSelectedAuxIDs() const override;

      /// @}

      /// @name Functions managing the instance name of the container
      /// @{

      /// Get the name of the container instance
      const char* name() const;
      /// Set the name of the container instance
      void setName( const char* name );

      /// @}

   private:
      void remakeAuxIDs() const;

      /// Dynamic attributes selection implementation
      AuxSelection m_selection;

      /// Internal dynamic auxiliary store object
      SG::IAuxStore* m_store;
      /// The IO interface to the internal auxiliary store
      SG::IAuxStoreIO* m_storeIO;
      /// Flag deciding if the object owns the dynamic store or not
      bool m_ownsStore;
      /// Flag keeping track of whether this object is locked or not
      bool m_locked;

      /// Link to the parent object
      DataLink< SG::IConstAuxStore > m_parentLink;
      /// Optional pointer to the IO interface of the parent object
      const SG::IAuxStoreIO* m_parentIO;
      /// Flag for whether to do "shallow IO" or not
      bool m_shallowIO;

      /// Mutex for multithread synchronization.
      typedef AthContainers_detail::mutex mutex_t;
      typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
      mutable mutex_t m_mutex;

      // Mutable is thread-safe since we lock access, and the object
      // is a ConcurrentBitset.
      mutable auxid_set_t m_auxids ATLAS_THREAD_SAFE;
      mutable bool m_auxidsValid ATLAS_THREAD_SAFE;

      /// Name of the container in memory. Set externally.
      std::string m_name;

   }; // class ShallowAuxContainer

} // namespace xAOD

// Declare a class ID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ShallowAuxContainer, 1111823638, 1 )

// Describe the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASES3( xAOD::ShallowAuxContainer, SG::IAuxStore, SG::IAuxStoreIO,
           SG::IAuxStoreHolder );

#endif // XAODCORE_SHALLOWAUXCONTAINER_H
