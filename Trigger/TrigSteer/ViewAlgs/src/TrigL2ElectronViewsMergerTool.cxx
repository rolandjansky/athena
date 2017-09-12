#include "TrigL2ElectronViewsMergerTool.h"


TrigL2ElectronViewsMergerTool::TrigL2ElectronViewsMergerTool( const std::string& type, 
							      const std::string& name, 
							      const IInterface* parent ) 
  : AthAlgTool( type, name, parent ) {}

StatusCode TrigL2ElectronViewsMergerTool::initialize() {
  CHECK( m_mergedElectronsKey.initialize() );
  renounce( m_inViewElectronsKey ); // this is an input no to be seen by scheduler
  CHECK( m_inViewElectronsKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigL2ElectronViewsMergerTool::merge( const EventContext& context, const std::vector<SG::View*>& views ) const {

  auto output = std::make_unique< ConstDataVector< xAOD::TrigElectronContainer > >();
  output->clear( SG::VIEW_ELEMENTS );

  for ( auto view : views ) {
    // make input electrons 
    EventContext viewContext( context );
    viewContext.setExtension(  Atlas::ExtendedEventContext( view ) );
    auto electronsInViewHandle = SG::makeHandle( m_inViewElectronsKey, viewContext );
    CHECK ( electronsInViewHandle.isValid() );
    ATH_MSG_DEBUG( "Found: "<< electronsInViewHandle->size() << "electrons in view" );
    for ( auto electron: *electronsInViewHandle ) {
      output->push_back( electron );
    }
  }
  ATH_MSG_DEBUG( "Output electrons container size: " << output->size() );
  
  auto outputHandle = SG::makeHandle( m_mergedElectronsKey, context );
  CHECK( outputHandle.record( std::move( output ) ) );  
  return StatusCode::SUCCESS;
}
