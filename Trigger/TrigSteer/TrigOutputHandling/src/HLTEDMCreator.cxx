
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
    CHECK( handles.out.size() == 1 ); // one output collection
    CHECK( handles.in.size() == handles.views.size() ); 
    for ( size_t i = 0; i < handles.in.size(); ++i ) {
      CHECK( handles.in.at(i).key() == handles.out.at(i).key() );
    }

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

  INIT_XAOD( L2StandAloneMuonContainer );
  INIT_XAOD( L2CombinedMuonContainer );
  INIT_XAOD( L2IsoMuonContainer );
  INIT_XAOD( MuonContainer );

#undef INIT
#undef INIT_XAOD
  
  return StatusCode::SUCCESS;
}

template<class T>
struct plainGenerator {
  std::unique_ptr<T> data;
  void create() {
    data = std::make_unique<T>();
  }
  StatusCode record( SG::WriteHandle<T>& h ) {    
    return h.record( std::move( data ) );
  } 
};

template<class T, class STORE>
struct xAODGenerator {
  std::unique_ptr<T> data;
  std::unique_ptr<STORE> store;

  void create() {
    data  = std::make_unique<T>();
    store = std::make_unique<STORE>();
    data->setStore( store.get() );      
  }

  StatusCode record ( SG::WriteHandle<T>& h ) {
    return h.record( std::move( data ), std::move( store ) );
  } 
};

template<typename T>
StatusCode  HLTEDMCreator::noMerge( ViewContainer const&, const SG::ReadHandleKey<T>&,
				    EventContext const&, T & ) const {
  //  if we are called it means views merging is requested but Type T does not support it (i.e. missing copy c'tor)
  return StatusCode::FAILURE;

}

template<typename T>
StatusCode  HLTEDMCreator::viewsMerge( ViewContainer const& views, const SG::ReadHandleKey<T>& inViewKey,
				       EventContext const& context, T & output ) const {
  
  typedef typename T::base_value_type type_in_container;
  StoreGateSvc* sg = evtStore().operator->(); // why the get() method is returing a null ptr is a puzzle, we have to use this ugly call to operator instead of it
  CHECK( sg != nullptr );
  ViewHelper::ViewMerger merger( sg, msg() );
  merger.mergeViewCollection<type_in_container>( views, inViewKey, context, output );

  return StatusCode::SUCCESS;
}

 
StatusCode HLTEDMCreator::fixLinks( const ConstHandlesGroup< xAOD::TrigCompositeContainer >& handles ) const {

  // Make a list of the collections we're going to mess with
  std::set< std::string > remappedCollections;
  for ( auto writeHandleKey : handles.out ) {

    remappedCollections.insert( writeHandleKey.key() );
  }

  // Do the remapping
  for ( auto writeHandleKey : handles.out ) {

    SG::ReadHandle<xAOD::TrigCompositeContainer> readHandle( writeHandleKey.key() );

    if ( readHandle.isValid() ) {

      // Create a container for the remapped TCs
      xAODGenerator<xAOD::TrigCompositeContainer, xAOD::TrigCompositeAuxContainer> output;
      output.create();

      // Examine each input TC
      for ( auto inputDecision : *( readHandle.cptr() ) ) {

        // Clone the TC (xAOD-style copy)
        xAOD::TrigComposite * outputDecision = new xAOD::TrigComposite();
        output.data->push_back( outputDecision );
        *outputDecision = *inputDecision;

        // Retrieve the link information for remapping
        SG::AuxElement::Accessor< std::vector< uint32_t > > keyAccessor( "linkColKeys" );
        SG::AuxElement::Accessor< std::vector< uint16_t > > offsetAccessor( "linkColIndices" );
        std::vector< uint32_t > remappedKeys = keyAccessor( *outputDecision );
        std::vector< uint16_t > remappedOffsets = offsetAccessor( *outputDecision );

        // Search the linked collections for remapping
        unsigned int const collectionTotal = inputDecision->linkColNames().size();
        for ( unsigned int collectionIndex = 0; collectionIndex < collectionTotal; ++collectionIndex ) {

          // Load identifiers
          std::string const collectionName = inputDecision->linkColNames()[ collectionIndex ];
          uint32_t const collectionKey = inputDecision->linkColKeys()[ collectionIndex ];
          std::string const keyString = *( evtStore()->keyToString( collectionKey ) );
          uint32_t const collectionClid = inputDecision->linkColClids()[ collectionIndex ];
          uint16_t const collectionOffset = inputDecision->linkColIndices()[ collectionIndex ];

          // Check for remapping in a merge
          uint32_t newKey = 0;
          size_t newOffset = 0;
          bool isRemapped = evtStore()->tryELRemap( collectionKey, collectionOffset, newKey, newOffset);
          if ( isRemapped ) {

            ATH_MSG_DEBUG( "Remap link from " << *( evtStore()->keyToString( collectionKey ) ) << " to " << *( evtStore()->keyToString( newKey ) ) );
            remappedKeys[ collectionIndex ] = newKey;
            remappedOffsets[ collectionIndex ] = newOffset;
          }

          // If the link is to a collection modified in this method, will also need to be remapped
          // WARNING: untested
          if ( remappedCollections.find( keyString ) != remappedCollections.end() ) {

            ATH_MSG_DEBUG( "Remap link to collection: " << keyString << " -> remap_" << keyString );
            remappedKeys[ collectionIndex ] = evtStore()->stringToKey( "remap_" + keyString, collectionClid );
          }
        }

        // Save the remaps
        keyAccessor( *outputDecision ) = remappedKeys;
        offsetAccessor( *outputDecision ) = remappedOffsets;
      }

      // Store the remapped TCs
      SG::WriteHandle<xAOD::TrigCompositeContainer> writeHandle( m_remapKey + writeHandleKey.key() );
      CHECK(output.record( writeHandle ));
    }
  }

  return StatusCode::SUCCESS;
}

template<typename T, typename G, typename M>
StatusCode HLTEDMCreator::createIfMissing( const EventContext& context, const ConstHandlesGroup<T>& handles, G& generator, M merger ) const {

  for ( auto writeHandleKey : handles.out ) {
    
    SG::ReadHandle<T> readHandle( writeHandleKey.key() );
    ATH_MSG_DEBUG( "Checking " << writeHandleKey.key() );
    
    if ( readHandle.isValid() ) {
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " already present" );
    } else {      
      ATH_MSG_DEBUG( "The " << writeHandleKey.key() << " was absent, creating it" );
      generator.create();      
      if ( handles.views.size() != 0 ) {

	ATH_MSG_DEBUG("Will be merging from " << handles.views.size() << " view containers into that output");
	auto viewCollKeyIter = handles.views.begin();
	auto inViewCollKeyIter = handles.in.begin();
	
	for ( ; viewCollKeyIter != handles.views.end(); ++viewCollKeyIter, ++inViewCollKeyIter ) {
	  // get the views handle
	  ATH_MSG_DEBUG("Will be merging from " << viewCollKeyIter->key() << " view container using key " << inViewCollKeyIter->key() );
	  auto viewsHandle = SG::makeHandle( *viewCollKeyIter );
	  if ( viewsHandle.isValid() ) {	    
	    CHECK( (this->*merger)( *viewsHandle, *inViewCollKeyIter , context, *generator.data.get() ) );
	  }
	}      
      }
      auto writeHandle = SG::makeHandle( writeHandleKey );
      CHECK( generator.record( writeHandle ) );
    }     
  }
  return StatusCode::SUCCESS;
}



StatusCode HLTEDMCreator::createOutput(const EventContext& context) const {

#define CREATE(__TYPE) \
    {									\
      plainGenerator<__TYPE> generator;					\
      CHECK( createIfMissing<__TYPE>( context, ConstHandlesGroup<__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), generator, &HLTEDMCreator::noMerge<__TYPE> ) ); \
    }

  CREATE( TrigRoiDescriptorCollection )

#undef CREATE

#define CREATE_XAOD(__TYPE, __STORE_TYPE) \
  { \
    xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE> generator; \
    CHECK( createIfMissing<xAOD::__TYPE>( context, ConstHandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), generator, &HLTEDMCreator::viewsMerge<xAOD::__TYPE> )  ); \
  }


#define CREATE_XAOD_NO_MERGE(__TYPE, __STORE_TYPE)			\
  { \
    xAODGenerator<xAOD::__TYPE, xAOD::__STORE_TYPE> generator; \
    CHECK( createIfMissing<xAOD::__TYPE>( context, ConstHandlesGroup<xAOD::__TYPE>( m_##__TYPE, m_##__TYPE##InViews, m_##__TYPE##Views ), generator, &HLTEDMCreator::noMerge<xAOD::__TYPE> )  ); \
  }
  
  CREATE_XAOD_NO_MERGE( TrigCompositeContainer, TrigCompositeAuxContainer )
  CREATE_XAOD( TrigElectronContainer, TrigElectronAuxContainer )
  CREATE_XAOD( TrigPhotonContainer, TrigPhotonAuxContainer )
  CREATE_XAOD( TrigEMClusterContainer, TrigEMClusterAuxContainer )
  CREATE_XAOD( TrigCaloClusterContainer, TrigCaloClusterAuxContainer )
  CREATE_XAOD( TrackParticleContainer, TrackParticleAuxContainer )

  CREATE_XAOD( L2StandAloneMuonContainer, L2StandAloneMuonAuxContainer );
  CREATE_XAOD( L2CombinedMuonContainer, L2CombinedMuonAuxContainer );
  CREATE_XAOD( L2IsoMuonContainer, L2IsoMuonAuxContainer );
  CREATE_XAOD( MuonContainer, MuonAuxContainer );

  // After view collections are merged, need to update collection links
  if ( m_fixLinks )
    ATH_CHECK( fixLinks( ConstHandlesGroup<xAOD::TrigCompositeContainer>( m_TrigCompositeContainer, m_TrigCompositeContainerInViews, m_TrigCompositeContainerViews ) ) );

#undef CREATE_XAOD
#undef CREATE_XAOD_NO_MERGE
  return StatusCode::SUCCESS;
}
