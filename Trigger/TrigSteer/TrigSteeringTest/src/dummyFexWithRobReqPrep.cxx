/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringTest/dummyFexWithRobReqPrep.h"


dummyFexWithRobReqPrep::dummyFexWithRobReqPrep(const std::string& name, ISvcLocator* pSvcLocator)
   : HLT::FexAlgo(name, pSvcLocator)
{}


HLT::ErrorCode
dummyFexWithRobReqPrep::hltExecute(const HLT::TriggerElement* /*input*/,
                                   HLT::TriggerElement* /*output*/)
{
  msg() << MSG::DEBUG << "calling hltExecute() for TE" << endreq;
  return HLT::OK;
}
     

HLT::ErrorCode
dummyFexWithRobReqPrep::prepareRobRequests(const HLT::TEConstVec& inputTEs )
{

  msg() << MSG::DEBUG << "executing prepareRobRequests() for " << inputTEs.size() << " input TEs of type " << (inputTEs.size()?inputTEs[0]->getId():0) << endreq;
    
  std::vector<uint32_t> ids_of_needed_robs;
  for(std::vector<const HLT::TriggerElement*>::const_iterator inputTE = inputTEs.begin();
      inputTE != inputTEs.end(); ++inputTE) {

    // calculate and register single ROB ID for given TE
    /*
      uint32_t id_of_needed_rob = calc_robid_for_TE(*inputTE);
      config()->robRequestInfo()->addRequestScheduledRobIDs(id_of_needed_rob);
    */

    // or

    // calculate and register list of ROB IDs for given TE
    calc_robids_for_TE(*inputTE, ids_of_needed_robs);

  }
  config()->robRequestInfo()->addRequestScheduledRobIDs(ids_of_needed_robs);
  return HLT::OK;
}


void 
dummyFexWithRobReqPrep::calc_robids_for_TE(const HLT::TriggerElement* /*inputTE*/, std::vector<uint32_t>& ids_of_needed_robs) {
  
  ids_of_needed_robs.push_back(12345);
  ids_of_needed_robs.push_back(987654);

}
