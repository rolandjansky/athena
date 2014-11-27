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

#if 0
  // Simple debug method using MsgStream
  void TrigT2MbtsBits_v1::print(MsgStream& log) const {
    const std::vector<float> m_triggerEnergies(triggerEnergies());
    const std::vector<float> m_triggerTimes(triggerTimes());
    std::vector<float>::const_iterator itr;
    std::vector<float>::const_iterator itr_end;
    
    itr=m_triggerEnergies.begin();
    itr_end=m_triggerEnergies.end();
    log << MSG::DEBUG << "m_triggerEnergies = { ";
      for(;itr != itr_end; ++itr) {
	log << MSG::DEBUG << (*itr) << " ";
      }
      log << MSG::DEBUG << " }" << endreq;  
      
      itr = m_triggerTimes.begin();
      itr_end = m_triggerTimes.end();
      log << MSG::DEBUG << "m_triggerTimes = { ";
	for(;itr != itr_end; ++itr) {
	  log << MSG::DEBUG << (*itr) << " ";
	}
	log << MSG::DEBUG << " }" << endreq;
  }
#endif // 0

  // Simple string conversion method.
  std::string str(const TrigT2MbtsBits_v1& trigT2MbtsBits) {
    
    std::stringstream sstream;
    std::vector<float> counterValues;
    std::vector<float>::const_iterator itr;
    std::vector<float>::const_iterator itr_end;
    
    counterValues = trigT2MbtsBits.triggerEnergies();
    itr = counterValues.begin();
    itr_end = counterValues.end();
    sstream << "m_triggerEnergies = { ";
      for(;itr != itr_end; ++itr) {
	sstream << (*itr) << " ";
      }
      sstream << " }" << std::endl;         
      
      counterValues = trigT2MbtsBits.triggerTimes();
      itr = counterValues.begin();
      itr_end = counterValues.end();
      sstream << "m_triggerTimes = { ";
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

