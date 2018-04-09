
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//#include "GaudiKernel/IToolSvc.h"
#include "AthViews/ViewHelper.h"

#include "HLTEDMCreator.h"



HLTEDMCreator::HLTEDMCreator( const std::string& type, 
			      const std::string& name, 
			      const IInterface* parent )
  : base_class( type, name, parent ) {}



template<typename T>
StatusCode HLTEDMCreator::initAndCheckOutputHandles( SG::WriteHandleKeyArray<T>& handles ) {
  for ( auto handle: handles ) {
    if ( handle.key().find("HLT_") != 0 ) {
      ATH_MSG_WARNING( "The collection of type DataVector<" << typeid(typename T::value_type).name() << ">"
		       << " has key " << handle.key() << " that is not starting with the HLT_" );      
    }
  } 
  return handles.initialize();
}

template<typename T>
StatusCode HLTEDMCreator::initAndCheckInViewHandles( SG::ReadHandleKey<T>& handle ) {
  CHECK( handle.initialize( not handle.key().empty() ) );
  renounce( handle );
  return StatusCode::SUCCESS;
}

StatusCode HLTEDMCreator::initialize()
{  

  CHECK( m_viewsKey.initialize( not m_viewsKey.key().empty() ) );
  renounce( m_viewsKey ); // they may be missing due to early rejection
  CHECK( initAndCheckOutputHandles( m_TrigRoiDescriptorCollection ) );      
  CHECK( initAndCheckOutputHandles( m_TrigCompositeContainer ) );
  CHECK( initAndCheckOutputHandles( m_TrigEMClusterContainer ) );
  CHECK( initAndCheckOutputHandles( m_TrigCaloClusterContainer ) );
  CHECK( initAndCheckOutputHandles( m_TrigElectronContainer ) );
  //  CHECK( initAndCheckInViewHandles( m_TrigElectronContainerInViews ) );
  CHECK( initAndCheckOutputHandles( m_TrigPhotonContainer ) );
  CHECK( initAndCheckOutputHandles( m_TrackParticleContainer ) );
  
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
/*
template<typename T>
struct NoMerge {
  StatusCode merge(std::vector< SG::View* > const&, EventContext const&, T  &) {
  return StatusCode::FAILURE; //  if we are called it means views merging is requested but Type T does not support it (i.e. missing copy c'tor)
  }
};

template<typename T>
struct ViewsMerge {
  const SG::ReadHandleKey<T>& m_inViewKey;
  ViewsMerge( const SG::ReadHandleKey<T>& r ): m_inViewKey(r) {}
  StatusCode  merge( std::vector< SG::View* > const& views,  EventContext const& context, T & output ) {
    return ViewHelper::MergeViewCollection< typename T::value_type >( views, m_inViewKey, context, output );  
  }
};
*/
template<typename T>
StatusCode  NoMerge( std::vector< SG::View* > const&, const SG::ReadHandleKey<T>&,
		     EventContext const&, T &  ) {
  return StatusCode::FAILURE; //  if we are called it means views merging is requested but Type T does not support it (i.e. missing copy c'tor)
}

template<typename T>
StatusCode  ViewsMerge( std::vector< SG::View* > const& views, const SG::ReadHandleKey<T>& inViewKey,
		     EventContext const& context, T & output ) {
  return ViewHelper::MergeViewCollection( views, inViewKey, context, output );  
}


template<typename T, typename Generator, typename ViewsMerger>
StatusCode HLTEDMCreator::createIfMissing( const SG::WriteHandleKeyArray<T>& handles, Generator createAndRecord, ViewsMerger viewsMerger ) const {

  for ( auto writeHandleKey : handles ) {
    SG::ReadHandle<T> readHandle( writeHandleKey.key() );
    if ( readHandle.isValid() ) {
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " already present" );
    } else {
      SG::WriteHandle<T> writeHandle = SG::makeHandle( writeHandleKey );
      CHECK( createAndRecord( writeHandle ) );

      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " was absent, created empty one" );

      if ( not m_viewsKey.key().empty() ) {
	auto viewsHandle = SG::makeHandle( m_viewsKey );
	if ( viewsHandle.isValid() ) {
	  //CHECK( viewsMerger.merge( *viewsHandle, Gaudi::Hive::currentContext(), *writeHandle ) );
	  
	  SG::ReadHandleKey<T> inViewKey( m_inViewKey );
	  //renounce( inViewKey );

	  CHECK( viewsMerger( *viewsHandle, inViewKey, Gaudi::Hive::currentContext(), *writeHandle ) );
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode HLTEDMCreator::createOutput() const {


#define CREATE(__TYPE) \
  CHECK( createIfMissing<__TYPE>( m_##__TYPE, plainGenerator<__TYPE>, ViewsMerge<__TYPE> ) );							
  //  CHECK( createIfMissing<__TYPE>( m_##__TYPE, plainGenerator<__TYPE>, ViewsMerge<__TYPE>( m_##__TYPE##InViews ) ) );

  CREATE( TrigRoiDescriptorCollection );

#undef CREATE

#define CREATE_XAOD(__TYPE, __STORE_TYPE) \
  CHECK( createIfMissing<xAOD::__TYPE>( m_##__TYPE, xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE>, ViewsMerge<xAOD::__TYPE> )  );
  //  CHECK( createIfMissing<xAOD::__TYPE>( m_##__TYPE, xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE>, ViewsMerge<xAOD::__TYPE>( m_##__TYPE##InViews ) )  );

#define CREATE_XAOD_NO_MERGE(__TYPE, __STORE_TYPE) \
  CHECK( createIfMissing<xAOD::__TYPE>( m_##__TYPE, xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE>, NoMerge<xAOD::__TYPE> )  );
  
  CREATE_XAOD_NO_MERGE( TrigCompositeContainer, TrigCompositeAuxContainer );
  CREATE_XAOD( TrigElectronContainer, TrigElectronAuxContainer );
  CREATE_XAOD( TrigPhotonContainer, TrigPhotonAuxContainer );
  CREATE_XAOD( TrigEMClusterContainer, TrigEMClusterAuxContainer );
  CREATE_XAOD( TrigCaloClusterContainer, TrigCaloClusterAuxContainer );
  CREATE_XAOD( TrackParticleContainer, TrackParticleAuxContainer );

#undef CREATE_XAOD
#undef CREATE_XAOD_NO_MERGE
  return StatusCode::SUCCESS;
}
