/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTrigMinBias/versions/TrigT2ZdcSignals_v1.h"
#include <iostream>

namespace xAOD {
  
  TrigT2ZdcSignals_v1::TrigT2ZdcSignals_v1()
  : SG::AuxElement() {
  }
  
  TrigT2ZdcSignals_v1::~TrigT2ZdcSignals_v1(){}
  
  // Simple debug method using MsgStream
  void TrigT2ZdcSignals_v1::print(void) const {
    
    std::cout << "triggerEnergiesVal = { ";
    for (float x : triggerEnergies()) {
      std::cout << x << " ";
    }
    std::cout << " }" << std::endl;
      
    for (float x : triggerTimes()) {
      std::cout << x << " ";
    }
    std::cout << " }" << std::endl;
  }

  // Simple accessor functions for the "default variable" access pattern
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigT2ZdcSignals_v1, std::vector<float>, triggerEnergies, setTriggerEnergies)
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigT2ZdcSignals_v1, std::vector<float>, triggerTimes, setTriggerTimes)
}

