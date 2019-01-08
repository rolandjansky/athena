/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxContainerBase.cxx 793746 2017-01-24 21:23:52Z ssnyder $

// System include(s):
#include <iostream>

// EDM include(s):
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"

// Local include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace std;

namespace xAOD {

   AuxContainerBase::AuxContainerBase( bool allowDynamicVars )
      : SG::IAuxStore(),
        m_selection(),
        m_auxids(), m_vecs(), m_store( 0 ), m_storeIO( 0 ),
        m_ownsStore( true ),
        m_locked( false ),
        m_name( "UNKNOWN" ) {

      if( allowDynamicVars ) {
         m_store = new SG::AuxStoreInternal();
         m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      }
   }

   /// This is an interesting one. The internal variables of AuxContainerBase
   /// are set up by the derived class(es) at construction. So, this internal
   /// state is not to be copied!
   ///
   /// However, since the derived classes (the contents of their regular
   /// members) are allowed to be copied, let's not make this constructor
   /// private.
   ///
   AuxContainerBase::AuxContainerBase( const AuxContainerBase& parent )
      : SG::IAuxStore(),
        m_selection( parent.m_selection ),
        m_auxids(), m_vecs(), m_store( 0 ), m_storeIO( 0 ),
        m_ownsStore( true ),
        m_locked( false ),
        m_name( parent.m_name ) {

      // Unfortunately the dynamic variables can not be copied this easily...
      if( parent.m_store ) {
         m_store = parent.m_store;
         m_ownsStore = false;
         m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
         m_selection = parent.m_selection;
         m_auxids = m_store->getAuxIDs();
      }
   }

   /// This constructor is used to wrap another object that is stored
   /// in StoreGate, and hence we should not take ownership of.
   ///
   /// @param store Another store that should be wrapped, but not owned
   ///
   AuxContainerBase::AuxContainerBase( const SG::IAuxStore* store )
      : SG::IAuxStore(),
        m_selection(),
        m_auxids(), m_vecs(),
        m_store( const_cast< SG::IAuxStore* >( store ) ),
        m_storeIO( 0 ), m_ownsStore( false ),
        m_locked( false),
        m_name( "UNKNOWN" ) {

      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      if( m_store ) {
         m_auxids = m_store->getAuxIDs();
      }
   }

   AuxContainerBase::~AuxContainerBase() {

      std::vector< SG::IAuxTypeVector* >::iterator itr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator end = m_vecs.end();
      for( ; itr != end; ++itr ) {
         if( ! *itr ) continue;
         delete *itr;
      }

      if( m_store && m_ownsStore ) delete m_store;
   }

   /// Just like the copy constructor, this operator doesn't actually copy
   /// anything either. It's just here to make it clear to C++ that the
   /// internal state of these objects is not to be messed with.
   ///
   /// @returns A reference to this same object
   ///
   AuxContainerBase&
   AuxContainerBase::operator=( const AuxContainerBase& rhs ) {

      // Protect against self-assignment:
      if( this == &rhs ) return *this;

      m_selection = rhs.m_selection;

      // Clean up after the old dynamic store:
      if( m_store && m_ownsStore ) {
         m_auxids -= m_store->getAuxIDs();
         delete m_store;
      }
      m_store = 0;
      m_storeIO = 0;
      m_ownsStore = true;

      // Take posession of the new dynamic store:
      if( rhs.m_store ) {
         m_store = rhs.m_store;
         m_ownsStore = false;
         m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
         m_auxids.insert (m_store->getAuxIDs());
      }

      m_name = rhs.m_name;

      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //         Implementation of the SG::IAuxStoreHolder functions
   //

   SG::IAuxStore* AuxContainerBase::getStore()
   {
      return m_store;
   }

   const SG::IAuxStore* AuxContainerBase::getStore() const
   {
      return m_store;
   }

   /// This function is used by the I/O infrastructure to possibly put a store
   /// object into this one, which can interact with dynamic variables
   /// directly.
   ///
   /// Note that the object takes ownership of the received store.
   ///
   /// @param store The store that should be used for dynamic variable handling
   ///              inside the object from now on
   ///
   void AuxContainerBase::setStore( SG::IAuxStore* store ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Check that no funny business is going on:
      if( m_store == store ) return;

      // Clean up the current store object:
      if( m_store && m_ownsStore ) {
         m_auxids -= m_store->getAuxIDs();
         delete m_store;
      }
      m_store = 0;
      m_storeIO = 0;

      // Take posession of the new object:
      m_store = store;
      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      m_ownsStore = true;
      if( m_store ) {
         m_auxids.insert (m_store->getAuxIDs());
      }

      return;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the SG::IConstAuxStore functions
   //

   const void* AuxContainerBase::getData( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_store ) {
            size_t nids = m_store->getAuxIDs().size();
            const void* result = m_store->getData( auxid );
            if( result && nids != m_store->getAuxIDs().size() ) {
               m_auxids.insert( auxid );
            }
            return result;
         } else {
            std::cout << "ERROR xAOD::AuxContainerBase::getData "
                      << "Unknown variable ("
                      << SG::AuxTypeRegistry::instance().getName( auxid )
                      << ") requested" << std::endl;
            return 0;
         }
      }

      // Update the statistics for this variable. The dynamic store registers
      // its own variable accesses.
      IOStats::instance().stats().readBranch( m_name, auxid );

      return m_vecs[ auxid ]->toPtr();
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getAuxIDs() const {

      // Return the full list of IDs:
      return getWritableAuxIDs();
   }

   bool AuxContainerBase::isDecoration (auxid_t auxid) const
   {
     guard_t guard( m_mutex );
     if (m_store) {
       return m_store->isDecoration (auxid);
     }
     return false;
   }

   void* AuxContainerBase::getDecoration( auxid_t auxid, size_t size,
                                          size_t capacity ) {
     {
       // Guard against multi-threaded execution:
       guard_t guard( m_mutex );

       // Check if we have it as a static variable:
       if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         // If not, but we have a dynamic store, push it in there:
         if( m_store ) {
           size_t nids = m_store->getAuxIDs().size();
           void* result = m_store->getDecoration( auxid, size, capacity );
           if( result && ( nids != m_store->getAuxIDs().size() ) ) {
             m_auxids.insert( auxid );
           }
           return result;
         }
         // If we don't have a dynamic store, complain:
         else {
           std::cout << "ERROR xAOD::AuxContainerBase::getDecoration "
                     << "Can't provide variable "
                     << SG::AuxTypeRegistry::instance().getName( auxid )
                     << std::endl;
           return 0;
         }
       }

       // If the container is locked, static variables can't be accessed this
       // way:
       if( m_locked ) {
         throw SG::ExcStoreLocked( auxid );
       }
     }

     // If the container is not locked, then fall back on the normal accessor
     // function:
     return getData( auxid, size, capacity );
   }

   void AuxContainerBase::lock() {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Lock the object and its dynamic store:
      m_locked = true;
      if( m_store ) {
         m_store->lock();
      }

      return;
   }

   bool AuxContainerBase::clearDecorations() {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Clear the decorations from the dynamic store:
      bool anycleared = false;
      if( m_store ) {
         anycleared = m_store->clearDecorations();
      }
      // Early exit if there were no decorations.
      if (!anycleared) return false;

      // Construct the list of managed auxiliary variables from scratch after
      // the cleanup:
      auxid_set_t ids;
      for( auxid_t auxid = 0; auxid < m_vecs.size(); ++auxid ) {
         if( m_vecs[ auxid ] ) {
            ids.insert( auxid );
         }
      }
      if( m_store ) {
         ids.insert (m_store->getAuxIDs());
      }
      m_auxids = ids;

      return true;
   }
   
   /// Lock a decoration.
   void AuxContainerBase::lockDecoration (SG::auxid_t auxid)
   { 
     guard_t guard (m_mutex);
     if (m_store) {
       m_store->lockDecoration (auxid);
     }
   }

   size_t AuxContainerBase::size() const {

      // Guard against multi-threaded execution:
      guard_t guard (m_mutex);

      // Try to find a variable:
      for (SG::auxid_t i : m_auxids) {
         if( ( i < m_vecs.size() ) && m_vecs[ i ] ) {
            size_t sz = m_vecs[ i ]->size();
            if( sz > 0 ) {
               return sz;
            }
         }
      }

      // If we didn't find any, let's ask the dynamic store:
      if( m_store ) {
         return m_store->size();
      }

      // If we don't have any variables, then the size must be null:
      return 0;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the SG::IAuxStore functions
   //

   void* AuxContainerBase::getData( auxid_t auxid, size_t size,
                                    size_t capacity ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {

         if( m_store ) {
            size_t nids = m_store->getAuxIDs().size();
            void* result = m_store->getData( auxid, size, capacity );
            if( result && nids != m_store->getAuxIDs().size() ) {
               m_auxids.insert( auxid );
            }
            return result;
         } else {
            std::cout << "ERROR xAOD::AuxContainerBase::getData "
                      << "Unknown variable ("
                      << SG::AuxTypeRegistry::instance().getName( auxid )
                      << ") requested" << std::endl;
            return 0;
         }
      }
      m_vecs[ auxid ]->reserve( capacity );
      m_vecs[ auxid ]->resize( size );

      return m_vecs[ auxid ]->toPtr();
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getWritableAuxIDs() const {

      // Return the full list of known IDs. The constness of this object's
      // members comes from the object being const or not.
      return m_auxids;
   }

   bool AuxContainerBase::resize( size_t size ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // This operation is not allowed on a locked container:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "resize" );
      }

      // Do the operation on the static variables:
      bool nomoves = true;
      for (SG::IAuxTypeVector* v : m_vecs) {
         if(v) {
           if (!v->resize( size ))
             nomoves = false;
         }
      }

      // Do the operation on the dynamic variables:
      if( m_store ) {
        if (!m_store->resize( size ))
          nomoves = false;
      }

      return nomoves;
   }

   void AuxContainerBase::reserve( size_t size ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // This operation is not allowed on a locked container:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "reserve" );
      }

      // Do the operation on the static variables:
      std::vector< SG::IAuxTypeVector* >::iterator itr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator end = m_vecs.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) ( *itr )->reserve( size );
      }

      // Do the operation on the dynamic variables:
      if( m_store ) {
         m_store->reserve( size );
      }

      return;
   }

   void AuxContainerBase::shift( size_t pos, ptrdiff_t offs ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // This operation is not allowed on a locked container:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "shift" );
      }

      // Do the operation on the static variables:
      std::vector< SG::IAuxTypeVector* >::iterator itr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator end = m_vecs.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) ( *itr )->shift( pos, offs );
      }

      // Do the operation on the dynamic variables:
      if( m_store ) {
         m_store->shift( pos, offs );
      }

      return;
   }


   bool AuxContainerBase::insertMove( size_t pos,
                                      IAuxStore& other,
                                      const SG::auxid_set_t& ignore_in ) {
      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // This operation is not allowed on a locked container:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "insertMove" );
      }

      const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
      bool nomove = true;
      size_t other_size = other.size();

      SG::auxid_set_t ignore = ignore_in;
      
      // Do the operation on the static variables:
      for (SG::auxid_t id : m_auxids) {
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

      // Do the operation on the dynamic variables:
      if( m_store ) {
        if (!m_store->insertMove( pos, other, ignore ))
          nomove = false;

        // Notice any new variables added as a result of this.
        m_auxids.insert (m_store->getAuxIDs());
      }

      return nomove;
   }


   bool AuxContainerBase::setOption( auxid_t id,
                                     const SG::AuxDataOption& option ) {

      guard_t guard (m_mutex);
      if (id < m_vecs.size() && m_vecs[id] != 0)
         return m_vecs[id]->setOption (option);

      if (m_store)
         return m_store->setOption (id, option);
      return false;
   }


   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //           Implementation of the SG::IAuxStoreIO functions
   //

   const void* AuxContainerBase::getIOData( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // If the variable is not static, look for it in the dynamic store:
      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_storeIO ) {
            return m_storeIO->getIOData( auxid );
         } else {
            std::cout << "ERROR xAOD::AuxContainerBase::getIOData "
                      << "Unknown variable ("
                      << SG::AuxTypeRegistry::instance().getName( auxid )
                      << ") requested" << std::endl;
            return 0;
         }
      }

      // Update the statistics for this variable. The dynamic store registers
      // its own variable accesses.
      IOStats::instance().stats().readBranch( m_name, auxid );

      return m_vecs[ auxid ]->toVector();
   }

   const std::type_info* AuxContainerBase::getIOType( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // If the variable is not static, ask the dynamic store:
      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_storeIO ) {
            return m_storeIO->getIOType( auxid );
         }
      }

      // Fall back on getting the information from the registry:
      return SG::AuxTypeRegistry::instance().getVecType( auxid );
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getDynamicAuxIDs() const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // All the variables handled by the internal store are dynamic
      // if such a store exists:
      if( m_storeIO ) {
         // I mean, all the variables. Not just the ones reported as dynamic
         // by the internal object. Because the internal object may be something
         // that was put into this one in order to achieve data slimming.
         return m_store->getAuxIDs();
      }
      // In case we don't use an internal store, there are no dynamic
      // variables:
      static const auxid_set_t dummy (0);
      return dummy;
   }

   void
   AuxContainerBase::selectAux( const std::set< std::string >& attributes ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      m_selection.selectAux( attributes );
      return;
   }

   AuxContainerBase::auxid_set_t
   AuxContainerBase::getSelectedAuxIDs() const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // All the variables handled by the internal store are dynamic
      // if such a store exists:
      if( m_storeIO ) {
         // I mean, all the variables. Not just the ones reported as dynamic
         // by the internal object. Because the internal object may be something
         // that was put into this one in order to achieve data slimming.
         return m_selection.getSelectedAuxIDs( m_store->getAuxIDs() );
      }

      // In case we don't use an internal store, there are no dynamic
      // variables:
      return auxid_set_t();
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   const char* AuxContainerBase::name() const {

      return m_name.c_str();
   }

   void AuxContainerBase::setName( const char* name ) {

      m_name = name;
      return;
   }

} // namespace xAOD
