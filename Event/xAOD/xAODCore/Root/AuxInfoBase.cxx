/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxInfoBase.cxx 793737 2017-01-24 20:11:10Z ssnyder $

// System include(s):
#include <iostream>
#include <stdexcept>

// EDM include(s):
#include "AthContainersInterfaces/AuxDataOption.h"
#include "AthContainers/AuxStoreStandalone.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/exceptions.h"

// Local include(s):
#include "xAODCore/AuxInfoBase.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

namespace xAOD {

   AuxInfoBase::AuxInfoBase( bool allowDynamicVars )
      : SG::IAuxStore(),
        m_selection(),
        m_auxids(), m_vecs(), m_store( 0 ), m_storeIO( 0 ),
        m_ownsStore( true ),
        m_locked( false ),
        m_name( "UNKNOWN" ) {

      if( allowDynamicVars ) {
         m_store = new SG::AuxStoreStandalone();
         m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      }
   }

   /// This is an interesting one. The internal variables of AuxInfoBase
   /// are set up by the derived class(es) at construction. So, this internal
   /// state is not to be copied!
   ///
   /// However, since the derived classes (the contents of their regular
   /// members) are allowed to be copied, let's not make this constructor
   /// private.
   ///
   AuxInfoBase::AuxInfoBase( const AuxInfoBase& parent )
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
         m_auxids.insert( m_store->getAuxIDs().begin(),
                          m_store->getAuxIDs().end() );
      }
   }

   /// This constructor is used to wrap another object that is stored
   /// in StoreGate, and hence we should not take ownership of.
   ///
   /// @param store Another store that should be wrapped, but not owned
   ///
   AuxInfoBase::AuxInfoBase( const SG::IAuxStore* store )
      : SG::IAuxStore(),
        m_selection(),
        m_auxids(), m_vecs(),
        m_store( const_cast< SG::IAuxStore* >( store ) ),
        m_storeIO( 0 ), m_ownsStore( false ),
        m_locked( false ),
        m_name( "UNKNOWN" ) {

      m_storeIO = dynamic_cast< SG::IAuxStoreIO* >( m_store );
      if( m_store ) {
         m_auxids.insert (m_store->getAuxIDs());
      }
   }

   AuxInfoBase::~AuxInfoBase() {

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
   AuxInfoBase&
   AuxInfoBase::operator=( const AuxInfoBase& rhs ) {

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
   //          Implementation of the SG::IAuxStoreHolder functions
   //

   SG::IAuxStore* AuxInfoBase::getStore()
   {
      return m_store;
   }


   const SG::IAuxStore* AuxInfoBase::getStore() const
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
   void AuxInfoBase::setStore( SG::IAuxStore* store ) {

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

   const void* AuxInfoBase::getData( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_store ) {
            size_t nids = m_store->getAuxIDs().size();
            const void* result = m_store->getData( auxid );
            if( result && ( nids != m_store->getAuxIDs().size() ) ) {
               m_auxids.insert( auxid );
            }
            return result;
         } else {
            std::cout << "ERROR xAOD::AuxInfoBase::getData "
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

   bool AuxInfoBase::isDecoration (auxid_t auxid) const
   {
     guard_t guard( m_mutex );
     if (m_store) {
       return m_store->isDecoration (auxid);
     }
     return false;
   }

   const AuxInfoBase::auxid_set_t&
   AuxInfoBase::getAuxIDs() const {

      // Return the full list of IDs:
      return getWritableAuxIDs();
   }

   void* AuxInfoBase::getDecoration( auxid_t auxid, size_t size,
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
           std::cout << "ERROR xAOD::AuxInfoBase::getDecoration "
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

   void AuxInfoBase::lock() {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Lock the container and the dynamic store:
      m_locked = true;
      if( m_store ) {
         m_store->lock();
      }

      return;
   }

   bool AuxInfoBase::clearDecorations() {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Clear the decorations that are in the dynamic store:
      bool anycleared = false;
      if( m_store ) {
         anycleared = m_store->clearDecorations();
      }
      // Early exit if there were no decorations.
      if (!anycleared) return false;

      // Reconstruct the list of managed auxiliary IDs from scratch:
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
   void AuxInfoBase::lockDecoration (SG::auxid_t auxid)
   { 
     guard_t guard (m_mutex);
     if (m_store) {
       m_store->lockDecoration (auxid);
     }
   }

   size_t AuxInfoBase::size() const {

      // Should really always be 1, but do the general thing anyway...

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Try to find a variable:
      for (SG::auxid_t i : m_auxids) {
         if( ( i < m_vecs.size() ) && m_vecs[ i ] ) {
            size_t sz = m_vecs[ i ]->size();
            if( sz > 0 ) {
               return sz;
            }
         }
      }

      // If we didn't find any statically defined variables, then we just
      // need to claim 1. Because pool::AuxStoreAPR at the moment always
      // returns 0 for the size.
      return 1;
   }

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the SG::IAuxStore functions
   //

   void* AuxInfoBase::getData( auxid_t auxid, size_t size,
                               size_t capacity ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {

         if( m_store ) {
            size_t nids = m_store->getAuxIDs().size();
            void* result = m_store->getData( auxid, size, capacity );
            if( result && ( nids != m_store->getAuxIDs().size() ) ) {
               m_auxids.insert( auxid );
            }
            return result;
         } else {
            std::cout << "ERROR xAOD::AuxInfoBase::getData "
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

   const AuxInfoBase::auxid_set_t&
   AuxInfoBase::getWritableAuxIDs() const {

      // Return the full list of known IDs. The constness of this object's
      // members comes from the object being const or not.
      return m_auxids;
   }

   bool AuxInfoBase::resize( size_t size ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Check if the container is locked:
      if( m_locked ) {
        throw SG::ExcStoreLocked( "resize" );
      }

      // Do a test already here:
      if( size != 1 ) {
         throw std::runtime_error( "Calling resize with != 1 on a "
                                   "non-vector" );
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

   void AuxInfoBase::reserve( size_t size ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Check if the container is locked:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "reserve" );
      }

      // Do a test already here:
      if( size != 1 ) {
         throw std::runtime_error( "Calling reserve with != 1 on a "
                                   "non-vector" );
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

   void AuxInfoBase::shift( size_t /*pos*/, ptrdiff_t /*offs*/ ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Check if the container is locked:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "shift" );
      }

      // We are just not allowed to do this...
      throw std::runtime_error( "Calling shift on a non-vector" );

      return;
   }


   bool AuxInfoBase::insertMove( size_t /*pos*/, IAuxStore& /*other*/,
                                 const SG::auxid_set_t& /*ignore*/ ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      // Check if the container is locked:
      if( m_locked ) {
         throw SG::ExcStoreLocked( "insertMove" );
      }

      // We are just not allowed to do this...
      throw std::runtime_error( "Calling insertMove on a non-vector" );

      return false;
   }


   bool AuxInfoBase::setOption( auxid_t id, const SG::AuxDataOption& option ) {

      if (id < m_vecs.size() && m_vecs[id] != 0)
         return m_vecs[id]->setOption( option );
      if (m_store)
         return m_store->setOption( id, option );
      return false;
   }


   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the SG::IAuxStoreIO functions
   //

   const void* AuxInfoBase::getIOData( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_storeIO ) {
            return m_storeIO->getIOData( auxid );
         } else {
            std::cout << "ERROR xAOD::AuxInfoBase::getIOData "
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

   const std::type_info* AuxInfoBase::getIOType( auxid_t auxid ) const {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      if( ( auxid >= m_vecs.size() ) || ( ! m_vecs[ auxid ] ) ) {
         if( m_storeIO ) {
            return m_storeIO->getIOType( auxid );
         } else {
            std::cout << "ERROR xAOD::AuxInfoBase::getIOType "
                      << "Unknown variable ("
                      << SG::AuxTypeRegistry::instance().getName( auxid )
                      << ") requested" << std::endl;
            return 0;
         }
      }

      return SG::AuxTypeRegistry::instance().getType( auxid );
   }

   const AuxInfoBase::auxid_set_t&
   AuxInfoBase::getDynamicAuxIDs() const {

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

   void AuxInfoBase::selectAux( const std::set< std::string >& attributes ) {

      // Guard against multi-threaded execution:
      guard_t guard( m_mutex );

      m_selection.selectAux( attributes );
      return;
   }

   AuxInfoBase::auxid_set_t
   AuxInfoBase::getSelectedAuxIDs() const {

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

   const char* AuxInfoBase::name() const {

      return m_name.c_str();
   }

   void AuxInfoBase::setName( const char* name ) {

      m_name = name;
      return;
   }

} // namespace xAOD
