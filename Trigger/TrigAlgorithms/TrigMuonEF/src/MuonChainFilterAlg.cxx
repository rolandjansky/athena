/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonChainFilterAlg.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"


MuonChainFilterAlg::MuonChainFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator) {}


StatusCode MuonChainFilterAlg::initialize() {

  ATH_CHECK( m_inputDecisionKeys.initialize() );
  ATH_CHECK( m_muCombKey.initialize(m_writeL2muComb) );
  ATH_CHECK( m_muFastKey.initialize(m_writeL2muFast) );

  // convert chain names into hashes
  if (!m_filterChains.empty()) {
    for (size_t i = 0; i < m_filterChains.size(); ++i) {
      const auto id = HLT::Identifier(m_filterChains[i]);
      m_filterChainIDs.insert(id.numeric());
      ATH_MSG_DEBUG( i << " filter chain >> " << m_filterChains[i] );
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode MuonChainFilterAlg::execute() {

  // check if there is anything in the filter list
  if (m_filterChains.empty()) {
    ATH_MSG_DEBUG( "Nothing to filter, pass = " << (m_notGate ? "false" : "true") );

    if (m_notGate) {
      ATH_CHECK( createDummyMuonContainers() );
    }
    setFilterPassed(!m_notGate);
    return StatusCode::SUCCESS;
  }

  // get input decisions (from output of input maker)
  for (auto inputKey : m_inputDecisionKeys) {
    auto inputDecisions = SG::makeHandle(inputKey);
    ATH_CHECK( inputDecisions.isValid() );
    ATH_MSG_DEBUG( "Checking inputHandle " << inputDecisions.key() << " with size: " << inputDecisions->size() );
    for (const TrigCompositeUtils::Decision* dec : *inputDecisions) {
      TrigCompositeUtils::DecisionIDContainer decIDs;
      TrigCompositeUtils::decisionIDs(dec, decIDs);
      for (const auto& legID : decIDs) {
        // in case of asymmetric trigger we should use HLT identifier which corresponds to a whole chain,
        // i.e. use HLT_mu6_mu4_bJpsimumu_L1MU6_2MU4 instead of leg001_HLT_mu6_mu4_bJpsimumu_L1MU6_2MU4;
        // TrigCompositeUtils::getIDFromLeg() will return unchanged id for the symmetric trigger
        HLT::Identifier id = TrigCompositeUtils::getIDFromLeg(HLT::Identifier(legID));
        std::string chainName = HLT::Identifier(id).name();
        const auto itr = m_filterChainIDs.find(id.numeric());
        if (!m_notGate) {  // pass if id does NOT match to any trigger from the filter list
          if (itr != m_filterChainIDs.end()) {
            ATH_MSG_DEBUG( "chain " << chainName << " is on the filter list, keep looking" );
          }
          else {
            ATH_MSG_DEBUG( "chain " << chainName << " is not on the filter list, passing" );
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
        }
        else {  // pass if id is found in the filter list
          if (itr != m_filterChainIDs.end()) {
            ATH_MSG_DEBUG( "chain " << chainName << " is on the not-filter list, passing" );
            setFilterPassed(true);
            return StatusCode::SUCCESS;
          }
          else {
            ATH_MSG_DEBUG( "chain " << chainName << " is not on the not-filter list, keep looking" );
          }
        }
      }
    }
  }

  // if we've reached this point, the only active chains were on the filter list, so the filter will fail
  ATH_MSG_DEBUG( "No trigger chains found, pass = false" );
  ATH_CHECK( createDummyMuonContainers() );
  setFilterPassed(false);

  return StatusCode::SUCCESS;
}


StatusCode MuonChainFilterAlg::createDummyMuonContainers() {

  // write out empty muComb/muFast container since EDM creator expects the container to always be present in the view
  if (m_writeL2muComb) {
    SG::WriteHandle handle(m_muCombKey);
    ATH_CHECK( handle.record(std::make_unique<xAOD::L2CombinedMuonContainer>(), std::make_unique<xAOD::L2CombinedMuonAuxContainer>()) );
  }
  if (m_writeL2muFast) {
    SG::WriteHandle handle(m_muFastKey);
    ATH_CHECK( handle.record(std::make_unique<xAOD::L2StandAloneMuonContainer>(), std::make_unique<xAOD::L2StandAloneMuonAuxContainer>()) );
  }
  return StatusCode::SUCCESS;
}
