/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TypeProxy.cxx 610817 2014-08-07 18:30:36Z lheinric $

// EDM include(s):
#include "AthContainersInterfaces/IAuxStore.h"

// Local include(s):
#include "TrigNavigation/TypeProxy.h"

namespace HLTNavDetails {

   ITypeProxy::ITypeProxy()
      : m_clid( 0 ), m_typeName( "" ), m_key( "" ), m_proxy( 0 ),
        m_ncPointer( 0 ), m_pointer( 0 ), m_isAuxVectorBase( false ) {

   }

   StatusCode ITypeProxy::sync( StoreGateSvc* sg, const std::string& key ) {

      // Get a proxy for this object:
      m_proxy = sg->proxy( m_clid, key );
      if( ! m_proxy ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "HLTNavDetails::ITypeProxy::sync" )
            << "Couldn't get SG::DataProxy to the managed object";
         return StatusCode::FAILURE;
      }

      // Get the pointers:
      m_ncPointer = SG::DataProxy_cast( m_proxy, m_clid );
      m_pointer = m_ncPointer;

      // Remember this key:
      m_key = key;

      // The rest is up to the code in the template class:
      return StatusCode::SUCCESS;
   }

   SG::AuxVectorBase* ITypeProxy::castAuxVectorBase() {

      if( m_isAuxVectorBase ) {
         return static_cast< SG::AuxVectorBase* >( m_ncPointer );
      } else {
         return 0;
      }
   }

   /// This cast can be done based on StoreGate, since SG::IAuxStore
   /// has a CLID, as should the type that we're proxying
   ///
   SG::IAuxStore* ITypeProxy::castIAuxStore() {

      return static_cast< SG::IAuxStore* >( m_ncPointer );
   }

   bool ITypeProxy::contains( StoreGateSvc* sg, const std::string& key ) const {

      return sg->contains( m_clid, key );
   }

   bool ITypeProxy::transientContains( StoreGateSvc* sg,
                                       const std::string& key ) const {

      return sg->transientContains( m_clid, key );
   }

   bool ITypeProxy::empty() const {

      return ( ( m_proxy == 0 ) && ( m_ncPointer == 0 ) );
   }

   CLID ITypeProxy::clid() const {

      return m_clid;
   }

   const std::string& ITypeProxy::typeName() const {

      return m_typeName;
   }

} // namespace HLTNavDetails
