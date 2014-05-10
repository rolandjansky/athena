/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AcceptAnyInput.h"
#include "boost/foreach.hpp"


AcceptAnyInput::AcceptAnyInput(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::AllTEAlgo(name, pSvcLocator) {
  declareProperty("mergeAll",  m_mergeAll=false, "If set True all trigger elemens will be used to seed the single output");
}


HLT::ErrorCode AcceptAnyInput::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
						  unsigned int output) {
  if (m_mergeAll) {
    std::vector<HLT::TriggerElement*> tes;
    BOOST_FOREACH( std::vector<HLT::TriggerElement*>& vec, input ) {
      tes.insert(tes.end(), vec.begin(), vec.end());      
    }
    config()->getNavigation()->addNode(tes, output);
    ATH_MSG_DEBUG("Created singe output TE seeded by: "<< tes.size() << " TEs");
  } else {
    unsigned counter(0);
    BOOST_FOREACH( std::vector<HLT::TriggerElement*>& vec, input ) {
      counter += vec.size();
      BOOST_FOREACH( HLT::TriggerElement* te, vec ) {
	config()->getNavigation()->addNode(te, output);
      }       
    }
    ATH_MSG_DEBUG("Created: " << counter << " TEs seeded be every input passed");    
  }
  return HLT::OK;
}
