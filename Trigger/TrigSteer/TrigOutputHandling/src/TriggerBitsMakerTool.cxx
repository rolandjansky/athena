/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigOutputHandling/TriggerBitsMakerTool.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfHLTData/HLTUtils.h"

#include <algorithm>

TriggerBitsMakerTool::TriggerBitsMakerTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent){}

TriggerBitsMakerTool::~TriggerBitsMakerTool() {}

StatusCode TriggerBitsMakerTool::initialize() {
  ATH_CHECK( m_finalChainDecisions.initialize() );

  TrigConf::JsonFileLoader fileLoader;
  TrigConf::HLTMenu hltmenu;
  ATH_CHECK( fileLoader.loadFile(m_menuJSON, hltmenu) );
  ATH_MSG_INFO("Configuring from " << m_menuJSON << " with " << hltmenu.size() << " chains");

  m_largestBit = 0;
  for(auto& ch : hltmenu) {
    ATH_MSG_DEBUG( "Chain " << ch.name() << " will flip " << ch.counter() <<  " bit" );
    ATH_CHECK(preInsertCheck(ch.name(), ch.counter()));
    ATH_CHECK(hashConsistencyCheck(ch.name(), ch.namehash()));
    m_mapping[ HLT::Identifier( ch.name() ).numeric() ] = ch.counter();
    m_largestBit = std::max(m_largestBit, ch.counter());
  }

  // This block allows extra mappings to be supplied by python, e.g. for testing purposes
  for ( auto& chainAndBit: m_extraChainToBit ) {
    struct { std::string chain; uint32_t bit; } conf { chainAndBit.first, chainAndBit.second };    
    ATH_MSG_DEBUG( "Extra Chain " << conf.chain << " will flip  " << conf.bit <<  " bit" );
    ATH_CHECK(preInsertCheck(conf.chain, conf.bit));
    m_mapping[ HLT::Identifier( conf.chain ).numeric() ] = conf.bit;
    m_largestBit = std::max(m_largestBit, conf.bit);
  }

  return StatusCode::SUCCESS;
}


StatusCode TriggerBitsMakerTool::hashConsistencyCheck(const std::string& chain, const size_t hash) const {
  if (HLT::Identifier( chain ).numeric() != hash) {
    ATH_MSG_ERROR("Inconsistent hashes found for chain:" << chain << ", from Python:" << hash
      << ", from C++:" << HLT::Identifier( chain ).numeric());
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerBitsMakerTool::preInsertCheck(const std::string& chain, const uint32_t bit) const {
  const auto checkIt = std::find_if(
    m_mapping.begin(), m_mapping.end(), [&](const std::pair<TrigCompositeUtils::DecisionID, uint32_t>& m) { return m.second == bit; }
  );
  if (checkIt != m_mapping.end()) {
    ATH_MSG_ERROR( "Multiple chains " << TrigConf::HLTUtils::hash2string(checkIt->first) 
      << " and " << chain << " are both configured with ChainCounter:" << bit);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerBitsMakerTool::fill( HLT::HLTResultMT& resultToFill ) const {

  using namespace TrigCompositeUtils;

  auto chainsHandle = SG::makeHandle( m_finalChainDecisions );
  if (!chainsHandle.isValid()) {
    ATH_MSG_ERROR("Unable to read in the HLTNav_Summary from the DecisionSummaryMakerAlg");
    return StatusCode::FAILURE;
  }

  const Decision* HLTPassRaw = nullptr;
  const Decision* HLTPrescaled = nullptr;
  const Decision* HLTRerun = nullptr;

  DecisionIDContainer passRawIDs; //!< The chains which returned a positive decision
  DecisionIDContainer prescaledIDs; //!< The chains which did not run due to being prescaled out
  DecisionIDContainer rerunIDs; //!< The chains which were activate only in the rerun (not physics decisions)

  // Read the sets of chain IDs
  for (const Decision* decisionObject : *chainsHandle) {
    // Collect all decisions (IDs of passed/prescaled/rerun chains) from named decisionObjects
    if (decisionObject->name() == "HLTPassRaw") {
      HLTPassRaw = decisionObject;
    } else if (decisionObject->name() == "HLTPrescaled") {
      HLTPrescaled = decisionObject;
    } else if (decisionObject->name() == "HLTRerun") {
      HLTRerun = decisionObject;
    }
    if (HLTPassRaw && HLTPrescaled && HLTRerun) {
      break;
    }
  }

  ATH_CHECK(HLTPassRaw != nullptr);
  ATH_CHECK(HLTPrescaled != nullptr);
  ATH_CHECK(HLTRerun != nullptr);

  decisionIDs(HLTPassRaw, passRawIDs);
  decisionIDs(HLTPrescaled, prescaledIDs);
  decisionIDs(HLTRerun, rerunIDs);

  resultToFill.reserveHltBits( m_largestBit );

  for ( TrigCompositeUtils::DecisionID chain: passRawIDs ) {
    ATH_CHECK(setBit(chain, HLTPassRawCategory, resultToFill));
  }

  for ( TrigCompositeUtils::DecisionID chain: prescaledIDs ) {
    ATH_CHECK(setBit(chain, HLTPrescaledCategory, resultToFill));
  }

  for ( TrigCompositeUtils::DecisionID chain: rerunIDs ) {
    ATH_CHECK(setBit(chain, HLTRerunCategory, resultToFill));
  }

  if ( msgLvl( MSG::DEBUG ) ) {
    ATH_MSG_DEBUG("HLT result now has " << resultToFill.getHltBitsAsWords().size() << " words with trigger bits:");
    for (const auto& w : resultToFill.getHltBitsAsWords()) ATH_MSG_DEBUG("0x" << MSG::hex << w << MSG::dec);
  }
  return StatusCode::SUCCESS;

}

StatusCode TriggerBitsMakerTool::setBit(const TrigCompositeUtils::DecisionID chain, const BitCategory category, HLT::HLTResultMT& resultToFill) const {
  auto mappingIter = m_mapping.find( chain );
  // each chain has to have stream
  if( mappingIter == m_mapping.end() ) {
    ATH_MSG_ERROR("Each chain has to have the bit/counter associated whereas the " << HLT::Identifier( chain ) << " does not" );
    return StatusCode::FAILURE;
  }
  const int chainBitPosition = mappingIter->second;
  ATH_MSG_DEBUG("Setting bit " << chainBitPosition << " corresponding to chain" << HLT::Identifier(chain) << " in BitCategory " << category);
  switch (category) {
    case HLTPassRawCategory: ATH_CHECK(resultToFill.addHltPassRawBit(chainBitPosition)); break;
    case HLTPrescaledCategory: ATH_CHECK(resultToFill.addHltPrescaledBit(chainBitPosition)); break;
    case HLTRerunCategory: ATH_CHECK(resultToFill.addHltRerunBit(chainBitPosition)); break;
    default: ATH_MSG_ERROR("Unknown BitCategory"); return StatusCode::FAILURE; break;
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerBitsMakerTool::finalize() {
  return StatusCode::SUCCESS;
}

