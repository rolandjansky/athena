/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigGenericAlgs/AcceptWhenInputMissing.h"
#include "boost/foreach.hpp"

AcceptWhenInputMissing::AcceptWhenInputMissing(const std::string& name, ISvcLocator* pSvcLocator) 
  : HLT::AllTEAlgo(name, pSvcLocator) {
}


HLT::ErrorCode AcceptWhenInputMissing::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
						  unsigned int output) {
  
  unsigned count(0);
  BOOST_FOREACH( std::vector<HLT::TriggerElement*>& vec, input ) 
    count += vec.size();

  if (count) {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "Not creating output because " <<  count << " input TEs present" << endmsg;    
  } else {
    if ( msgLvl() <= MSG::DEBUG )
      msg() << MSG::DEBUG << "Creating output TE" << endmsg;

    std::vector<HLT::TriggerElement*> fake_seed;
    HLT::TriggerElement* te = config()->getNavigation()->addNode(fake_seed, output);
    te->setActiveState(true);
  }
  return HLT::OK;
}
