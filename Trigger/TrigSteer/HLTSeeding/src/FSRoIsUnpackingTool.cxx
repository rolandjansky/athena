/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FSRoIsUnpackingTool.h"
#include "xAODTrigger/TrigCompositeContainer.h"


FSRoIsUnpackingTool::FSRoIsUnpackingTool( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
  : RoIsUnpackingToolBase(type, name, parent) {}

StatusCode FSRoIsUnpackingTool::initialize()
{
  ATH_CHECK(RoIsUnpackingToolBase::initialize());
  return StatusCode::SUCCESS;
}

StatusCode FSRoIsUnpackingTool::start() {
  ATH_CHECK( decodeMapping( [](const std::string& name ){ return
	  name.find("FS") == 0 or
	  name == ""; } ) );

  m_allFSChains.clear();
  
  for ( auto thresholdToChain: m_thresholdToChainMapping ) {
    m_allFSChains.insert( thresholdToChain.second.begin(), thresholdToChain.second.end() );
  }

  for ( auto id: m_allFSChains ) {
    ATH_MSG_DEBUG( "FS Chain " << id  );
  }

  return StatusCode::SUCCESS;
}


StatusCode FSRoIsUnpackingTool::unpack( const EventContext& ctx,
					const ROIB::RoIBResult& /*roib*/,
					const HLT::IDSet& activeChains ) const {
  using namespace TrigCompositeUtils;
  SG::WriteHandle<DecisionContainer> handle = createAndStore(m_decisionsKey, ctx );
  auto *decisionOutput = handle.ptr();

  HLT::IDSet activeFSchains;
  // see if any chain we care of is active
  std::set_intersection(activeChains.begin(), activeChains.end(),
  			m_allFSChains.begin(), m_allFSChains.end(),
			std::inserter(activeFSchains, activeFSchains.end() ) );

  auto *decision  = TrigCompositeUtils::newDecisionIn( decisionOutput, hltSeedingNodeName() ); // This hltSeedingNodeName() denotes an initial node with no parents
  addChainsToDecision( HLT::Identifier( "FSNOSEED" ), decision, activeChains );

  ATH_MSG_DEBUG("Unpacking FS RoI for " << activeFSchains.size() << " chains: " << [&](){ 
      TrigCompositeUtils::DecisionIDContainer ids; 
      TrigCompositeUtils::decisionIDs( decision, ids ); 
      return std::vector<TrigCompositeUtils::DecisionID>( ids.begin(), ids.end() ); }() );

  auto roiHandle = SG::makeHandle( m_trigRoIsKey, ctx );
  ATH_CHECK(roiHandle.record( std::make_unique<TrigRoiDescriptorCollection>() ));
  roiHandle->push_back( std::make_unique<TrigRoiDescriptor>(true) ); // true == FS

  ATH_MSG_DEBUG("Linking to FS RoI descriptor");
  decision->setObjectLink( initialRoIString(), ElementLink<TrigRoiDescriptorCollection>( m_trigRoIsKey.key(), 0 ) );  

  return StatusCode::SUCCESS;
}
