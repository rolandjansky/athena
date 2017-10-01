/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "RoIsUnpackingToolBase.h"


RoIsUnpackingToolBase::RoIsUnpackingToolBase(const std::string& type, 
                                             const std::string& name, 
                                             const IInterface* parent) 
  : base_class(type, name, parent)
{
}


StatusCode RoIsUnpackingToolBase::initialize() 
{
  if ( !m_monTool.empty() ) CHECK( m_monTool.retrieve() );
  CHECK( m_decisionsKey.initialize() );

  if ( decodeMapping().isFailure() ) {
    ATH_MSG_ERROR( "Failed to decode threshold to chains mapping, is the format th : chain?" );
    return StatusCode::FAILURE;
  }
  if ( m_thresholdToChainMapping.empty() ) {
    ATH_MSG_WARNING( "No chains configured in ThresholdToChainMapping: " << m_thresholdToChainProperty );
  }
  for ( auto el: m_thresholdToChainMapping ) {
    ATH_MSG_DEBUG( "Threshold " << el.first << " mapped to chains " << el.second );
  }

  return StatusCode::SUCCESS;
}


StatusCode RoIsUnpackingToolBase::decodeMapping() {
  std::istringstream input;
  for ( auto entry: m_thresholdToChainProperty ) {
    input.clear();
    input.str(entry);
    std::string thresholdName;
    char delim;
    std::string chainName;
    input >> thresholdName >> delim >> chainName;
    if ( delim != ':' ) {
      return StatusCode::FAILURE;
    }
    m_thresholdToChainMapping[HLT::Identifier(thresholdName)].push_back(HLT::Identifier(chainName));
  }
  return StatusCode::SUCCESS;
}

void RoIsUnpackingToolBase::addChainsToDecision( HLT::Identifier thresholdId,
                                                 TrigCompositeUtils::Decision* d,
                                                 const HLT::IDSet& activeChains ) const {
  auto chains = m_thresholdToChainMapping.find( thresholdId );
  if ( chains == m_thresholdToChainMapping.end() )
    return;
  for ( auto chainId: chains->second ) {
    if ( activeChains.find(chainId) != activeChains.end() )
      TrigCompositeUtils::addDecisionID( chainId.numeric(), d );
  }
}





