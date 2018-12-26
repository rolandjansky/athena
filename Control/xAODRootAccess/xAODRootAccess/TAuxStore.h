// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxStore.h 793778 2017-01-25 04:06:29Z ssnyder $
#ifndef XAODROOTACCESS_TAUXSTORE_H
#define XAODROOTACCESS_TAUXSTORE_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "AthContainersInterfaces/IAuxStore.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/tools/threading.h"
#include "xAODCore/AuxSelection.h"

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"

#include "Rtypes.h"

// Forward declaration(s):
class TTree;
class TBranch;
namespace SG {
   class IAuxTypeVector;
   class AuxStoreInternal;
}

namespace xAOD {

   // Forward declaration(s):
   class TEvent;

   /// @short "ROOT implementation" of IAuxStore
   ///
   /// This is a "D3PDReader-like" implementation for the
   /// auxiliary store interface. It is meant to provide very
   /// efficient access to a relatively small number of auxiliary
   /// variables.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 793778 $
   /// $Date: 2017-01-25 05:06:29 +0100 (Wed, 25 Jan 2017) $
   ///
   class TAuxStore : public SG::IAuxStore,
                     public SG::IAuxStoreIO {

      /// Make TEvent a friend of this class
      friend class TEvent;

   public:
      /// "Structural" modes of the object
      enum EStructMode {
         kUndefinedStore = 0, ///< The structure mode is not defined
         kContainerStore = 1, ///< The object describes an entire container
         kObjectStore    = 2  ///< The object describes a single object
      };
      /// Convenience type declaration
      typedef SG::auxid_t auxid_t;
      /// Convenience type declaration
      typedef SG::auxid_set_t auxid_set_t;

      /// Default constructor
      TAuxStore( const char* prefix = "", Bool_t topStore = kTRUE,
                 EStructMode mode = kUndefinedStore,
                 Int_t basketSize = 2048, Int_t splitLevel = 0 );
      /// Destructor
      ~TAuxStore();

      /// Get what structure mode the object was constructed with
      EStructMode structMode() const;
      /// Set the structure mode of the object to a new value
      TReturnCode setStructMode( EStructMode mode );

      /// Get the currently configured branch name prefix
      const char* prefix() const;
      /// Set the branch name prefix
      void setPrefix( const char* prefix );

      /// Check if the object is a "top store", or not
      Bool_t isTopStore() const;
      /// Set whether the object should behave as a "top store" or not
      void setTopStore( Bool_t value = kTRUE );

      /// Get the size of the baskets created for the output branches
      Int_t basketSize() const;
      /// Set the size of the baskets created for the output branches
      void setBasketSize( Int_t value );

      /// Get the split level of the output branches
      Int_t splitLevel() const;
      /// Set the split level of the output branches
      void setSplitLevel( Int_t value );

      /// Connect the object to an input TTree
      TReturnCode readFrom( ::TTree* tree, ::Bool_t printWarnings = kTRUE );
      /// Connect the object to an output TTree
      TReturnCode writeTo( ::TTree* tree );

      /// Read the values from the specified TTree entry
      Int_t getEntry( Long64_t entry, Int_t getall = 0 );

      /// Tell the object that all branches will need to be re-read
      void reset();

      /// @name Functions implementing the SG::IConstAuxStore interface
      /// @{

      /// Get a pointer to a given array
      virtual const void* getData( auxid_t auxid ) const override;

      /// Get the types(names) of variables handled by this container
      virtual const auxid_set_t& getAuxIDs() const override;

      /// Get a pointer to a given array, creating the array if necessary
      virtual void* getDecoration( auxid_t auxid, size_t size,
                                   size_t capacity ) override;

      /// Test if a variable is a decoration.
      virtual bool isDecoration (auxid_t auxid) const override;

      /// Lock the object, and don't let decorations be added
      virtual void lock() override;
      /// Remove the decorations added so far. Only works for transient
      /// decorations.
      virtual bool clearDecorations() override;

      /// Lock a decoration.
      virtual void lockDecoration (SG::auxid_t auxid) override;

      /// Return the number of elements in the store
      virtual size_t size() const override;

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

      /// Select dynamic auxiliary attributes for writing
      virtual void selectAux( const std::set< std::string >& attributes ) override;

      /// Get the IDs of the selected aux variables
      virtual auxid_set_t getSelectedAuxIDs() const override;

      /// @}

   private:
      /// Function used for setting up the statistics info about the managed branches
      TReturnCode initStats( ::TTree* tree );
      /// Connect a variable to the input tree
      TReturnCode setupInputData( auxid_t auxid ) const;
      /// Connect a variable to the output tree
      TReturnCode setupOutputData( auxid_t auxid ) const;
      /// Scan the input TTree for auxiliary branches
      TReturnCode scanInputTree();
      /// Register one input branch as an available auxiliary variable
      TReturnCode setupAuxBranch( ::TBranch* br, const char* auxName,
                                  ::Bool_t staticBranch );
      /// Check if this auxiliary variable needs to go to the output
      ::Bool_t isAuxIDSelected( auxid_t auxid ) const;
      /// Check if a branch holds a primitive variable or not
      static ::Bool_t isPrimitiveBranch( TBranch* br );
      /// Check if a branch describes a container or a single object
      static ::Bool_t isContainerBranch( TBranch* br, auxid_t auxid );
      /// Check if the auxiliary variable has a registered type
      static ::Bool_t isRegisteredType( auxid_t auxid );

      /// Static prefix for the branch names
      std::string m_prefix;
      /// Dynamic prefix for the branch names
      std::string m_dynPrefix;

      /// Flag stating whether this is a "top store"
      ::Bool_t m_topStore;

      /// The "structural" mode of the object
      mutable EStructMode m_structMode;
      /// The basket size for the output branches
      ::Int_t m_basketSize;
      /// The split level for the output branches
      ::Int_t m_splitLevel;

      /// Current entry being read from an input tree
      ::Long64_t m_entry;

      /// The TTree being read from
      ::TTree* m_inTree;
      /// The TTree being written to
      ::TTree* m_outTree;

      /// "Scan status" of the input TTree
      ::Bool_t m_inputScanned;

      /// Object helping to select which auxiliary variables to write
      AuxSelection m_selection;
      /// Store for the in-memory-only variables
      SG::AuxStoreInternal* m_transientStore;

      /// Internal list of variable IDs handled currently by the object
      mutable auxid_set_t m_auxIDs;
      /// Variables handled currently by the object
      mutable std::vector< SG::IAuxTypeVector* > m_vecs;
      /// The current size of the container being described
      mutable std::size_t m_size;

      /// Is this container locked?
      ::Bool_t m_locked;
      /// Flags items as decorations
      mutable std::vector< ::Bool_t > m_isDecoration;

      /// Mutex type for multithread synchronization
      typedef AthContainers_detail::mutex mutex_t;
      /// Guard type for multithreaded synchronisation
      typedef AthContainers_detail::lock_guard< mutex_t > guard_t;
      /// Mutexes object used for multithreaded synchronisation
      mutable mutex_t m_mutex1, m_mutex2;

      /// Class used internally to implement just-in-time reading
      ///
      /// This is a much simplified version of the code that we used in the
      /// D3PDReader code to read variables just in time for the users.
      ///
      class TBranchHandle {

      public:
         /// Constructor
         TBranchHandle( ::Bool_t staticBranch, ::Bool_t primitiveBranch,
                        const std::type_info* ti, void* obj,
                        SG::auxid_t auxid, const std::string* prefix );

         /// Get an entry from the branch
         ::Int_t getEntry( ::Long64_t entry );
         /// Get a pointer to the branch being held
         ::TBranch** branchPtr();
         /// Get a pointer to the object
         void* objectPtr();
         /// Get a pointer to the object (or its pointer) in memory for reading
         void* inputObjectPtr();
         /// Get a pointer to the object (or its pointer) in memory for writing
         void* outputObjectPtr();
         /// Get the type of the variable in memory
         const std::type_info* typeInfo() const;
         /// Tell the object that the branch contents need to be re-read
         void reset();

      private:
         /// The branch that this object is handling
         ::TBranch* m_branch;
         /// The pointer to the object in memory
         void* m_object;
         /// Is this a static variable in question?
         ::Bool_t m_static;
         /// Is this a primitive variable?
         ::Bool_t m_primitive;
         /// Type info for the variable
         const std::type_info* m_typeInfo;
         /// Read status flag
         ::Bool_t m_needsRead;
         /// The auxiliary ID of the branch
         SG::auxid_t m_auxid;
         /// Pointer to the m_prefix string of the parent object
         const std::string* m_prefix;

      }; // class TBranchHandle

      /// Branches reading the various auxiliary variables
      mutable std::vector< TBranchHandle* > m_branches;
      /// "Write status" of the different variables
      mutable std::vector< bool > m_branchesWritten;
      /// Mark branches we've found to be missing. (Because TTree::GetBranch
      /// is very expensive.)
      mutable std::vector< bool > m_missingBranches;

   }; // class TAuxStore

} // namespace xAOD

#endif // XAODROOTACCESS_TAUXSTORE_H
