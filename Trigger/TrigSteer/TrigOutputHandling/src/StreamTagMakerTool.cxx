/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/HLTIdentifier.h"
#include "StreamTagMakerTool.h"

StreamTagMakerTool::StreamTagMakerTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

StreamTagMakerTool::~StreamTagMakerTool() {
}

StatusCode StreamTagMakerTool::initialize() {  
  // decode mapping
  return StatusCode::SUCCESS;
}

StatusCode StreamTagMakerTool::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode StreamTagMakerTool::fill( HLT::HLTResultMT& resultToFill ) const {
  // obtain chain decisions, 
  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );

  std::vector<eformat::helper::StreamTag> streams;  

  // for each accepted chain lookup the map of chainID -> ST
  for ( TrigCompositeUtils::DecisionID chain: TrigCompositeUtils::decisionIDs( chainsHandle->at(0)) ) {
    auto mappingIter = m_mapping.find( chain );
    // each chain has to have stream
    if( mappingIter == m_mapping.end() ) { 
      ATH_MSG_ERROR("Each chain has to have stream associated whereas the " << HLT::Identifier(chain) << " does not" );
      return StatusCode::FAILURE;
    }
    streams.push_back( mappingIter->second ); // TODO assume nultiple streams per chain
  }

  // push back ST vector to HLTResultMT




  // make sure thy are unique
  std::sort( streams.begin(), streams.end() );
  streams.erase( std::unique( streams.begin(), streams.end() ),  streams.end() );
  
  resultToFill.setStreamTags( streams );
  return StatusCode::SUCCESS;  
}
