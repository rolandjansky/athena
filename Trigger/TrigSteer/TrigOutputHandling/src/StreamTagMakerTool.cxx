/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HLTIdentifier.h"
#include "StreamTagMakerTool.h"

StreamTagMakerTool::StreamTagMakerTool( const std::string& type, const std::string& name, const IInterface* parent ):
  base_class( type, name, parent ) {}

StreamTagMakerTool::~StreamTagMakerTool() {}

StatusCode StreamTagMakerTool::initialize() {  
  // decode mapping
  ATH_CHECK( m_finalChainDecisions.initialize() );
  for ( auto& chainAndStream: m_chainToStreamProperty ) {
    struct { std::string chain, stream; } conf { chainAndStream.first, chainAndStream.second };    
    ATH_MSG_DEBUG( "Chain " << conf.chain << " accepts events to stream " << conf.stream );
    m_mapping[ HLT::Identifier( conf.chain ) ] = eformat::helper::StreamTag( conf.stream, "physics", true );
    
  }

  return StatusCode::SUCCESS;
}

StatusCode StreamTagMakerTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode StreamTagMakerTool::fill( HLTResultMT& resultToFill ) const {
  // obtain chain decisions, 
  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );

  std::vector<eformat::helper::StreamTag> streams;  

  // for each accepted chain lookup the map of chainID -> ST
  for ( TrigCompositeUtils::DecisionID chain: TrigCompositeUtils::decisionIDs( chainsHandle->at( 0 )) ) {
    auto mappingIter = m_mapping.find( chain );
    // each chain has to have stream
    if( mappingIter == m_mapping.end() ) { 
      ATH_MSG_ERROR("Each chain has to have stream associated whereas the " << HLT::Identifier( chain ) << " does not" );
      return StatusCode::FAILURE;
    }
    streams.push_back( mappingIter->second ); // TODO assume nultiple streams per chain
  }

  // push back ST vector to HLTResultMT
  // make sure ST vector contains only unique content
  std::sort( streams.begin(), streams.end() );
  auto endUnique = std::unique( streams.begin(), streams.end() );
  streams.erase( endUnique,  streams.end() );
  
  resultToFill.setStreamTags( streams );
  ATH_MSG_DEBUG("Number of streams for event " <<  streams.size() );
  return StatusCode::SUCCESS;  
}
