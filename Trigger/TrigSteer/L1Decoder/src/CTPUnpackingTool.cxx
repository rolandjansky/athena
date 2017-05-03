/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigT1Result/RoIBResult.h"
#include "CTPUnpackingTool.h"

using namespace HLT;

CTPUnpackingTool::~CTPUnpackingTool()
{}

StatusCode CTPUnpackingTool::decode(const ROIB::RoIBResult* roib, const IndexToIdentifiers& ctpToChain, HLT::IDVec& enabledChains) const {
  size_t numberPfActivatedBits= 0;
  
  auto tav = roib->cTPResult().TAV();
  const size_t tavSize = tav.size();

  for ( size_t wordCounter = 0; wordCounter < tavSize; ++wordCounter ) {
    for ( size_t bitCounter = 0;  bitCounter < 32; ++bitCounter ) {
      const size_t ctpIndex = 32*wordCounter+bitCounter;
      const bool decision = (tav[wordCounter].roIWord() & (1 << bitCounter)) > 0;
      if ( decision == true ) {
	numberPfActivatedBits++;
	auto itr = ctpToChain.find(ctpIndex);
	if ( itr != ctpToChain.end() ) 
	  enabledChains.insert(enabledChains.end(), itr->second.begin(), itr->second.end());
      }
    }    
  }
  for ( auto chain: enabledChains ) {
    ATH_MSG_DEBUG("Enabling chain: " << chain);
  }
  if ( numberPfActivatedBits == 0 ) {
    ATH_MSG_ERROR("All CTP bits were disabled, this event shoudl not have shown here");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


