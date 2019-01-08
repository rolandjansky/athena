/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TAuxStore.cxx 796624 2017-02-10 17:17:23Z ssnyder $

// System include(s):
#include <string.h>
#include <sstream>
#include <stdexcept>

// ROOT include(s):
#include <TError.h>
#include <TTree.h>
#include <TBranch.h>
#include <TString.h>
#include <TClass.h>
#include <TROOT.h>
#include <TVirtualCollectionProxy.h>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/exceptions.h"

// xAOD include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

// Local include(s):
#include "xAODRootAccess/TAuxStore.h"
#include "xAODRootAccess/tools/Utils.h"
#include "xAODRootAccess/tools/Message.h"
#include "xAODRootAccess/tools/TAuxVectorFactory.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

namespace {


TClass* lookupVectorType (TClass *cl)
{
  std::string tname = cl->GetName();
  tname += "::vector_type";
  TDataType* typ = gROOT->GetType (tname.c_str());
  if (typ)
    return TClass::GetClass (typ->GetFullTypeName());
  return nullptr;
}


}


namespace xAOD {

   TAuxStore::TAuxStore( const char* prefix, Bool_t topStore,
                         EStructMode mode, Int_t basketSize,
                         Int_t splitLevel )
      : SG::IAuxStore(), SG::IAuxStoreIO(),
        m_prefix( prefix ), m_dynPrefix( Utils::dynBranchPrefix( prefix ) ),
        m_topStore( topStore ), m_structMode( mode ), m_basketSize( basketSize ),
        m_splitLevel( splitLevel ), m_entry( 0 ), m_inTree( 0 ), m_outTree( 0 ),
        m_inputScanned( kFALSE ), m_selection(), m_transientStore( 0 ),
        m_auxIDs(), m_vecs(), m_size( 0 ), m_locked( kFALSE ), m_isDecoration(),
        m_mutex1(), m_mutex2(), 
        m_branches(), m_branchesWritten(), m_missingBranches() {

   }

   TAuxStore::~TAuxStore() {

      std::vector< SG::IAuxTypeVector* >::iterator vitr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator vend = m_vecs.end();
      for( ; vitr != vend; ++vitr ) {
         if( *vitr ) delete *vitr;
      }

      std::vector< TBranchHandle* >::iterator bitr = m_branches.begin();
      std::vector< TBranchHandle* >::iterator bend = m_branches.end();
      for( ; bitr != bend; ++bitr ) {
         if( *bitr ) delete *bitr;
      }

      if( m_transientStore ) delete m_transientStore;
   }

   TAuxStore::EStructMode TAuxStore::structMode() const {

      return m_structMode;
   }

   /// This function should be used with great care. It makes it possible for
   /// the TEvent code to set the type of the object once the type is known.
   /// But user code should probably not fiddle with this function.
   ///
   /// @param mode The structure mode to set
   /// @returns The usual TReturnCode values
   ///
   TReturnCode TAuxStore::setStructMode( EStructMode mode ) {

      // Only allow this on an uninitialised object:
      if( m_branches.size() || ( m_structMode != kUndefinedStore ) ) {
         ::Error( "xAOD::TAuxStore::setStructMode",
                  XAOD_MESSAGE( "Trying to change the structure mode of an "
                                "initialised object" ) );
         return TReturnCode::kFailure;
      }

      // Make the change:
      m_structMode = mode;
      return TReturnCode::kSuccess;
   }

   const char* TAuxStore::prefix() const {

      return m_prefix.c_str();
   }

   void TAuxStore::setPrefix( const char* prefix ) {

      m_prefix = prefix;
      m_dynPrefix = Utils::dynBranchPrefix( prefix );
      reset();
      return;
   }

   ::Bool_t TAuxStore::isTopStore() const {

      return m_topStore;
   }

   void TAuxStore::setTopStore( ::Bool_t value ) {

      m_topStore = value;
      reset();
      return;
   }

   ::Int_t TAuxStore::basketSize() const {

      return m_basketSize;
   }

   void TAuxStore::setBasketSize( ::Int_t value ) {

      m_basketSize = value;
      return;
   }

   ::Int_t TAuxStore::splitLevel() const {

      return m_splitLevel;
   }

   void TAuxStore::setSplitLevel( ::Int_t value ) {

      m_splitLevel = value;
      return;
   }

   /// This function is called by the infrastructure to connect the object
   /// to an input TTree whenever a new input file is opened.
   ///
   /// @param tree Pointer to the TTree that is being read from
   ///
   TReturnCode TAuxStore::readFrom( ::TTree* tree, ::Bool_t printWarnings ) {

      // Make sure that everything will be re-read after this:
      reset();

      // We will need to check again which branches are available:
      m_missingBranches.clear();

      // Remember the tree:
      m_inTree = tree;

      // Catalogue all the branches:
      RETURN_CHECK( "xAOD::TAuxStore::readFrom", scanInputTree() );

      // Check if we'll be likely to be able to read the "static"
      // variables:
      TBranch* br = m_inTree->GetBranch( m_prefix.c_str() );
      if( ! br ) {
         // We might not even have static branches, so this is not an error
         // by itself...
         return TReturnCode::kSuccess;
      }
      // In order to read complex objects, like smart pointers from an
      // auxiliary container variable-by-variable, the split level of the
      // branch must be exactly 1.
      if( ( br->GetSplitLevel() != 1 ) && m_topStore && printWarnings ) {
         ::Warning( "xAOD::TAuxStore::readFrom",
                    "Static branch (%s) with split level %i discovered",
                    m_prefix.c_str(), br->GetSplitLevel() );
         ::Warning( "xAOD::TAuxStore::readFrom",
                    "The reading of complex variables from it may/will fail!" );
      }

      return TReturnCode::kSuccess;
   }

   /// This function is called by the infrastructure to connect the object
   /// to an output TTree.
   ///
   /// @param tree Pointer to the TTree that is being written to
   ///
   TReturnCode TAuxStore::writeTo( ::TTree* tree ) {

      // Look for any auxiliary branches that have not been connected to yet:
      RETURN_CHECK( "xAOD::TAuxStore::writeTo", scanInputTree() );

      // Store the TTree pointer:
      m_outTree = tree;

      // Create all the variables that we already know about. Notice that the
      // code makes a copy of the auxid set on purpose. Because the underlying
      // AuxSelection object gets modified while doing the for loop.
      const auxid_set_t selAuxIDs = getSelectedAuxIDs();
      for (SG::auxid_t id : selAuxIDs) {
         RETURN_CHECK( "xAOD::TAuxStore::writeTo", setupOutputData( id ) );
      }

      return TReturnCode::kSuccess;
   }

   ::Int_t TAuxStore::getEntry( ::Long64_t entry, ::Int_t getall ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // Remember the entry:
      m_entry = entry;

      // Reset the transient store. TEvent::fill() calls this function with
      // getall==99. When that is happening, we need to keep the transient
      // store still around. Since the user may want to interact with the
      // object after it was written out. (And since TEvent::fill() asks for
      // the transient decorations after calling getEntry(...).)
      if( m_transientStore && ( getall != 99 ) ) {
         // Remove the transient auxiliary IDs from the internal list:
         m_auxIDs -= m_transientStore->getAuxIDs();
         // Delete the object:
         delete m_transientStore;
         m_transientStore = 0;
      }

      // Now remove the IDs of the decorations that are getting persistified:
      if( getall != 99 ) {
         for( auxid_t auxid = 0; auxid < m_isDecoration.size(); ++auxid ) {
            if( ! m_isDecoration[ auxid ] ) {
               continue;
            }
            m_auxIDs.erase( auxid );
         }
      }

      // If we don't need everything loaded, return now:
      if( ! getall ) return 0;

      // Get all the variables at once:
      ::Int_t bytesRead = 0;
      std::vector< TBranchHandle* >::iterator itr = m_branches.begin();
      std::vector< TBranchHandle* >::iterator end = m_branches.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) {
            bytesRead += ( *itr )->getEntry( entry );
         }
      }

      return bytesRead;
   }

   void TAuxStore::reset() {

      // Reset all the branches:
      std::vector< TBranchHandle* >::iterator itr = m_branches.begin();
      std::vector< TBranchHandle* >::iterator end = m_branches.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) ( *itr )->reset();
      }

      // Remember that the input TTree needs to be re-scanned:
      m_inputScanned = kFALSE;

      return;
   }

   const void* TAuxStore::getData( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // Check if the transient store already handles this variable:
      if( m_transientStore && 
          ( m_transientStore->getAuxIDs().test( auxid ) ) ) {
         return m_transientStore->getData( auxid );
      }

      // Connect this auxiliary variable both to the input and output
      // if needed:
      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ||
          ( auxid >= m_branches.size() ) || ( ! m_branches[ auxid ] ) ) {
         if( ( ! setupInputData( auxid ).isSuccess() ) ||
             ( ! setupOutputData( auxid ).isSuccess() ) ) {
            return 0;
         }
      }

      // Make sure the variable is up to date:
      const ::Int_t readBytes = m_branches[ auxid ]->getEntry( m_entry );
      if( readBytes < 0 ) {
         ::Error( "xAOD::TAuxStore::getData",
                  XAOD_MESSAGE( "Couldn't read in variable %s" ),
                  SG::AuxTypeRegistry::instance().getName( auxid ).c_str() );
         return 0;
      }

      // Return the pointer to the object:
      return m_vecs[ auxid ]->toPtr();
   }

   const TAuxStore::auxid_set_t& TAuxStore::getAuxIDs() const {

      return m_auxIDs;
   }

   void* TAuxStore::getDecoration( auxid_t auxid, size_t size,
                                   size_t capacity ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // Remember the requested size:
      m_size = size;

      // If this is a locked object, deal with it correctly:
      if( m_locked ) {
         // If the variable exists already and it's a decoration, then let's
         // give it back.
         if( ( auxid < m_vecs.size() ) && m_vecs[ auxid ] &&
             ( auxid < m_isDecoration.size() && m_isDecoration[ auxid ] ) ) {
            // Things look okay...
            m_vecs[ auxid ]->reserve( capacity );
            m_vecs[ auxid ]->resize( size );
            return m_vecs[ auxid ]->toPtr();
         }
         // If it's in the transient store already, return it from there.
         // Since in a locked store *everything* is a decoration in the
         // transient store.
         if( m_transientStore &&
             m_transientStore->getAuxIDs().test( auxid ) ) {
            return m_transientStore->getDecoration( auxid, size, capacity );
         }
         // If we know this auxiliary ID, but it was not found as a decoration
         // by the previous checks, then we're in trouble.
         if( m_auxIDs.test( auxid ) ) {
            // It may still be a decoration in a transient store. If such
            // a store exists, leave it up to that store to
            throw SG::ExcStoreLocked( auxid );
         }
      }

      // Check if we want to write this variable to the output:
      if( ( ! isAuxIDSelected( auxid ) ) || ( ! m_outTree ) ) {

         // Create the store only when necessary:
         if( ! m_transientStore ) {
            m_transientStore =
               new SG::AuxStoreInternal( m_structMode == kObjectStore );
            if( m_locked ) {
               m_transientStore->lock();
            }
         }
         // Let the transient store create the decoration:
         const size_t nids = m_transientStore->getAuxIDs().size();
         void* result = m_transientStore->getDecoration( auxid, size,
                                                         capacity );
         if( result && ( nids != m_transientStore->getAuxIDs().size() ) ) {
            m_auxIDs.insert( auxid );
         }
         // Return the memory address from the transient store:
         return result;
      }

      // Doesn't exist yet. So let's make it:
      void* result = getData( auxid, size, capacity );
      if( m_locked ) {
         // If the container is locked, remember that this is a decoration:
         if( m_isDecoration.size() <= auxid ) {
            m_isDecoration.resize( auxid + 1 );
         }
         m_isDecoration[ auxid ] = ::kTRUE;
      }

      // Return the pointer made by getData(...):
      return result;
   }


   bool TAuxStore::isDecoration (SG::auxid_t auxid) const
   {
     if (m_locked) {
       if (auxid < m_isDecoration.size() && m_isDecoration[auxid]) {
         return true;
       }
       if (m_transientStore) {
         return m_transientStore->isDecoration (auxid);
       }
     }
     return false;
   }


   void TAuxStore::lock() {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      m_locked = true;
      if( m_transientStore ) {
         m_transientStore->lock();
      }

      return;
   }

   bool TAuxStore::clearDecorations() {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // Clear the transient decorations:
      bool anycleared = false;
      if( m_transientStore ) {
         SG::auxid_set_t old_id_set = m_transientStore->getAuxIDs();

         // Clear the decorations from the transient store:
         anycleared = m_transientStore->clearDecorations();

         // Now remove ids that were cleared.
         if (anycleared) {
           old_id_set -= m_transientStore->getAuxIDs();
           // old_id_set is now the set of ids that were cleared.
           m_auxIDs -= old_id_set;
         }
      }

      // The decorations which are going into the output file, are here to stay.
      // Removing their IDs from the internal set would just cause more problems
      // in my mind than just leaving them be.

      return anycleared;
   }

   /// Lock a decoration.
   void TAuxStore::lockDecoration (SG::auxid_t auxid)
   { 
     if( m_transientStore ) {
       m_transientStore->lockDecoration (auxid);
     }
   }


   size_t TAuxStore::size() const {

      // First, try to find a managed vector in the store:
      for( SG::auxid_t id : m_auxIDs) {
         // Make sure that we are still within the bounds of our vector:
         if( id >= m_vecs.size() ) break;
         // Skip non-existent objects:
         if( ! m_vecs[ id ] ) continue;
         // Ask the vector for its size:
         const size_t size = m_vecs[ id ]->size();
         // Only accept a non-zero size. Not sure why...
         if( size > 0 ) {
            return size;
         }
      }

      // Check if we have a transient store, and get the size from that:
      if( m_transientStore ) {
         return m_transientStore->size();
      }

      // Apparently the store is empty:
      return 0;
   }

   void* TAuxStore::getData( auxid_t auxid, size_t size,
                             size_t capacity ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex2 );

      // Remember the size:
      m_size = size;

      // Check if we want to write this variable to the output:
      if( ( ! m_outTree ) || ( ! isAuxIDSelected( auxid ) ) ) {
         // Create the store only when necessary:
         if( ! m_transientStore ) {
            m_transientStore =
               new SG::AuxStoreInternal( m_structMode == kObjectStore );
            if( m_locked ) {
               m_transientStore->lock();
            }
         }
         // Let the transient store create the variable:
         size_t nids = m_transientStore->getAuxIDs().size();
         void* result = m_transientStore->getData( auxid, size, capacity );
         if( result && ( nids != m_transientStore->getAuxIDs().size() ) ) {
            m_auxIDs.insert( auxid );
         }
         // Return the address in the transient memory:
         return result;
      }

      // If the variable exists already, and this is a locked store, then
      // we are in trouble.
      if( m_locked && ( auxid < m_vecs.size() ) && m_vecs[ auxid ] ) {
         if( ! ( ( auxid < m_isDecoration.size() ) &&
                 m_isDecoration[ auxid ] ) ) {
            throw SG::ExcStoreLocked( auxid );
         }
      }

      // Connect this auxiliary variable just to the output:
      if( setupOutputData( auxid ).isFailure() ) {
         ::Error( "xAOD::TAuxStore::getData",
                  XAOD_MESSAGE( "Failed to set up variable %s" ),
                  SG::AuxTypeRegistry::instance().getName( auxid ).c_str() );
         return 0;
      }

      // Check whether things make sense:
      if( ( m_structMode == kObjectStore ) && ( size != 1 ) ) {
         ::Error( "xAOD::TAuxStore::getData",
                  XAOD_MESSAGE( "Branch creation requested with:" ) );
         ::Error( "xAOD::TAuxStore::getData",
                  XAOD_MESSAGE( "  name = %s" ),
                  SG::AuxTypeRegistry::instance().getName( auxid ).c_str() );
         ::Error( "xAOD::TAuxStore::getData",
                  XAOD_MESSAGE( "  size = %i" ),
                  static_cast< int >( size ) );
         ::Error( "xAOD::TAuxStore::getData",
                  XAOD_MESSAGE( "  m_structMode = kObjectStore" ) );
         return 0;
      }

      // Make sure the variable is of the right size:
      m_vecs[ auxid ]->reserve( capacity );
      m_vecs[ auxid ]->resize( size );

      // Return the object:
      return m_vecs[ auxid ]->toPtr();
   }

   const TAuxStore::auxid_set_t& TAuxStore::getWritableAuxIDs() const {

      return getAuxIDs();
   }

   bool TAuxStore::resize( size_t size ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // A sanity check:
      if( ( m_structMode == kObjectStore ) && ( size != 1 ) ) {
         ::Error( "xAOD::TAuxStore::resize",
                  XAOD_MESSAGE( "size = %i for single-object store" ),
                  static_cast< int >( size ) );
         return false;
      }

      // Remember the new size:
      m_size = size;

      bool nomoves = true;
      for (SG::IAuxTypeVector* v : m_vecs) {
         if(v) {
           if (!v->resize( size ))
             nomoves = false;
         }
      }
      if( m_transientStore ) {
        if (!m_transientStore->resize( size ))
          nomoves = false;
      }

      return nomoves;
   }

   void TAuxStore::reserve( size_t size ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // A sanity check:
      if( ( m_structMode == kObjectStore ) && ( size != 1 ) ) {
         ::Error( "xAOD::TAuxStore::reserve",
                  XAOD_MESSAGE( "size = %i for single-object store" ),
                  static_cast< int >( size ) );
         return;
      }

      std::vector< SG::IAuxTypeVector* >::iterator itr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator end = m_vecs.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) ( *itr )->reserve( size );
      }
      if( m_transientStore ) {
         m_transientStore->reserve( size );
      }

      return;
   }

   void TAuxStore::shift( size_t pos, ptrdiff_t offs ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // A sanity check:
      if( m_structMode == kObjectStore ) {
         ::Error( "xAOD::TAuxStore::shift",
                  XAOD_MESSAGE( "Should not have been called for single-object "
                                "store" ) );
         return;
      }

      // Adjust the size of the container:
      if( ( static_cast< std::size_t >( std::abs( offs ) ) > m_size ) &&
          ( offs < 0 ) ) {
         m_size = 0;
      } else {
         m_size += offs;
      }

      std::vector< SG::IAuxTypeVector* >::iterator itr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator end = m_vecs.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) ( *itr )->shift( pos, offs );
      }
      if( m_transientStore ) {
         m_transientStore->shift( pos, offs );
      }

      return;
   }

   bool TAuxStore::insertMove (size_t pos,
                               IAuxStore& other,
                               const SG::auxid_set_t& ignore_in)
   {
      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // A sanity check:
      if( m_structMode == kObjectStore ) {
         ::Error( "xAOD::TAuxStore::insertMove",
                  XAOD_MESSAGE( "Should not have been called for single-object "
                                "store" ) );
         return false;
      }

      const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
      bool nomove = true;
      size_t other_size = other.size();

      SG::auxid_set_t ignore = ignore_in;

      for (SG::auxid_t id : m_auxIDs) {
        SG::IAuxTypeVector* v_dst = nullptr;
        if (id < m_vecs.size())
          v_dst = m_vecs[id];
        if (v_dst) {
          ignore.insert (id);
          if (other.getData (id)) {
            void* src_ptr = other.getData (id, other_size, other_size);
            if (src_ptr) {
              if (!v_dst->insertMove (pos, src_ptr,
                                      reinterpret_cast<char*>(src_ptr) + other_size*r.getEltSize(id)))
                nomove = false;
            }
          }
          else {
            const void* orig = v_dst->toPtr();
            v_dst->shift (pos, other_size);
            if (orig != v_dst->toPtr())
              nomove = false;
          }
        }
      }

      if( m_transientStore ) {
        if (!m_transientStore->insertMove( pos, other, ignore ))
          nomove = false;
      }

      return nomove;
   }


   const void* TAuxStore::getIOData( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // If the variable is connected to already:
      if( ( auxid < m_branches.size() ) && m_branches[ auxid ] ) {
         // Make sure that the right payload is in memory:
         m_branches[ auxid ]->getEntry( m_entry );
         // Return the pointer:
         return m_branches[ auxid ]->objectPtr();
      }

      // Check if it's in the transient store:
      if( m_transientStore && m_transientStore->getAuxIDs().test( auxid ) ) {
         return m_transientStore->getIOData( auxid );
      }

      // If not, try connecting to it now:
      if( ! setupInputData( auxid ).isSuccess() ) {
         // This is not actually an error condition anymore. We can end up here
         // when we decorate constant objects coming from the input file, but
         // on one event we can't set any decorations. For instance when the
         // input container is empty. In that case the object will still list
         // the auxiliary ID belonging to that decoration as being available,
         // but it really isn't.
         //
         // Later on it might be needed to tweak the logic of all of this, but
         // for now just silently returning 0 seems to do the right thing.
         return 0;
      }

      // Now we should know this variable:
      if( ( auxid >= m_branches.size() ) || ( ! m_branches[ auxid ] ) ) {
         ::Fatal( "xAOD::TAuxStore::getIOData",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return 0;
      }

      // Make sure that the right payload is in memory:
      m_branches[ auxid ]->getEntry( m_entry );

      // Return the pointer:
      return m_branches[ auxid ]->objectPtr();
   }

   const std::type_info* TAuxStore::getIOType( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // If the variable is connected to already:
      if( ( auxid < m_branches.size() ) && m_branches[ auxid ] ) {
         return m_branches[ auxid ]->typeInfo();
      }

      // Check if it's in the transient store:
      if( m_transientStore && m_transientStore->getAuxIDs().test( auxid ) ) {
         return m_transientStore->getIOType( auxid );
      }

      // If not, try connecting to it now:
      if( ! setupInputData( auxid ).isSuccess() ) {
         ::Error( "xAOD::TAuxStore::getIOType",
                  XAOD_MESSAGE( "Couldn't connect to auxiliary variable "
                                "%i %s" ),
                  static_cast< int >( auxid ),
                  SG::AuxTypeRegistry::instance().getName( auxid ).c_str() );
         return 0;
      }

      // Now we should know this variable:
      if( ( auxid >= m_branches.size() ) || ( ! m_branches[ auxid ] ) ) {
         ::Fatal( "xAOD::TAuxStore::getIOType",
                  XAOD_MESSAGE( "Internal logic error detected" ) );
         return 0;
      }

      // Return the type info:
      return m_branches[ auxid ]->typeInfo();
   }

   const TAuxStore::auxid_set_t& TAuxStore::getDynamicAuxIDs() const {

      // All the auxiliary decorations handled by this object are considered
      // dynamic:
      return getAuxIDs();
   }

   void TAuxStore::selectAux( const std::set< std::string >& attributes ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      m_selection.selectAux( attributes );
      return;
   }

   TAuxStore::auxid_set_t TAuxStore::getSelectedAuxIDs() const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex1 );

      // Leave the calculation up to the internal object:
      return m_selection.getSelectedAuxIDs( m_auxIDs );
   }

   /// This function is used to initialise the statistics registry with
   /// information about the branches managed by this object. It is used by
   /// TEvent when connecting to new input files/chains.
   ///
   /// @param tree The tree to collect the information from
   /// @returns <code>TReturnCode::kSuccess</code> if the function was
   ///          successful, something else otherwise
   ///
   TReturnCode TAuxStore::initStats( ::TTree* tree ) {

      // Connect the object to this input tree:
      RETURN_CHECK( "initStats", readFrom( tree, kFALSE ) );

      // Conveninence variable:
      ReadStats& stats = IOStats::instance().stats();

      // Teach the cache about all the branches:
      size_t nbranch = 0;
      for (SG::auxid_t id : m_auxIDs) {
         stats.branch( m_prefix, id );
         ++nbranch;
      }

      // Increment the number of known branches:
      stats.setBranchNum( stats.branchNum() + nbranch );

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This internal function takes care of connecting to an individual
   /// (sub-)branch in the input file for a given auxiliary variable. It
   /// needs to handle a number of different use cases, so it's a bit long.
   ///
   /// @param auxid The ID of the variable to connect to
   /// @returns <code>kTRUE</code> if the operation was successful,
   ///          <code>kFALSE</code> if not
   ///
   TReturnCode TAuxStore::setupInputData( auxid_t auxid ) const {

      // Return right away if we already know that the branch is missing:
      if( ( auxid < m_missingBranches.size() ) && m_missingBranches[ auxid ] ) {
         return TReturnCode::kRecoverable;
      }

      // Make sure the internal storage is large enough:
      if( m_vecs.size() <= auxid ) {
         m_vecs.resize( auxid + 1 );
      }
      if( m_branches.size() <= auxid ) {
         m_branches.resize( auxid + 1 );
      }

      // Check if we need to do anything:
      if( m_vecs[ auxid ] && m_branches[ auxid ] ) {
         return TReturnCode::kSuccess;
      }

      // A little sanity check:
      if( ! m_inTree ) {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "No input TTree set up!" ) );
         return TReturnCode::kFailure;
      }

      // Another sanity check:
      if( m_vecs[ auxid ] || m_branches[ auxid ] ) {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "The internal variables of the object got "
                                "messed up?!?" ) );
         return TReturnCode::kFailure;
      }

      // Get the property name:
      const TString statBrName = m_prefix +
         SG::AuxTypeRegistry::instance().getName( auxid ).c_str();
      const TString dynBrName = m_dynPrefix +
         SG::AuxTypeRegistry::instance().getName( auxid ).c_str();

      // Check if the branch exists:
      Bool_t staticBranch = kTRUE;
      TString brName = statBrName;
      ::TBranch* br = m_inTree->GetBranch( statBrName );
      if( ! br ) {
         br = m_inTree->GetBranch( dynBrName );
         if( ! br ) {
            // Since TTree::GetBranch is expensive, remember that we didn't
            // find this branch in this file.
            if( m_missingBranches.size() <= auxid ) {
               m_missingBranches.resize( auxid + 1 );
            }
            m_missingBranches[ auxid ] = true;
            // The branch doesn't exist, but this is not an error per se.
            // The user may just be calling isAvailable(...) on the variable.
            return TReturnCode::kRecoverable;
         }
         // We have a dynamic branch:
         staticBranch = kFALSE;
         brName = dynBrName;
      }

      // Check if it's a "primitive branch":
      const Bool_t primitiveBranch = isPrimitiveBranch( br );
      // Check if it's a "container branch":
      const Bool_t containerBranch =
         ( primitiveBranch ? kFALSE : isContainerBranch( br, auxid ) );

      // Set the structure mode if it has not been defined externally:
      if( m_structMode == kUndefinedStore ) {
         m_structMode = ( containerBranch ? kContainerStore : kObjectStore );
      }

      // Check that the branch type makes sense:
      if( ( containerBranch && ( m_structMode != kContainerStore ) ) ||
          ( ( ! containerBranch ) && ( m_structMode != kObjectStore ) ) ) {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "Branch type and requested structure mode "
                                "differ for branch: %s" ),
                  brName.Data() );
         return TReturnCode::kFailure;
      }

      // Check what variable it is:
      ::TClass* clDummy = 0;
      ::EDataType dType = kOther_t;
      if( br->GetExpectedType( clDummy, dType ) ) {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "Couldn't determine the type of branch "
                                "\"%s\"" ), brName.Data() );
         return TReturnCode::kFailure;
      }

      // Get the property type:
      const std::type_info* brType = 0;
      if( isRegisteredType( auxid ) ) {
         // Get the type from the auxiliary type registry:
         brType =
            ( containerBranch ?
              SG::AuxTypeRegistry::instance().getVecType( auxid ) :
              SG::AuxTypeRegistry::instance().getType( auxid ) );
      } else {
         // Get the type from the input branch itself:
         brType = ( clDummy ? clDummy->GetTypeInfo() :
                    &( Utils::getTypeInfo( dType ) ) );
      }
      if( ! brType ) {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "Can't read/copy variable %s (%s)" ),
                  brName.Data(), clDummy->GetName() );
         return TReturnCode::kRecoverable;
      }
      const TString brTypeName = Utils::getTypeName( *brType ).c_str();

      // Check if we have the needed dictionary for an object branch:
      ::TClass* brClass = 0;
      if( ! primitiveBranch ) {
         // Get the property's class:
         brClass = ::TClass::GetClass( *brType, kTRUE, kTRUE );
         if( ! brClass ) {
            brClass = ::TClass::GetClass( brTypeName );
         }
         if( ! brClass ) {
            ::Error( "xAOD::TAuxStore::setupInputData",
                     XAOD_MESSAGE( "No dictionary available for class \"%s\"" ),
                     brTypeName.Data() );
            return TReturnCode::kFailure;
         }
      }

      // Create the smart object holding this vector:
      if( isRegisteredType( auxid ) ) {
         m_vecs[ auxid ] =
            SG::AuxTypeRegistry::instance().makeVector( auxid, (size_t)0, (size_t)0 ).release();
         if( ! containerBranch ) {
            m_vecs[ auxid ]->resize( 1 );
         }
         if (clDummy && strncmp (clDummy->GetName(), "SG::PackedContainer<", 20) == 0) {
           SG::IAuxTypeVector* packed = m_vecs[ auxid ]->toPacked().release();
           std::swap (m_vecs[ auxid ], packed);
           delete packed;
         }
      } else {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "Couldn't create in-memory vector for "
                                "variable %s (%i)" ),
                  brName.Data(),
                  static_cast< int >( auxid ) );
         return TReturnCode::kFailure;
      }

      // Create a new branch handle:
      const std::type_info* objType = brType;
      if (containerBranch) {
        objType = m_vecs[ auxid ]->objType();
        if (!objType)
          objType = SG::AuxTypeRegistry::instance().getType( auxid );
      }
      m_branches[ auxid ] = new TBranchHandle( staticBranch, primitiveBranch,
                                               objType,
                                               ( containerBranch ?
                                                 m_vecs[ auxid ]->toVector() :
                                                 m_vecs[ auxid ]->toPtr() ),
                                               auxid, &m_prefix );

      // Set the tree in the "right mode":
      m_inTree->SetMakeClass( staticBranch );

      // Connect to the branch:
      ::Int_t status = 0;
      if( clDummy &&
          ::TString( clDummy->GetName() ).Contains( "basic_string<char>" ) ) {
         // This is pretty much just a hack. As it happens, Athena I/O can
         // create dynamic branches that consider themselves to be of type
         // "vector<basic_string<char> >" and similar. (Instead of the
         // canonical "vector<string>" name.) When we encounter such a branch,
         // we just connect to it without performing any compatibility checks.
         // Since we don't need to apply any read rules in this case anyway.
         status =
            m_inTree->SetBranchAddress( brName,
                                        m_branches[ auxid ]->inputObjectPtr(),
                                        m_branches[ auxid ]->branchPtr() );
      } else {
         status =
            m_inTree->SetBranchAddress( brName,
                                        m_branches[ auxid ]->inputObjectPtr(),
                                        m_branches[ auxid ]->branchPtr(),
                                        brClass, dType,
                                        ( ( ! staticBranch ) &&
                                          ( ! primitiveBranch ) ) );
      }
      if( status < 0 ) {
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "Coulnd't connect to branch \"%s\"" ),
                  brName.Data() );
         ::Error( "xAOD::TAuxStore::setupInputData",
                  XAOD_MESSAGE( "Return code: %i" ), status );
         delete m_vecs[ auxid ];
         m_vecs[ auxid ] = 0;
         delete m_branches[ auxid ];
         m_branches[ auxid ] = 0;
         return TReturnCode::kFailure;
      }

      // Get the current entry:
      m_branches[ auxid ]->getEntry( m_entry );

      // Remember which variable got created:
      m_auxIDs.insert( auxid );

      // Check if we just replaced a generic object:
      if( isRegisteredType( auxid ) ) {
         // The name of the variable we just created:
         const std::string auxname =
            SG::AuxTypeRegistry::instance().getName( auxid );
         // Check if there's another variable with this name already:
         for( auxid_t i = 0; i < m_vecs.size(); ++i ) {
            // Check if we have this aux ID:
            if( ! m_vecs[ i ] ) continue;
            // Ingore the object that we *just* created:
            if( i == auxid ) continue;
            // The name of the variable:
            const std::string testname =
               SG::AuxTypeRegistry::instance().getName( i );
            // Check if it has the same name:
            if( testname != auxname ) continue;
            // Check that the other one is a non-registered type:
            if( isRegisteredType( i ) ) {
               ::Error( "xAOD::TAuxStore::setupInputData",
                        XAOD_MESSAGE( "Internal logic error!" ) );
               continue;
            }
            // Okay, we do need to remove this object:
            delete m_vecs[ i ]; delete m_branches[ i ];
            m_vecs[ i ] = 0; m_branches[ i ] = 0;
            m_auxIDs.erase( i );
         }
      }

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function is used internally to create a "simple" output branch
   /// with the contents of a single auxiliary variable.
   ///
   /// @param auxid The ID of the variable to create an output branch for
   /// @returns <code>kTRUE</code> if the operation was successful,
   ///          <code>kFALSE</code> if not
   ///
   TReturnCode TAuxStore::setupOutputData( auxid_t auxid ) const {

      // Check whether we need to do anything:
      if( ! m_outTree ) return TReturnCode::kSuccess;

      // Check if the variable needs to be written out:
      if( ! isAuxIDSelected( auxid ) ) return TReturnCode::kSuccess;

      // Make sure that containers are large enough:
      if( m_vecs.size() <= auxid ) {
         m_vecs.resize( auxid + 1 );
      }
      if( m_branches.size() <= auxid ) {
         m_branches.resize( auxid + 1 );
      }
      if( m_branchesWritten.size() <= auxid ) {
         m_branchesWritten.resize( auxid + 1 );
      }

      // Check if this auxiliary variable is already in the output:
      if( m_branchesWritten[ auxid ] ) return TReturnCode::kSuccess;

      // Check if the variable was put into the transient store as a
      // decoration, and now needs to be put into the output file:
      if( ( ! m_vecs[ auxid ] ) && m_transientStore &&
          ( m_transientStore->getAuxIDs().test( auxid ) ) ) {

         // Get the variable from the transient store:
         const void* pptr = m_transientStore->getData( auxid );
         if( ! pptr ) {
            ::Fatal( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "Internal logic error detected" ) );
            return TReturnCode::kFailure;
         }

         // The registry:
         SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();

         // Create the new object:
         m_vecs[ auxid ] = reg.makeVector( auxid, m_size, m_size ).release();
         void* ptr = m_vecs[ auxid ]->toPtr();
         if( ! ptr ) {
            ::Error( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "Couldn't create decoration in memory "
                                   "for writing" ) );
            return TReturnCode::kFailure;
         }

         // Get the type of this variable:
         const std::type_info* type = reg.getType( auxid );
         if( ! type ) {
            ::Error( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "Couldn't get the type of transient "
                                   "variable %i" ),
                     static_cast< int >( auxid ) );
            return TReturnCode::kFailure;
         }
         // Now get the factory for this variable:
         const SG::IAuxTypeVectorFactory* factory =
            reg.getFactory( *type );
         if( ! factory ) {
            ::Error( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "No factory found for transient variable "
                                   "%i" ), static_cast< int >( auxid ) );
            return TReturnCode::kFailure;
         }

         // Finally, do the copy:
         for( std::size_t i = 0; i < m_size; ++i ) {
            factory->copy( ptr, i, pptr, i );
         }

         // And now remember that this is a decoration:
         if( m_isDecoration.size() <= auxid ) {
            m_isDecoration.resize( auxid + 1 );
         }
         m_isDecoration[ auxid ] = ::kTRUE;
      }

      // Check if we know about this variable to be on the input,
      // but haven't connected to it yet:
      if( ( m_auxIDs.test( auxid ) ) &&
          ( ! m_vecs[ auxid ] ) && ( ! m_branches[ auxid ] ) ) {
         RETURN_CHECK( "xAOD::TAuxStore::setupOutputData",
                       setupInputData( auxid ) );
      }

      // Check that we know the store's type:
      if( ( m_structMode != kContainerStore ) &&
          ( m_structMode != kObjectStore ) ) {
         ::Error( "xAOD::TAuxStore::setupOutputData",
                  XAOD_MESSAGE( "Structure mode unknown for variable %s" ),
                  SG::AuxTypeRegistry::instance().getName( auxid ).c_str() );
         return TReturnCode::kFailure;
      }

      // Check if the variable exists already in memory:
      if( ! m_vecs[ auxid ] ) {
         m_vecs[ auxid ] =
            SG::AuxTypeRegistry::instance().makeVector( auxid, (size_t)0, (size_t)0 ).release();
         if( m_structMode == kObjectStore ) {
            m_vecs[ auxid ]->resize( 1 );
         }
      }

      // Check if the branch handle exists already:
      if( ! m_branches[ auxid ] ) {
         // Get the property type:
         const std::type_info* brType =
            ( m_structMode == kContainerStore ?
              SG::AuxTypeRegistry::instance().getVecType( auxid ) :
              SG::AuxTypeRegistry::instance().getType( auxid ) );
         // Create the handle object:
         bool primitiveBranch = (strlen( brType->name() ) == 1);
         m_branches[ auxid ] =
            new TBranchHandle( kFALSE, ( strlen( brType->name() ) == 1 ),
                               (primitiveBranch ? 
                                brType :
                                m_vecs[ auxid ]->objType() ),
                               ( m_structMode == kObjectStore ?
                                 m_vecs[ auxid ]->toPtr() :
                                 m_vecs[ auxid ]->toVector() ),
                               auxid, &m_prefix );
      }

      // Construct a name for the branch:
      const TString brName =
         m_dynPrefix + SG::AuxTypeRegistry::instance().getName( auxid );

      // If the output branch exists already, assume that it was us making
      // it:
      ::TBranch* br = m_outTree->GetBranch( brName );
      if( br ) {
         // Apparently a branch that was already set up for copying as a basic
         // variable, now got accessed explicitly. So let's update the output
         // branch to point to this new location now.
         br->SetAddress( m_branches[ auxid ]->outputObjectPtr() );
         // Update the cache. Notice that the "write status" of the typeless
         // auxiliary ID is not turned off. But it shouldn't matter, as the
         // variable will not be accessed in a typeless way anymore.
         m_branchesWritten[ auxid ] = true;
         // Return gracefully:
         return TReturnCode::kSuccess;
      }

      // Check that we know the type of the branch:
      const std::type_info* brType = m_branches[ auxid ]->typeInfo();
      if( ! brType ) {
         ::Error( "xAOD::TAuxStore::setupOutputData",
                  XAOD_MESSAGE( "There's an internal logic error in the "
                                "code" ) );
         return TReturnCode::kFailure;
      }
      const std::string brTypeName = Utils::getTypeName( *brType );

      // Decide if this is a primitive branch:
      const Bool_t primitiveBranch = ( strlen( brType->name() ) == 1 );

      // Let's create the branch now:
      if( primitiveBranch ) {

         // Get the "ROOT type" belonging to this primitive:
         const char rootType = Utils::rootType( brType->name()[ 0 ] );
         if( rootType == '\0' ) {
            ::Error( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "Type not known for variable \"%s\" "
                                   "of type \"%s\"" ),
                     brName.Data(), brTypeName.c_str() );
            return TReturnCode::kFailure;
         }

         // Construct the type description:
         std::ostringstream typeDesc;
         typeDesc << brName << "/" << rootType;

         // Create the branch:
         br = m_outTree->Branch( brName,
                                 m_branches[ auxid ]->outputObjectPtr(),
                                 typeDesc.str().c_str(), m_basketSize );

      } else {

         // Access the dictionary for the type:
         TClass* cl = TClass::GetClass( *brType );
         if( ! cl ) {
            cl = TClass::GetClass( brTypeName.c_str() );
         }
         if( ! cl ) {
            ::Error( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "Couldn't find dictionary for type: %s" ),
                     brTypeName.c_str() );
            return TReturnCode::kFailure;
         }
         if( ! cl->GetStreamerInfo() ) {
            ::Error( "xAOD::TAuxStore::setupOutputData",
                     XAOD_MESSAGE( "No streamer info available for type %s" ),
                     cl->GetName() );
            return TReturnCode::kFailure;
         }

         // Create the branch:
         br = m_outTree->Branch( brName, cl->GetName(),
                                 m_branches[ auxid ]->outputObjectPtr(),
                                 m_basketSize, m_splitLevel );
      }

      // Check if we succeeded:
      if( ! br ) {
         ::Error( "xAOD::TAuxStore::setupOutputData",
                  XAOD_MESSAGE( "Failed creating branch \"%s\" of type "
                                "\"%s\"" ),
                  brName.Data(), brTypeName.c_str() );
         return TReturnCode::kFailure;
      }

      // If this is not the first event, fill up the branch with dummy
      // info:
      for( Long64_t i = 0; i < m_outTree->GetEntries(); ++i ) {
         br->Fill();
      }

      // Update the cache:
      m_branchesWritten[ auxid ] = true;

      // Also, remember that we now handle this variable:
      m_auxIDs.insert( auxid );

      // We were successful:
      return TReturnCode::kSuccess;
   }

   /// When writing an output tree, while reading information from an input
   /// tree, it can often happen that we want to copy the contents of some
   /// variables that we don't actually need during the event processing of
   /// this particular job. Since the user doesn't ask for all the possible
   /// input variables, this function needs to look at the input TTree, and
   /// try to figure out which of the branches in the tree belong to this
   /// object.
   ///
   /// The function creates a "proper" or "virtual" auxiliary ID for each of
   /// the branches found, so they can be referenced in the "usual way" in
   /// the other parts of the code later on.
   ///
   /// @returns <code>kTRUE</code> if the operation was successful,
   ///          <code>kFALSE</code> if not
   ///
   TReturnCode TAuxStore::scanInputTree() {

      // Check if an input tree is even available:
      if( ! m_inTree ) {
         // It's not an error if it isn't.
         return TReturnCode::kSuccess;
      }

      // Check if the input was already scanned:
      if( m_inputScanned ) {
         return TReturnCode::kSuccess;
      }

      // Get a list of all branches in the tree:
      TObjArray* branches = m_inTree->GetListOfBranches();

      // Check each of them:
      for( Int_t i = 0; i < branches->GetEntriesFast(); ++i ) {

         // The name of this top-level branch:
         const TString brName = branches->At( i )->GetName();

         // Access the branch pointer:
         TBranch* br = dynamic_cast< TBranch* >( branches->At( i ) );
         if( ! br ) {
            ::Fatal( "xAOD::TAuxStore::scanInputTree",
                     XAOD_MESSAGE( "Logic error detected" ) );
         }

         // For top-level stores let's scan the static branches as well:
         if( m_topStore && ( brName == m_prefix ) ) {

            // Get a list of its sub-branches:
            TObjArray* sbranches = br->GetListOfBranches();

            // ...and then loop over them:
            for( Int_t j = 0; j < sbranches->GetEntriesFast(); ++j ) {

               // The name of the sub-branch:
               const TString brName = sbranches->At( j )->GetName();

               // Try to make a variable name out of the branch name:
               const TString auxName = brName( brName.Index( "." ) + 1,
                                               brName.Length() );

               // Skip this entry if it refers to a base class:
               if( auxName.BeginsWith( "xAOD::" ) ||
                   auxName.BeginsWith( "SG::" ) ||
                   ( auxName == "ILockable" ) ) {
                  continue;
               }

               // The sub-branch:
               ::TBranch* sbr =
                    dynamic_cast< ::TBranch* >( sbranches->At( j ) );
               if( ! sbr ) {
                  ::Fatal( "xAOD::TAuxStore::scanInputTree",
                           XAOD_MESSAGE( "Logic error detected" ) );
               }

               // Leave the rest up to the function that is shared with the
               // dynamic branches:
               RETURN_CHECK( "xAOD::TAuxStore::scanInputTree",
                             setupAuxBranch( sbr, auxName, kTRUE ) );
            }

            // Don't check the rest of the loop's body:
            continue;
         }

         // Check if it has the right prefix to be a dynamic variable:
         if( ! brName.BeginsWith( m_dynPrefix ) ) {
            continue;
         }
         // It's possible to create dynamic variables with an empty name
         // as well. Which is a bug. Such variables are just ignored
         // for now.
         if( brName == m_dynPrefix ) {
            ::Warning( "xAOD::TAuxStore::scanInputTree",
                       "Dynamic branch with empty name found on container: %s",
                       m_prefix.c_str() );
            continue;
         }

         // The auxiliary property name:
         const TString auxName = brName( brName.Index( "." ) + 1,
                                         brName.Length() );

         // Leave the rest up to the function that is shared with the
         // dynamic branches:
         RETURN_CHECK( "xAOD::TAuxStore::scanInputTree",
                       setupAuxBranch( br, auxName, kFALSE ) );
      }

      // Okay, the input was successfully scanned:
      m_inputScanned = kTRUE;

      // Return gracefully:
      return TReturnCode::kSuccess;
   }

   /// This function takes care of assigning an auxiliary ID to a given branch.
   /// It tries its best to find an auxiliary vector factory for the branch's
   /// type, but if it fails, it still falls back to using
   /// <code>SG::AuxTypePlaceholder</code> as the type. In which case of course
   /// only dumb copying will be possible for the given branch. (And no vector
   /// operations on the branch's payload until the variable of the branch is
   /// accessed explicitly.)
   ///
   /// @param br Pointer to the branch under investigation
   /// @param auxName The name of the auxiliary property, extracted from the
   ///                branch's name
   /// @param staticBranch <code>kTRUE</code> if this is a "static branch", and
   ///                     <code>kFALSE</code> if it's a dynamic one
   /// @returns <code>kTRUE</code> if successful, <code>kFALSE</code> if not
   ///
   TReturnCode TAuxStore::setupAuxBranch( ::TBranch* br, const char* auxName,
                                          ::Bool_t staticBranch ) {

      // Get the branch's type:
      ::TClass* expectedClass = 0;
      ::EDataType expectedType = kOther_t;
      if( br->GetExpectedType( expectedClass, expectedType ) &&
          ( ( ! staticBranch ) || ( strncmp( auxName, "m_", 2 ) != 0 ) ) ) {
         ::Warning( "xAOD::TAuxStore::setupAuxBranch",
                    "Couldn't get the type of branch \"%s\"",
                    br->GetName() );
      }

      // If this is a primitive variable, and we're still not sure whether this
      // is a store for an object or a container, the answer is given...
      if( ( ! expectedClass ) &&
          ( m_structMode == kUndefinedStore ) ) {
         m_structMode = kObjectStore;
      }

      // Get the type_info of the branch:
      const std::type_info* ti = 0;
      if( m_structMode == kObjectStore ) {
         if( expectedClass ) {
            ti = expectedClass->GetTypeInfo();
         } else {
            ti = &( Utils::getTypeInfo( expectedType ) );
         }
      } else {
         if( ! expectedClass ) {
            if( ( ! staticBranch ) || ( strncmp( auxName, "m_", 2 ) != 0 ) ) {
               ::Warning( "xAOD::TAuxStore::setupAuxBranch",
                          "Couldn't get the type of branch \"%s\"",
                          br->GetName() );
            }
         } else {
            ::TVirtualCollectionProxy* prox =
               expectedClass->GetCollectionProxy();

           if (!prox) {
             TClass* cl2 = lookupVectorType (expectedClass);
             if (cl2)
               prox = cl2->GetCollectionProxy();
           }

           if( ! prox ) {
               if( ( ! staticBranch ) ||
                   ( strncmp( auxName, "m_", 2 ) != 0 ) ) {
                  ::Warning( "xAOD::TAuxStore::setupAuxBranch",
                             "Couldn't get the type of branch \"%s\"",
                             br->GetName() );
               }
            } else {
               if( prox->GetValueClass() ) {
                  ti = prox->GetValueClass()->GetTypeInfo();
               } else {
                  ti = &( Utils::getTypeInfo( prox->GetType() ) );
               }
            }
         }
      }

      // Get the registry:
      SG::AuxTypeRegistry& registry = SG::AuxTypeRegistry::instance();

      // Check if the registry already knows this variable name. If yes, let's
      // use the type known by the registry. To be able to deal with simple
      // schema evolution in dynamic branches.
      const auxid_t regAuxid = registry.findAuxID( auxName );
      if( regAuxid != SG::null_auxid ) {
         m_auxIDs.insert( regAuxid );
         return TReturnCode::kSuccess;
      }

      // If we didn't find a type_info for the branch, give up now...
      if( ! ti ) {
         return TReturnCode::kSuccess;
      }

      // Check for an auxiliary ID for this branch:
      auxid_t auxid = registry.getAuxID( *ti, auxName );

      // First try to find a compiled factory for the vector type:
      if( auxid == SG::null_auxid ) {

         // Construct the name of the factory's class:
         std::string fac_class_name = "SG::AuxTypeVectorFactory<" +
            Utils::getTypeName( *ti );
         if( fac_class_name[ fac_class_name.size() - 1 ] == '>' ) {
            fac_class_name += ' ';
         }
         fac_class_name += '>';

         // Look for the dictionary of this type:
         ::TClass* fac_class = TClass::GetClass( fac_class_name.c_str() );
         if( fac_class && fac_class->IsLoaded() ) {
            ::TClass* base_class =
               ::TClass::GetClass( "SG::IAuxTypeVectorFactory" );
            if( base_class && base_class->IsLoaded() ) {
               const Int_t offs = fac_class->GetBaseClassOffset( base_class );
               if( offs >= 0 ) {
                  void* fac_vp = fac_class->New();
                  if( fac_vp ) {
                     unsigned long tmp =
                        reinterpret_cast< unsigned long >( fac_vp ) + offs;
                     SG::IAuxTypeVectorFactory* fac =
                        reinterpret_cast< SG::IAuxTypeVectorFactory* >( tmp );
                     registry.addFactory( *ti, fac );
                     auxid = registry.getAuxID( *ti, auxName );
                  }
               }
            }
         }
      }

      // If that didn't succeed, let's assign a generic factory to this type:
      if( auxid == SG::null_auxid ) {

         // Construct the name of the vector type:
         std::string vec_class_name = "std::vector<" +
            Utils::getTypeName( *ti );
         if( vec_class_name[ vec_class_name.size() - 1 ] == '>' ) {
            vec_class_name += ' ';
         }
         vec_class_name += '>';

         // Get the dictionary for the type:
         ::TClass* vec_class = ::TClass::GetClass( vec_class_name.c_str() );
         if( vec_class && vec_class->IsLoaded() ) {
            SG::IAuxTypeVectorFactory* fac = new TAuxVectorFactory( vec_class );
            registry.addFactory( *ti, fac );
            auxid = registry.getAuxID( *ti, auxName );
         } else {
            ::Warning( "xAOD::TAuxStore::setupAuxBranch",
                       "Couldn't find dictionary for type: %s",
                       vec_class_name.c_str() );
         }
      }

      // Check if we succeeded:
      if( auxid == SG::null_auxid ) {
         ::Error( "xAOD::TAuxStore::setupAuxBranch",
                  XAOD_MESSAGE( "Couldn't assign auxiliary ID to branch "
                                "\"%s\"" ),
                  br->GetName() );
         return TReturnCode::kFailure;
      }

      // Remember the auxiliary ID:
      m_auxIDs.insert( auxid );
      return TReturnCode::kSuccess;
   }

   /// This is a tricky one. The function can't just rely on getSelectedAuxIDs,
   /// as the aux ID received here may be a new ID that the object doesn't yet
   /// know about. So we have no other choice but to check this ID explicitly.
   ///
   /// @apram auxid The auxiliary ID that should be checked
   /// @returns <code>kTRUE</code> if the variable needs to be written out,
   ///          <code>kFALSE</code> if not
   ///
   ::Bool_t TAuxStore::isAuxIDSelected( auxid_t auxid ) const {

      // A temporary object:
      auxid_set_t auxids; auxids.insert( auxid );

      // Check if the auxid is returned as a selected ID:
      return m_selection.getSelectedAuxIDs( auxids ).size();
   }

   /// The code needs to be fairly smart to figure out whether a branch is a
   /// primitive one, or an std::vector<T> one. The check is done in this
   /// function.
   ///
   /// @param br The branch that should be checked
   /// @returns <code>kTRUE</code> if the branch describes a primitive variable,
   ///          <code>kFALSE</code> otherwise
   ///
   ::Bool_t TAuxStore::isPrimitiveBranch( ::TBranch* br ) {

      // The variables needed for the check:
      ::TClass* cl = 0;
      ::EDataType dType = kOther_t;

      // Get the variable type from the branch:
      if( br->GetExpectedType( cl, dType ) ) {
         ::Error( "xAOD::TAuxStore::isPrimitiveBranch",
                  XAOD_MESSAGE( "Couldn't determine the type of branch "
                                "\"%s\"" ), br->GetName() );
         return kFALSE;
      }

      // The check is made using the data type variable:
      return ( ( dType != kOther_t ) && ( dType != kNoType_t ) &&
               ( dType != kVoid_t ) );
   }

   /// The code needs to know if a given branch describes a single object, or
   /// a container of objects. This function decides about this.
   ///
   /// In case the function encounters a problem it returns
   /// <code>kFALSE</code>, as that is more likely to cause a crash a little
   /// later...
   ///
   /// The function should probably be a bit optimisied in the future, as it
   /// makes a lot of security checks at the moment.
   ///
   /// @param br    The branch whose type we're interested in
   /// @param auxid The auxiliary ID of the variable described by the branch
   /// @returns <code>kTRUE</code> if the branch describes a container,
   ///          <code>kFALSE</code> if it describes a single object
   ///
   ::Bool_t TAuxStore::isContainerBranch( ::TBranch* br, auxid_t auxid ) {

      // For unknown types it doesn't matter if the branch describes a
      // container or a single element.
      if( ! isRegisteredType( auxid ) ) return kTRUE;

      // The variables needed for the check:
      ::TClass* cl = 0;
      ::EDataType dType = kOther_t;

      // Get the variable type from the branch:
      if( br->GetExpectedType( cl, dType ) ) {
         ::Error( "xAOD::TAuxStore::isContainerBranch",
                  XAOD_MESSAGE( "Couldn't determine the type of branch "
                                "\"%s\"" ), br->GetName() );
         return kFALSE;
      }

      // If there is no class associated with the branch then it should be
      // a branch describing a standalone object. (As it should be a
      // "primitive" branch in this case.)
      if( ! cl ) {
         return kFALSE;
      }

      // If there is a class, ask for the type_info of its type:
      const std::type_info* root_ti = cl->GetTypeInfo();
      if( ! root_ti ) {
         // This may be an emulated class. One known case is when the type name
         // is saved as "basic_string<char>" rather than "string" by Athena I/O.
         // (It's not fully understood why this happens for dynamic branches...)
         // So, let's see if we can get a functional TClass by massaging the
         // type name a bit.
         ::TString typeName( cl->GetName() );
         typeName.ReplaceAll( "basic_string<char>", "string" );
         ::TClass* newCl = ::TClass::GetClass( typeName );
         if( newCl ) {
            root_ti = newCl->GetTypeInfo();
         }
      }
      if( ! root_ti ) {
         ::Error( "xAOD::TAuxStore::isContainerBranch",
                  XAOD_MESSAGE( "Couldn't get an std::type_info object out of "
                                "branch \"%s\" of type \"%s\"" ),
                  br->GetName(), cl->GetName() );
         return kFALSE;
      }

      // Ask for the auxiliary type infos:
      const std::type_info* aux_obj_ti =
         SG::AuxTypeRegistry::instance().getType( auxid );
      if( ! aux_obj_ti ) {
         ::Error( "xAOD::TAuxStore::isContainerBranch",
                  XAOD_MESSAGE( "Couldn't get std::type_info object for "
                                "auxiliary id: %i" ),
                  static_cast< int >( auxid ) );
         return kFALSE;
      }
      const std::type_info* aux_vec_ti =
         SG::AuxTypeRegistry::instance().getVecType( auxid );
      if( ! aux_vec_ti ) {
         ::Error( "xAOD::TAuxStore::isContainerBranch",
                  XAOD_MESSAGE( "Couldn't get std::type_info object for "
                                "auxiliary id: %i" ),
                  static_cast< int >( auxid ) );
         return kFALSE;
      }

      // Check which one the ROOT type info agrees with:
      if( *root_ti == *aux_obj_ti ) {
         // This branch describes a single object:
         return kFALSE;
      } else if( *root_ti == *aux_vec_ti ) {
         // This branch describes a container of objects:
         return kTRUE;
      }

      // For enum and vector<enum> types (PFO...) the type given by
      // the aux type registry is vector<int>. We have to take it into account
      // here...
      if( cl->GetCollectionProxy() &&
          ( *aux_vec_ti == typeid( std::vector< int > ) ) ) {
         return kTRUE;
      }

      if (cl) {
        TClass* cl2 = lookupVectorType (cl);
        if (cl2) {
          if (*cl2->GetTypeInfo() == *aux_vec_ti)
            return kTRUE;
        }
      }

     // If neither, then something went wrong...
      ::Error( "xAOD::TAuxStore::isContainerBranch",
               XAOD_MESSAGE( "Couldn't determine if branch describes a single "
                             "object or a container" ) );
      ::Error( "xAOD::TAuxStore::isContainerBranch",
               XAOD_MESSAGE( "ROOT type  : %s" ),
               Utils::getTypeName( *root_ti ).c_str() );
      ::Error( "xAOD::TAuxStore::isContainerBranch",
               XAOD_MESSAGE( "Object type: %s" ),
               Utils::getTypeName( *aux_obj_ti ).c_str() );
      ::Error( "xAOD::TAuxStore::isContainerBranch",
               XAOD_MESSAGE( "Vector type: %s" ),
               Utils::getTypeName( *aux_vec_ti ).c_str() );
      return kFALSE;
   }

   /// This function is used to test if a given auxiliary variable is known
   /// in the registry with a proper type.
   ///
   /// @returns <code>kTRUE</code> if the full type of the auxiliary property
   ///          is known, <code>kFALSE</code> otherwise
   ///
   ::Bool_t TAuxStore::isRegisteredType( auxid_t auxid ) {

      // Cache some data:
      static SG::AuxTypeRegistry& registry =
         SG::AuxTypeRegistry::instance();
      static const auxid_t sauxid =
         registry.getAuxID< SG::AuxTypePlaceholder >( "AuxTypePlaceholder" );
      static const std::type_info* sti = registry.getVecType( sauxid );

      // Check if the types match:
      return ( *sti != *( registry.getVecType( auxid ) ) );
   }

   TAuxStore::TBranchHandle::TBranchHandle( ::Bool_t staticBranch,
                                            ::Bool_t primitiveBranch,
                                            const std::type_info* ti,
                                            void* obj, SG::auxid_t auxid,
                                            const std::string* prefix )
      : m_branch( 0 ), m_object( obj ), m_static( staticBranch ),
        m_primitive( primitiveBranch ),
        m_typeInfo( ti ), m_needsRead( kTRUE ), m_auxid( auxid ),
        m_prefix( prefix ) {

   }

   /// This function takes care of implementing just-in-time reading of
   /// branches for us.
   ///
   /// @param entry The entry to read in for the managed branch
   /// @returns The number of bytes read. A negative number in case of error.
   //
   ::Int_t TAuxStore::TBranchHandle::getEntry( ::Long64_t entry ) {

      // A little sanity check:
      if( ! m_branch ) {
         // This is no longer an error. We can have such objects for
         // decorations, which don't exist on the input.
         return 0;
      }

      // Update the I/O monitoring:
      IOStats::instance().stats().readBranch( *m_prefix, m_auxid );

      // Check if anything needs to be done:
      if( ( m_branch->GetReadEntry() == entry ) && ( ! m_needsRead ) ) {
         return 0;
      }

      // Switch the branch in the right mode:
      if( ! m_primitive ) {
         if( ( m_branch->GetMakeClass() != m_static ) &&
             ( ! m_branch->SetMakeClass( m_static ) ) ) {
            ::Error( "xAOD::TAuxStore::TBranchHandle::getEntry",
                     XAOD_MESSAGE( "Failed to call SetMakeClass(%i) on "
                                   "branch \"%s\"" ),
                     static_cast< int >( m_static ), m_branch->GetName() );
            return -1;
         }
      }

      // The reading will now be done:
      m_needsRead = kFALSE;

      // Read the entry:
      return m_branch->GetEntry( entry );
   }

   /// No magic here. <code>TTree::SetBranchAddress</code> needs a pointer
   /// to a <code>TBranch</code> pointer. This function just makes sure that
   /// we can give it such a pointer, which will stay valid during the job.
   ///
   /// @returns A pointer to the branch object handled by this object
   ///
   ::TBranch** TAuxStore::TBranchHandle::branchPtr() {

      return &m_branch;
   }

   /// This function is used in the implementation of the IAuxStoreIO interface.
   /// In there we just need a simple pointer to the object, we don't care
   /// about ROOT's idiosyncracies.
   ///
   /// @returns A pointer to the object in memory
   ///
   void* TAuxStore::TBranchHandle::objectPtr() {

      return m_object;
   }

   /// This is a tricky one. When reading a single sub-branch of a branch
   /// that was written using an auxiliary container with direct ROOT I/O,
   /// TTree expects us to give it a simple pointer to the object that it
   /// should fill with content. But when reading a top level branch, like
   /// the dynamically created branches that we make, TTree needs to receive
   /// a pointer to a pointer.
   ///
   /// Also, when reading a primitive type, ROOT also expects a simple pointer
   /// to the primitive variable in memory.
   ///
   /// This function returns the correct kind of pointer for
   /// <code>TTree::SetBranchAddress</code> in all situations.
   ///
   /// @returns The pointer to be given to <code>TTree::SetBranchAddress</code>
   ///
   void* TAuxStore::TBranchHandle::inputObjectPtr() {

      // Return the correct pointer:
      if( m_static || m_primitive ) {
         return m_object;
      } else {
         return &m_object;
      }
   }

   /// This is a slightly simplified version of GetInputObjectPtr(), for the
   /// case when we need to write a branch, and have to pass a pointer to
   /// <code>TTree::Branch</code>. In this case we don't need to be mindful of
   /// whether the variable was read as a static or dynamic variable. The only
   /// interesting thing is if the variable is of a primitive or an object
   /// type.
   ///
   /// @returns The pointer to be given to <code>TTree::Branch</code>
   ///
   void* TAuxStore::TBranchHandle::outputObjectPtr() {

      // Return the correct pointer:
      if( m_primitive ) {
         return m_object;
      } else {
         return &m_object;
      }
   }

   const std::type_info* TAuxStore::TBranchHandle::typeInfo() const {

      return m_typeInfo;
   }

   /// After TEvent::Fill is called, the TAuxStore objects used for data
   /// writing are cleared. Since TEvent doesn't know if the object is also
   /// used to read information or not. (If it's just used for writing, then
   /// this explicit clearing is mandatory.)
   ///
   /// In case the user wants to continue processing the same event after
   /// the fill() call (because for instance the code is writing multiple
   /// trees), then we need to be aware that the variable needs to be
   /// re-read from the input to be in a good shape for the next write
   /// operation.
   ///
   void TAuxStore::TBranchHandle::reset() {

      m_needsRead = kTRUE;
      return;
   }

} // namespace xAOD
