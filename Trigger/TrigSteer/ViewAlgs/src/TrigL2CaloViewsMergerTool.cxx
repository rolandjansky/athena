#include "TrigL2CaloViewsMergerTool.h"


TrigL2CaloViewsMergerTool::TrigL2CaloViewsMergerTool( const std::string& type, 
							      const std::string& name, 
							      const IInterface* parent ) 
  : AthAlgTool( type, name, parent ) {}

StatusCode TrigL2CaloViewsMergerTool::initialize() {
  CHECK( m_mergedClustersKey.initialize() );
  renounce( m_inViewClustersKey ); // this is an input no to be seen by scheduler
  CHECK( m_inViewClustersKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigL2CaloViewsMergerTool::merge( const EventContext& context, const std::vector<SG::View*>& views ) const {

  auto output = std::make_unique< ConstDataVector< xAOD::TrigEMClusterContainer > >();
  output->clear( SG::VIEW_ELEMENTS );

  for ( auto view : views ) {
    // make input electrons 
    EventContext viewContext( context );
    viewContext.setExtension(  Atlas::ExtendedEventContext( view ) );
    auto clustersInViewHandle = SG::makeHandle( m_inViewClustersKey, viewContext );
    CHECK ( clustersInViewHandle.isValid() );
    ATH_MSG_DEBUG( "Found: "<< clustersInViewHandle->size() << "clusters in view" );
    for ( auto electron: *clustersInViewHandle ) {
      output->push_back( electron );
    }
  }
  ATH_MSG_DEBUG( "Output clusters container size: " << output->size() );
  
  auto outputHandle = SG::makeHandle( m_mergedClustersKey, context );
  CHECK( outputHandle.record( std::move( output ) ) );  
  return StatusCode::SUCCESS;
}
