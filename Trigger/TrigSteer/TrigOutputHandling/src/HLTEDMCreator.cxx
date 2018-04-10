
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
StatusCode HLTEDMCreator::initHandles( const HandlesGroup<T>&  handles ) {
  CHECK( handles.in.initialize() );
  renounceArray( handles.in );
  CHECK( handles.out.initialize() );
  CHECK( handles.views.initialize() );
  renounceArray( handles.views );
  // the case w/o reading from views, both views handles and collection in views shoudl be empty
  if ( handles.views.size() == 0 ) {
    CHECK( handles.in.size() == 0 );
  } else {
    // the case with views, we want to store from many views into a single output container
    CHECK( handles.in.size() == handles.views.size() );
    CHECK( handles.out.size() == 1 );
  }
  return StatusCode::SUCCESS;
}


StatusCode HLTEDMCreator::initialize()
{  
#define INIT(__TYPE) \
  CHECK( initHandles( HandlesGroup<__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ) ) );

#define INIT_XAOD(__TYPE) \
  CHECK( initHandles( HandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ) ) );

  INIT( TrigRoiDescriptorCollection );
  INIT_XAOD( TrigCompositeContainer );
  INIT_XAOD( TrigEMClusterContainer );
  INIT_XAOD( TrigCaloClusterContainer );
  INIT_XAOD( TrigElectronContainer ); 
  INIT_XAOD( TrigPhotonContainer );
  INIT_XAOD( TrackParticleContainer );

#undef INIT
#undef INIT_XAOD
  
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

template<typename T>
StatusCode  noMerge( std::vector< SG::View* > const&, const SG::ReadHandleKey<T>&,
				    EventContext const&, T &  ) {
  //  if we are called it means views merging is requested but Type T does not support it (i.e. missing copy c'tor)
  return StatusCode::FAILURE;
}

template<typename T>
StatusCode  viewsMerge( std::vector< SG::View* > const& views, const SG::ReadHandleKey<T>& inViewKey,
				       EventContext const& context, T & output ) {
  return ViewHelper::MergeViewCollection( views, inViewKey, context, output );  
}


template<typename T, typename G, typename M>
StatusCode HLTEDMCreator::createIfMissing( const  ConstHandlesGroup<T>& handles, G createAndRecord, M merger) const {

  for ( auto writeHandleKey : handles.out ) {
    SG::ReadHandle<T> readHandle( writeHandleKey.key() );
    if ( readHandle.isValid() ) {
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " already present" );
    } else {
      SG::WriteHandle<T> writeHandle = SG::makeHandle( writeHandleKey );
      CHECK( createAndRecord( writeHandle ) );

      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " was absent, created empty one" );

      if ( handles.views.size() != 0 ) {
	ATH_MSG_DEBUG("Will be merging from " << handles.views.size() << " view containers into that output");
	auto viewCollKeyIter = handles.views.begin();
	auto inViewCollKeyIter = handles.in.begin();
	for ( ; viewCollKeyIter != handles.views.end(); ++viewCollKeyIter, ++inViewCollKeyIter ) {
	  // get the views handle
	  ATH_MSG_DEBUG("Will be merging from " << viewCollKeyIter->key() << " view container using key " << inViewCollKeyIter->key() );
	  auto viewsHandle = SG::makeHandle( *viewCollKeyIter );
	  if ( viewsHandle.isValid() ) {	    
	    CHECK( merger( *viewsHandle, *inViewCollKeyIter , Gaudi::Hive::currentContext(), *writeHandle ) );
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}



StatusCode HLTEDMCreator::createOutput() const {

#define CREATE(__TYPE) \
  CHECK( createIfMissing<__TYPE>( ConstHandlesGroup<__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), plainGenerator<__TYPE>, viewsMerge<__TYPE> ) )							

  CREATE( TrigRoiDescriptorCollection );

#undef CREATE

#define CREATE_XAOD(__TYPE, __STORE_TYPE) \
  CHECK( createIfMissing<xAOD::__TYPE>( ConstHandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE>, viewsMerge<xAOD::__TYPE> )  )


#define CREATE_XAOD_NO_MERGE(__TYPE, __STORE_TYPE)			\
  CHECK( createIfMissing<xAOD::__TYPE>( ConstHandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE>, noMerge<xAOD::__TYPE> )  )
  
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
