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


StatusCode TriggerBitsMakerTool::fill( HLT::HLTResultMT& resultToFill ) const {
  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );

  const TrigCompositeUtils::Decision* passRawChains = nullptr;
  for (const TrigCompositeUtils::Decision* d : *chainsHandle) {
    if (d->name() == "HLTPassRaw") {
      passRawChains = d;
      break;
    }
  }

  if (passRawChains == nullptr) {
    ATH_MSG_ERROR("Unable to read in the HLTSummary from the DecisionSummaryMakerAlg");
    return StatusCode::FAILURE;
  }

  for ( TrigCompositeUtils::DecisionID chain: TrigCompositeUtils::decisionIDs( passRawChains ) ) {
    auto mappingIter = m_mapping.find( chain );
    // each chain has to have stream
    if( mappingIter == m_mapping.end() ) { 
      ATH_MSG_ERROR("Each chain has to have the bit/counter associated whereas the " << HLT::Identifier( chain ) << " does not" );
      return StatusCode::FAILURE;
    }
    const int chainBitPosition = mappingIter->second;
    ATH_MSG_DEBUG("Setting bit " << chainBitPosition << " corresponding to chain" << HLT::Identifier(chain));
    ATH_CHECK(resultToFill.addHltBit(chainBitPosition));
  }

  if ( msgLvl( MSG::DEBUG ) ) {
    ATH_MSG_DEBUG("HLT result now has " << resultToFill.getHltBits().num_blocks() << " words with trigger bits:");
    for (const auto& w : resultToFill.getHltBitsAsWords()) ATH_MSG_DEBUG("0x" << MSG::hex << w << MSG::dec);
  }
  return StatusCode::SUCCESS;

}



StatusCode TriggerBitsMakerTool::finalize() {
  return StatusCode::SUCCESS;
}

