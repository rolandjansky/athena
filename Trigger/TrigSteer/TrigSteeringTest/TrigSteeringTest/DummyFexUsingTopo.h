/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteeringTest_DummyFexUsingTopo
#define TrigSteeringTest_DummyFexUsingTopo

#include "TrigInterfaces/FexAlgo.h"


class TrigRoiDescriptor;

class DummyFexUsingTopo : public HLT::FexAlgo {
public:

   DummyFexUsingTopo(const std::string& name, ISvcLocator* pSvcLocator);

   HLT::ErrorCode hltExecute(const HLT::TriggerElement* input,
                             HLT::TriggerElement* output);

   virtual HLT::ErrorCode hltInitialize();

   virtual HLT::ErrorCode hltFinalize();
      
   virtual ~DummyFexUsingTopo() {}
   
private:

};

#endif
