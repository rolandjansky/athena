/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HltConfigNtComponent_h__
#define __HltConfigNtComponent_h__
/*
  HltConfigNtComponent.h
*/
#include <vector>
#include <string>
#include "TrigNtupleBase/NtupleComponent.h"
#include "TrigNtupleBase/NtupleAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigConfigSvc/IHLTConfigSvc.h"

class HltConfigNtComponent : public NtComponent::NtupleComponent {
public:
  HltConfigNtComponent(NtupleAlgorithm* algo, 
		       const NtComponent::NtComponentParameters& params);
  ~HltConfigNtComponent();

  StatusCode setupServices();
  StatusCode book();
  StatusCode fill();

  void clear();

protected:
  ServiceHandle<TrigConf::IHLTConfigSvc> mTrigConfigSvc;
  std::string mEventInfoKey;

  int mRun;
  std::vector<std::string>* mChainNames;
  std::vector<int>* mChainCounters;
};

#endif // __HltConfigNtComponent_h__
