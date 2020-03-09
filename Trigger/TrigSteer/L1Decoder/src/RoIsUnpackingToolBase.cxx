/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    const HLT::Identifier chainIdentifier(chain.name());
    const std::vector<std::string> thresholds{ chain.l1thresholds() };
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
      ATH_MSG_DEBUG( "Added chain to the RoI/threshold decision " << chainId );
    } else {    // maybe it is a leg?
      auto legIterator = m_legToChainMapping.find( chainId );
      if ( legIterator != m_legToChainMapping.end() ) { // this is a leg we care about, need to check if respective chain was active, and activate
	if ( activeChains.find( legIterator->second ) != activeChains.end() ) {
	  ids.insert( chainId.numeric() );
	  ATH_MSG_DEBUG( "Added chain leg to the RoI/threshold decision " << chainId );
	}
      }
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
