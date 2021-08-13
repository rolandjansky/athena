/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "RoIsUnpackingToolBase.h"
#include "TrigConfL1Data/TriggerItem.h"

RoIsUnpackingToolBase::RoIsUnpackingToolBase(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : base_class(type, name, parent) {}

StatusCode RoIsUnpackingToolBase::initialize() {
  if ( !m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
  ATH_CHECK( m_decisionsKey.initialize() );
  ATH_CHECK( m_decisionsKeyProbe.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_trigRoIsKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_l1MenuKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode RoIsUnpackingToolBase::getL1Thresholds(const TrigConf::L1Menu& l1Menu,
                                                  const std::string& thrType,
                                                  std::optional<ThrVecRef>& thrVec) const {
  try {
    thrVec = ThrVecRef(l1Menu.thresholds(thrType));
  }
  catch (const std::exception& ex) {
    ATH_MSG_ERROR("Failed to retrieve " << thrType << " thresholds from L1 menu. Exception:" << ex.what());
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

std::string RoIsUnpackingToolBase::getProbeThresholdName(const std::string& thresholdName) {
  return "PROBE" + thresholdName;
}

StatusCode RoIsUnpackingToolBase::decodeMapping( std::function< bool(const std::string&)> filter ) {

  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );

  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    const HLT::Identifier chainIdentifier(chain.name());
    const std::vector<std::string> thresholds{ chain.l1thresholds() };
    const std::vector<size_t> legMultiplicities{ chain.legMultiplicities() };
    if (thresholds.size() != legMultiplicities.size()) {
      ATH_MSG_ERROR("Encountered a chain " << chain.name() << " with " << legMultiplicities.size() << " legs but only " << 
        thresholds.size() << " thresolds. These should be the same.");
      return StatusCode::FAILURE;
    }
    size_t counter = 0;
    for ( const std::string& th: thresholds ) {
      if ( filter(th) ) {
        const HLT::Identifier thresholIdentifier(th);
        m_thresholdToChainMapping[ thresholIdentifier ].push_back( chainIdentifier );
        ATH_MSG_DEBUG( "Associating " << chainIdentifier << " with threshold " << th );
        if ( thresholds.size() > 1 ) {
          HLT::Identifier legIdentifier = TrigCompositeUtils::createLegName(chainIdentifier, counter);
          m_thresholdToChainMapping[ thresholIdentifier ].push_back( legIdentifier );
          m_legToChainMapping.insert( std::make_pair( legIdentifier,  chainIdentifier ) );
          ATH_MSG_INFO( "Associating additional chain leg " << legIdentifier << " with threshold " << thresholIdentifier );
        }
      }
      ++counter;
    }
  }
  return StatusCode::SUCCESS;
}

void RoIsUnpackingToolBase::addChainsToDecision( HLT::Identifier thresholdId,
                                                 TrigCompositeUtils::Decision* d,
                                                 const HLT::IDSet& activeChains ) const {
  auto chains = m_thresholdToChainMapping.find( thresholdId );
  if ( chains == m_thresholdToChainMapping.end() ) {
    ATH_MSG_DEBUG("Threshold not known " << thresholdId);
    return;
  }

  TrigCompositeUtils::DecisionIDContainer ids;
  for ( auto chainId: chains->second ) {
    if ( activeChains.find(chainId) != activeChains.end() ) {
      ids.insert( chainId.numeric() );
      ATH_MSG_DEBUG( "Added " << chainId << " to the RoI/threshold decision " << thresholdId );
    } else {    // maybe it is a leg?
      auto legIterator = m_legToChainMapping.find( chainId );
      if ( legIterator != m_legToChainMapping.end() ) { // this is a leg we care about, need to check if respective chain was active, and activate
        if ( activeChains.find( legIterator->second ) != activeChains.end() ) {
          ids.insert( chainId.numeric() );
          ATH_MSG_DEBUG( "Added " << chainId << " to the RoI/threshold decision " << thresholdId );
        }
      }
    }
  }
  TrigCompositeUtils::insertDecisionIDs(ids, d);
  ATH_MSG_DEBUG( "Number of decisions in this RoI after adding chains using threshold " << thresholdId << " " << TrigCompositeUtils::decisionIDs( d ).size() );
}
