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
    
    const std::vector<float> m_triggerEnergies(triggerEnergies());
    const std::vector<float> m_triggerTimes(triggerTimes());
    std::vector<float>::const_iterator itr;
    std::vector<float>::const_iterator itr_end;
    
    itr=m_triggerEnergies.begin();
    itr_end=m_triggerEnergies.end();
    std::cout << "m_triggerEnergies = { ";
      for(;itr != itr_end; ++itr) {
	std::cout << (*itr) << " ";
      }
      std::cout << " }" << std::endl;
      
      itr = m_triggerTimes.begin();
      itr_end = m_triggerTimes.end();
      std::cout << "m_triggerTimes = { ";
	for(;itr != itr_end; ++itr) {
	  std::cout << (*itr) << " ";
	}
	std::cout << " }" << std::endl;
  }

  // Simple accessor functions for the "default variable" access pattern
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigT2ZdcSignals_v1, std::vector<float>, triggerEnergies, setTriggerEnergies)
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigT2ZdcSignals_v1, std::vector<float>, triggerTimes, setTriggerTimes)
}

