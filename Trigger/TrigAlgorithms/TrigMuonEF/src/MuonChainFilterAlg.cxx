/*
  Filter chains for muon trigger algorithms. Designed to pass if at least one active chain is not in the list to be filtered.
  
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonChainFilterAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"

MuonChainFilterAlg::MuonChainFilterAlg(const std::string& name, ISvcLocator* pSvcLocator )
:AthAlgorithm(name, pSvcLocator)
{


}

StatusCode MuonChainFilterAlg::initialize(){

  ATH_CHECK(m_inputDecisionKeys.initialize());
  ATH_CHECK(m_muCombKey.initialize(m_writeL2muComb));
  ATH_CHECK(m_muFastKey.initialize(m_writeL2muFast));
  if ( m_filterChains.size()!=0 ) {
    for (size_t i=0; i<m_filterChains.size(); ++i) {
      ATH_MSG_DEBUG("IO TEST: " << i << " filter chains >> " << m_filterChains[i]);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonChainFilterAlg::finalize()
{
  ATH_MSG_DEBUG("Finalization");

  return StatusCode::SUCCESS;
}

StatusCode MuonChainFilterAlg::execute()
{

  bool pass = false;
  //First check if there is anything in the filter list
  if(m_filterChains.size()==0){
    ATH_MSG_DEBUG("Nothing to filter");
    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }

  //get input decisions (from output of input maker)
  for(auto inputKey : m_inputDecisionKeys){
    auto inputHandle = SG::makeHandle(inputKey);
    if(inputHandle.isValid()){
      auto inputs = inputHandle.ptr();
      ATH_MSG_DEBUG("Checking inputHandle "<<inputHandle.key()<<" with size: "<<inputs->size());
      for(size_t i=0; i < inputs->size(); i++){
	const TrigCompositeUtils::Decision* dec = inputs->at(i);
	TrigCompositeUtils::DecisionIDContainer decIDs;
	TrigCompositeUtils::decisionIDs(dec, decIDs);
	for( auto id : decIDs){
	  //get name of decisions to determine active chains
	  //and compare to the chains on list to be
	  std::string chainName = HLT::Identifier(id).name();
	  auto it = find(m_filterChains.begin(), m_filterChains.end(), chainName);
	  //if there is any chain that is active and not on 
	  //the filter list, set the filter to pass
	  if(it != m_filterChains.end()){
	    ATH_MSG_DEBUG("chain "<<chainName<<" is on the filter list, keep looking");
	  }
	  else{
	    ATH_MSG_DEBUG("chain "<<chainName<<" is not on the filter list. Passing");
	    pass = true;
	    setFilterPassed(pass);
	    return StatusCode::SUCCESS;
	  }
	}
      }
    }
    else ATH_MSG_DEBUG("No valid inputs found");
  }

  //If we've reached this point, the only active chains were on the 
  //filter list, so the filter will fail
  ATH_MSG_DEBUG("No chains found, pass = "<<pass);
  //write out empty muComb container since edm creator expects the container to always be present in the view
  if(!pass && m_writeL2muComb){
    SG::WriteHandle wh_muons(m_muCombKey);
    ATH_CHECK( wh_muons.record (std::make_unique<xAOD::L2CombinedMuonContainer>(), std::make_unique<xAOD::L2CombinedMuonAuxContainer>()) );
  }
  //write out empty muFast container since edm creator expects the container to always be present in the view
  if(!pass && m_writeL2muFast){
    SG::WriteHandle wh_muons(m_muFastKey);
    ATH_CHECK( wh_muons.record (std::make_unique<xAOD::L2StandAloneMuonContainer>(), std::make_unique<xAOD::L2StandAloneMuonAuxContainer>()) );
  }
  setFilterPassed(pass);

  return StatusCode::SUCCESS;
}
