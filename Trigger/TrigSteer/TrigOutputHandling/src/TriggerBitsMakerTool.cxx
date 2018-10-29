/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigOutputHandling/TriggerBitsMakerTool.h"

TriggerBitsMakerTool::TriggerBitsMakerTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent){}

TriggerBitsMakerTool::~TriggerBitsMakerTool() {}

StatusCode TriggerBitsMakerTool::initialize() {
  ATH_CHECK( m_finalChainDecisions.initialize() );

  for ( auto& chainAndBit: m_chainToStreamProperty ) {
    struct { std::string chain; int bit; } conf { chainAndBit.first, chainAndBit.second };    
    ATH_MSG_DEBUG( "Chain " << conf.chain << " will flip  " << conf.bit <<  " bit" );
    m_mapping[ HLT::Identifier( conf.chain ) ] = conf.bit;
    
  }

  return StatusCode::SUCCESS;
}


StatusCode TriggerBitsMakerTool::fill( HLTResultMT& resultToFill ) const {
  std::vector<uint32_t> bits;
  ATH_CHECK( fill( bits ) );
  resultToFill.setHltBits( bits );
  return StatusCode::SUCCESS;
}



StatusCode TriggerBitsMakerTool::fill( std::vector<uint32_t>& bits ) const {
  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );

  for ( TrigCompositeUtils::DecisionID chain: TrigCompositeUtils::decisionIDs( chainsHandle->at( 0 )) ) {
    auto mappingIter = m_mapping.find( chain );
    // each chain has to have stream
    if( mappingIter == m_mapping.end() ) { 
      ATH_MSG_ERROR("Each chain has to have the bit/counter associated whereas the " << HLT::Identifier( chain ) << " does not" );
      return StatusCode::FAILURE;
    }
    const int chainBitPosition = mappingIter->second;
    // obtain bit position
    const int word = chainBitPosition / ( sizeof(uint32_t) * 8 );
    const int mask = 1 << (chainBitPosition % ( sizeof(uint32_t) * 8 ) );
    bits.resize( word+1 ); // assure space
    bits[word] |= mask;    
  }


  if ( msgLvl( MSG::DEBUG ) ) {
    ATH_MSG_DEBUG("Prepared " << bits.size() << " words with trigger bits");
    for ( auto w: bits )
      ATH_MSG_DEBUG("0x" << MSG::hex << w );
  }
  return StatusCode::SUCCESS;

}



StatusCode TriggerBitsMakerTool::finalize() {
  return StatusCode::SUCCESS;
}

