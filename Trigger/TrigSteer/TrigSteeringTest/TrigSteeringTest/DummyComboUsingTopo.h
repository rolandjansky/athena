/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigSteeringTest_DummyComboUsingTopo
#define TrigSteeringTest_DummyComboUsingTopo

#include "TrigInterfaces/ComboAlgo.h"


//class TrigRoiDescriptor;

class DummyComboUsingTopo : public HLT::ComboAlgo {
public:

   DummyComboUsingTopo(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~DummyComboUsingTopo() {}

   virtual HLT::ErrorCode hltInitialize();

   virtual HLT::ErrorCode hltFinalize();

   HLT::ErrorCode hltExecute(HLT::TEConstVec& input, HLT::TriggerElement* output);
      
private:

   std::vector<float> m_pt;   // monitoring of pT of rois (multiple per execute call)
   float              m_dEta; // monitoring of delta eta of roi combinations (one per execute call)

};

#endif
