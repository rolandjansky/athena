/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteeringTest_DummyFexWithAcceptInput_h
#define TrigSteeringTest_DummyFexWithAcceptInput_h

#include "TrigInterfaces/FexAlgo.h"


class TrigRoiDescriptor;

class DummyFexWithAcceptInput : public HLT::FexAlgo {
public:
  DummyFexWithAcceptInput(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor

  HLT::ErrorCode hltInitialize() { return HLT::OK; } //!< hlt initialize -> do nothing
  HLT::ErrorCode hltFinalize()   { return HLT::OK; } //!< hlt finalize   -> do nothing
     
  HLT::ErrorCode acceptInput(const HLT::TriggerElement* input, bool& pass );

  HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
			    HLT::TriggerElement* output);
      
  ~DummyFexWithAcceptInput() {}

private:
  unsigned m_counter;
  unsigned m_prescale;
  
  TrigRoiDescriptor *m_descriptor;
};



#endif // #ifndef TrigSteeringTest_DummyFexWithAcceptInput_h
