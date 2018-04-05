/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// OutputHandling includes
#include "HLTEDMCreator.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"




HLTEDMCreator::HLTEDMCreator( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent )
  : base_class( type, name, parent ) {}



template<typename T>
StatusCode HLTEDMCreator::initAndCheckHandles( SG::WriteHandleKeyArray<T>& handles ) {
  for ( auto handle: handles ) {
    if ( handle.key().find("HLT_") != 0 ) {
      ATH_MSG_WARNING( "The collection of type DataVector<" << typeid(typename T::value_type).name() << ">"
		       << " has key " << handle.key() << " that is not starting with the HLT_" );      
    }
  } 
  return handles.initialize();
}


StatusCode HLTEDMCreator::initialize()
{  
  CHECK( initAndCheckHandles( m_TrigRoiDescriptorCollection ) );
  CHECK( initAndCheckHandles( m_TrigCompositeContainer ) );
  CHECK( initAndCheckHandles( m_TrigEMClusterContainer ) );
  CHECK( initAndCheckHandles( m_TrigCaloClusterContainer ) );
  CHECK( initAndCheckHandles( m_TrigElectronContainer ) );
  CHECK( initAndCheckHandles( m_TrigPhotonContainer ) );
  CHECK( initAndCheckHandles( m_TrackParticleContainer ) );
  
  return StatusCode::SUCCESS;
}


template<class T>
StatusCode plainGenerator( SG::WriteHandle<T>& h ) {
  return h.record( std::move( std::make_unique<T>() ) );
} 


template<class T, class STORE>
StatusCode xAODGenerator( SG::WriteHandle<T>& h ) {
  auto coll = std::make_unique<T>();
  auto store = std::make_unique<STORE>();
  coll->setStore( store.get() );      
  return h.record( std::move( coll ), std::move( store ) );
} 


template<typename T, typename G>
StatusCode HLTEDMCreator::createIfMissing( const SG::WriteHandleKeyArray<T>& handles, G createAndRecord ) const {

  for ( auto writeHandleKey : handles ) {
    SG::ReadHandle<T> readHandle( writeHandleKey.key() );
    if ( readHandle.isValid() ) {
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " already present" );
    } else {
      SG::WriteHandle<T> writeHandle = SG::makeHandle( writeHandleKey );
      CHECK( createAndRecord( writeHandle ) );

      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " was absent, created empty one" );
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode HLTEDMCreator::createOutput() const {

#define CREATE(__TYPE) \
  CHECK( createIfMissing<__TYPE>( m_##__TYPE, plainGenerator<__TYPE> ) );

  CREATE( TrigRoiDescriptorCollection );

#undef CREATE

#define CREATE_XAOD(__TYPE, __STORE_TYPE) \
  CHECK( createIfMissing<xAOD::__TYPE>( m_##__TYPE, xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE> ) );
  
  CREATE_XAOD( TrigCompositeContainer, TrigCompositeAuxContainer );
  CREATE_XAOD( TrigElectronContainer, TrigElectronAuxContainer );
  CREATE_XAOD( TrigPhotonContainer, TrigPhotonAuxContainer );
  CREATE_XAOD( TrigEMClusterContainer, TrigEMClusterAuxContainer );
  CREATE_XAOD( TrigCaloClusterContainer, TrigCaloClusterAuxContainer );
  CREATE_XAOD( TrackParticleContainer, TrackParticleAuxContainer );

#undef CREATE_XAOD
  return StatusCode::SUCCESS;
}
