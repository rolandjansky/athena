/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxContainerBase.cxx 611306 2014-08-11 16:48:11Z ssnyder $

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
        m_tick( 0 ) {

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
        m_tick( 1 ) {

      // Unfortunately the dynamic variables can not be copied this easily...
      if( parent.m_store ) {
         m_store = parent.m_store;
         m_ownsStore = false;
         m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
         m_selection = parent.m_selection;
         m_auxids.insert( m_store->getAuxIDs().begin(),
                          m_store->getAuxIDs().end() );
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
        m_tick( 1 ) {

      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      if( m_store ) {
         m_auxids.insert( m_store->getAuxIDs().begin(),
                          m_store->getAuxIDs().end() );
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
         auxid_set_t::const_iterator itr = m_store->getAuxIDs().begin();
         auxid_set_t::const_iterator end = m_store->getAuxIDs().end();
         for( ; itr != end; ++itr ) {
            m_auxids.erase( *itr );
         }
         ++m_tick;
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
         m_auxids.insert( m_store->getAuxIDs().begin(),
                          m_store->getAuxIDs().end() );
         ++m_tick;
      }

      return *this;
   }

   SG::IAuxStore* AuxContainerBase::getStore() const {

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

      guard_t guard (m_mutex);

      // Check that no funny business is going on:
      if( m_store == store ) return;

      // Clean up the current store object:
      if( m_store && m_ownsStore ) {
         auxid_set_t::const_iterator itr = m_store->getAuxIDs().begin();
         auxid_set_t::const_iterator end = m_store->getAuxIDs().end();
         for( ; itr != end; ++itr ) {
            m_auxids.erase( *itr );
         }
         ++m_tick;
         delete m_store;
      }
      m_store = 0;
      m_storeIO = 0;

      // Take posession of the new object:
      m_store = store;
      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      m_ownsStore = true;
      if( m_store ) {
         m_auxids.insert( m_store->getAuxIDs().begin(),
                          m_store->getAuxIDs().end() );
         ++m_tick;
      }

      return;
   }

   const void* AuxContainerBase::getData( auxid_t auxid ) const {

      guard_t guard (m_mutex);

      // Update the statistics for this variable:
      BranchStats* stat = IOStats::instance().stats().branch( "", auxid );
      stat->setReadEntries( stat->readEntries() + 1 );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_store ) {
            const void* result = m_store->getData( auxid );
            if( result ) {
               m_auxids.insert( auxid );
               ++m_tick;
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

      return m_vecs[ auxid ]->toPtr();
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getAuxIDs() const {
      {
        guard_t guard (m_mutex);
        // Add the dynamic variables if they exist:
        if( m_storeIO ) {
          m_auxids.insert( m_store->getAuxIDs().begin(),
                           m_store->getAuxIDs().end() );
          ++m_tick;
        }
      }

      // Return the full list of IDs:
      return getWritableAuxIDs();
   }

   void* AuxContainerBase::getData( auxid_t auxid, size_t size,
                                    size_t capacity ) {

      guard_t guard (m_mutex);

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {

         if( m_store ) {
            m_auxids.insert( auxid );
            ++m_tick;
            return m_store->getData( auxid, size, capacity );
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

   void*
   AuxContainerBase::getDecoration (auxid_t auxid, size_t size, size_t capacity)
   {
     guard_t guard (m_mutex);
     if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
       if( m_store ) {
         m_auxids.insert (auxid);
         ++m_tick;
         return m_store->getDecoration (auxid, size, capacity);
       }
       else {
         std::cout << "ERROR xAOD::AuxContainerBase::getData "
                   << "Unknown variable ("
                   << SG::AuxTypeRegistry::instance().getName( auxid )
                   << ") requested" << std::endl;
         return 0;
       }
     }
     if (m_locked)
       throw SG::ExcStoreLocked (auxid);
     return getData (auxid, size, capacity);
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getWritableAuxIDs() const {

      // Return the full list of known IDs. The constness of this object's
      // members comes from the object being const or not. 
      guard_t guard (m_mutex);
      if (m_tsAuxids.get() == 0) {
        m_tsAuxids.reset (new TSAuxidSet (m_tick, m_auxids));
      }
      else if (m_tsAuxids->m_tick != m_tick) {
        m_tsAuxids->m_set = m_auxids;  // May need to optimize this!
        m_tsAuxids->m_tick = m_tick;
      }
      return m_tsAuxids->m_set;
   }

   void AuxContainerBase::resize( size_t size ) {

       guard_t guard (m_mutex);
       if (m_locked)
         throw SG::ExcStoreLocked ("resize");

      // Do the operation on the static variables:
      std::vector< SG::IAuxTypeVector* >::iterator itr = m_vecs.begin();
      std::vector< SG::IAuxTypeVector* >::iterator end = m_vecs.end();
      for( ; itr != end; ++itr ) {
         if( *itr ) ( *itr )->resize( size );
      }

      // Do the operation on the dynamic variables:
      if( m_store ) {
         m_store->resize( size );
      }

      return;
   }

   void AuxContainerBase::reserve( size_t size ) {

       guard_t guard (m_mutex);
       if (m_locked)
         throw SG::ExcStoreLocked ("reserve");

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

       guard_t guard (m_mutex);
       if (m_locked)
         throw SG::ExcStoreLocked ("shift");

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

   const void* AuxContainerBase::getIOData( auxid_t auxid ) const {

      guard_t guard (m_mutex);
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

      return m_vecs[ auxid ]->toVector();
   }
   
   const std::type_info* AuxContainerBase::getIOType( auxid_t auxid ) const {

      guard_t guard (m_mutex);
      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_storeIO ) {
            return m_storeIO->getIOType( auxid );
         } else {
            std::cout << "ERROR xAOD::AuxContainerBase::getIOType "
                      << "Unknown variable ("
                      << SG::AuxTypeRegistry::instance().getName( auxid )
                      << ") requested" << std::endl;
            return 0;
         }
      }

      return SG::AuxTypeRegistry::instance().getVecType( auxid );
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getDynamicAuxIDs() const {

      guard_t guard (m_mutex);

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
      static const auxid_set_t dummy;
      return dummy;
   }

   void
   AuxContainerBase::selectAux( const std::set< std::string >& attributes ) {

      guard_t guard (m_mutex);

      m_selection.selectAux( attributes );
      return;
   }

   const AuxContainerBase::auxid_set_t&
   AuxContainerBase::getSelectedAuxIDs() const {

      guard_t guard (m_mutex);

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
      static auxid_set_t dummy;
      return dummy;
   }

   void AuxContainerBase::lock()
   {
     guard_t guard (m_mutex);
     m_locked = true;
     if (m_store)
       m_store->lock();
   }

   void AuxContainerBase::clearDecorations()
   {
     guard_t guard (m_mutex);
     if (m_store)
       m_store->clearDecorations();
     m_auxids.clear();
     for (auxid_t auxid = 0; auxid < m_vecs.size(); auxid++) {
       if (m_vecs[auxid])
         m_auxids.insert (auxid);
     }
     ++m_tick;
   }

   size_t AuxContainerBase::size() const
   {
     guard_t guard (m_mutex);
     auxid_set_t::const_iterator i = m_auxids.begin();
     auxid_set_t::const_iterator end = m_auxids.end();
     for (; i != end; ++i) {
       if (*i < m_vecs.size() && m_vecs[*i]) {
         size_t sz = m_vecs[*i]->size();
         if (sz > 0)
           return sz;
       }
     }

     if (m_store)
       return m_store->size();
     return 0;
   }


} // namespace xAOD
