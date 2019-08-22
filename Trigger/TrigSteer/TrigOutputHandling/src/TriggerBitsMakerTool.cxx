/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigOutputHandling/TriggerBitsMakerTool.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/HLTMenu.h"

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
    m_mapping[ HLT::Identifier( ch.name() ) ] = ch.counter();
    m_largestBit = std::max(m_largestBit, ch.counter());
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

