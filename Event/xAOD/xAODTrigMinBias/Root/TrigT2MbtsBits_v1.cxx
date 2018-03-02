/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "xAODCore/AuxStoreAccessorMacros.h"

#include "xAODTrigMinBias/versions/TrigT2MbtsBits_v1.h"
//#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <sstream>

namespace xAOD {
  
  TrigT2MbtsBits_v1::TrigT2MbtsBits_v1()
  : SG::AuxElement() {
  }
  
  TrigT2MbtsBits_v1::~TrigT2MbtsBits_v1(){}
  
  // Simple debug method using MsgStream
  void TrigT2MbtsBits_v1::print(void) const {
    
    std::cout << "triggerEnergies = { ";
    for (float x : triggerEnergies()) {
      std::cout << x << " ";
    }
    std::cout << " }" << std::endl;
      
    for (float x : triggerTimes()) {
      std::cout << x << " ";
    }
    std::cout << " }" << std::endl;
  }

// Simple string conversion method.
std::string str(const TrigT2MbtsBits_v1& trigT2MbtsBits) {
    
  std::stringstream sstream;
  std::vector<float> counterValues;
  std::vector<float>::const_iterator itr;
  std::vector<float>::const_iterator itr_end;
    
  counterValues = trigT2MbtsBits.triggerEnergies();
  itr = counterValues.begin();
  itr_end = counterValues.end();
  sstream << "triggerEnergies = { ";
  for(;itr != itr_end; ++itr) {
    sstream << (*itr) << " ";
  }
  sstream << " }" << std::endl;         
      
  counterValues = trigT2MbtsBits.triggerTimes();
  itr = counterValues.begin();
  itr_end = counterValues.end();
  sstream << "triggerTimes = { ";
	for(;itr != itr_end; ++itr) {
	  sstream << (*itr) << " ";
	}
	sstream << "}";
	
	return sstream.str();
  }

#if 0
  MsgStream& operator<< (MsgStream& m, const TrigT2MbtsBits_v1& trigT2MbtsBits) {
    return (m << str(trigT2MbtsBits));
  }
#endif // 0

  // Simple accessor functions for the "default variable" access pattern
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigT2MbtsBits_v1, std::vector<float>, triggerEnergies, setTriggerEnergies)
  AUXSTORE_OBJECT_SETTER_AND_GETTER (TrigT2MbtsBits_v1, std::vector<float>, triggerTimes, setTriggerTimes)
}

