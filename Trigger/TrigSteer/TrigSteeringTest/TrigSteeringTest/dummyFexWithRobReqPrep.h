/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteeringTest_dummyFexWithRobReqPrep_h
#define TrigSteeringTest_dummyFexWithRobReqPrep_h

#include "TrigInterfaces/FexAlgo.h"

class dummyFexWithRobReqPrep : public HLT::FexAlgo {
public:
  dummyFexWithRobReqPrep(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

  HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize -> do nothing
  HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize   -> do nothing
     
  using HLT::FexAlgo::prepareRobRequests;
  HLT::ErrorCode prepareRobRequests(const HLT::TEConstVec& inputTEVs );

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
                            HLT::TriggerElement* output);
   
  ~dummyFexWithRobReqPrep() {}

private:

  void calc_robids_for_TE(const HLT::TriggerElement* /*inputTE*/, std::vector<uint32_t>& ids_of_needed_robs);

};

#endif
