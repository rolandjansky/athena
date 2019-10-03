/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "RoIsUnpackingToolBase.h"
#include "TrigConfL1Data/TriggerItem.h"

RoIsUnpackingToolBase::RoIsUnpackingToolBase(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : base_class(type, name, parent)
{
}


StatusCode RoIsUnpackingToolBase::initialize()
{
  if ( !m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
  ATH_CHECK( m_decisionsKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode RoIsUnpackingToolBase::decodeMapping( std::function< bool(const std::string&)> filter ) {

  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );

  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    const std::vector<std::string> thresholds{ chain.l1thresholds() };
    int prefix = -1;
    for ( const std::string& th: thresholds ) {
      prefix++;
      if ( filter(th) ) {
	m_thresholdToChainMapping[ HLT::Identifier( th ) ].push_back(  HLT::Identifier( chain.name() ) );
	ATH_MSG_DEBUG( "Associating " << chain.name() << " with threshold " << th );
	if ( thresholds.size() > 1 ) {
	  std::string legName = "leg000_" + chain.name();
	  const std::string ps = std::to_string(prefix);
	  legName.replace( 6-ps.size(), ps.size(), ps );
	  m_thresholdToChainMapping[ HLT::Identifier(th) ].push_back( HLT::Identifier( legName ) );
	  ATH_MSG_INFO( "Associating additional chain leg " << legName << " with threshold " << th );
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}
StatusCode RoIsUnpackingToolBase::decodeMapping( std::function< bool(const TrigConf::TriggerThreshold*)> filter, const TrigConf::ItemContainer& l1Items, const IRoIsUnpackingTool::SeedingMap& seeding ) {
  for ( auto chainItemPair: seeding) {
    std::string chainName = chainItemPair.first;
    std::string itemName = chainItemPair.second;
    auto itemsIterator = l1Items.get<TrigConf::tag_name_hash>().find(itemName);

    if ( itemsIterator != l1Items.get<TrigConf::tag_name_hash>().end() ) {

      const TrigConf::TriggerItem* item = *itemsIterator;
      const TrigConf::TriggerItemNode* node = item->topNode();
      std::vector<TrigConf::TriggerThreshold*> itemThresholds;
      node->getAllThresholds(itemThresholds);
      const int thresholdCount = itemThresholds.size();
      ATH_MSG_DEBUG( "Item " << item->name() << " with thresholds " <<  thresholdCount);
      int prefix = -1;
      for ( const TrigConf::TriggerThreshold* th: itemThresholds ) {
	prefix++;
	if ( filter(th) ) {
	  m_thresholdToChainMapping[HLT::Identifier(th->name())].push_back( HLT::Identifier(chainName) );
	  ATH_MSG_DEBUG( "Associating " << chainName << " with threshold " << th->name() );
	  if ( thresholdCount > 1 ) {
	    std::string legName = "000_" + chainName;
	    const std::string ps = std::to_string(prefix);
	    legName.replace(3-ps.size(), ps.size(), ps );
	    m_thresholdToChainMapping[HLT::Identifier(th->name())].push_back( HLT::Identifier(legName) );
	    ATH_MSG_INFO( "Associating " << legName << " with threshold " << th->name() );
	  }
	}
      }
    } else {
      // we may wish to change that to ERROR at some point
      ATH_MSG_WARNING( "Could not find item: " << itemName << " that is used in seeding");
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
      ATH_MSG_DEBUG( "Added chain to the RoI/threshold decision " << chainId );
    }
  }
  TrigCompositeUtils::insertDecisionIDs(ids, d);
  ATH_MSG_DEBUG( "Number of decisions per RoI after adding chains using threshold " << thresholdId << " " << TrigCompositeUtils::decisionIDs( d ).size() );
}

StatusCode RoIsUnpackingToolBase::copyThresholds( const std::vector<TrigConf::TriggerThreshold*>& src, std::vector<TrigConf::TriggerThreshold*>& dest ) const {
  for ( auto th: src ) {
    if ( th == nullptr ) {
      ATH_MSG_INFO( "Nullptr TrigConf::TriggerThreshold" );
    } else {
      ATH_MSG_INFO( "Found threshold in the configuration: " << th->name() << " of ID: " << HLT::Identifier( th->name() ).numeric() );
      dest.push_back( th );
    }
  }

  return StatusCode::SUCCESS;
}
