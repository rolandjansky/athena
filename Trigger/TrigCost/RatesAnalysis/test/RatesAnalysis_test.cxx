/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "../RatesAnalysis/RatesTrigger.h"
#include "../RatesAnalysis/RatesScanTrigger.h"
#include "../RatesAnalysis/RatesGroup.h"

#include "GaudiKernel/MsgStream.h"

#include "CxxUtils/ubsan_suppress.h"
#include "TInterpreter.h"

#include <iostream>

int main() {
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });

  MsgStream log(nullptr, "RatesAnalysis_test");

  RatesTrigger* triggerA1 = new RatesTrigger("TriggerA1", log, /*prescale*/1, /*XpressPrescale*/1, "SeedA", /*seedPrescale*/2);
  RatesTrigger* triggerA2 = new RatesTrigger("TriggerA2", log, /*prescale*/2, /*XpressPrescale*/1, "SeedA", /*seedPrescale*/2);

  RatesTrigger* triggerB1 = new RatesTrigger("TriggerB1", log, /*prescale*/3, /*XpressPrescale*/1, "SeedB", /*seedPrescale*/4);
  RatesTrigger* triggerB2 = new RatesTrigger("TriggerB2", log, /*prescale*/4, /*XpressPrescale*/1, "SeedB", /*seedPrescale*/4);

  WeightingValuesSummary_t wvs;
  wvs.m_enhancedBiasWeight = 5000;
  wvs.m_eventMu = 30; 
  wvs.m_eventLumi = 1e30; 
  wvs.m_isUnbiased = false; 
  wvs.m_distanceInTrain = 1;  
  wvs.m_eventLiveTime = 0.1; 
  wvs.m_bunchFactor = 1.;
  wvs.m_muFactor = 1; 
  wvs.m_linearLumiFactor = 1;  
  wvs.m_expoMuFactor = 1; 

  RatesGroup* groupA =  new RatesGroup("GroupA", log);
  groupA->addToGroup(triggerA1);
  groupA->addToGroup(triggerA2);

  RatesGroup* groupB =  new RatesGroup("GroupB", log);
  groupB->addToGroup(triggerB1);
  groupB->addToGroup(triggerB2);

  // simulate event one

  triggerA1->setPassedAndExecute(true, true, wvs);
  triggerA2->setPassedAndExecute(true, true, wvs);

  triggerB1->setPassedAndExecute(true, true, wvs);
  triggerB2->setPassedAndExecute(true, true, wvs);

  groupA->execute(wvs);
  groupB->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();
  triggerB1->reset();
  triggerB2->reset();

  // simulate event two

  wvs.m_enhancedBiasWeight = 1000;

  triggerA1->setPassedAndExecute(true, true, wvs);
  triggerA2->setPassedAndExecute(true, true, wvs);

  triggerB1->setPassedAndExecute(false, false, wvs);
  triggerB2->setPassedAndExecute(false, false, wvs);

  groupA->execute(wvs);
  groupB->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();
  triggerB1->reset();
  triggerB2->reset();

  // simulate event three

  wvs.m_enhancedBiasWeight = 500;

  triggerA1->setPassedAndExecute(false, false, wvs);
  triggerA2->setPassedAndExecute(false, false, wvs);

  triggerB1->setPassedAndExecute(true, true, wvs);
  triggerB2->setPassedAndExecute(true, true, wvs);

  groupA->execute(wvs);
  groupB->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();
  triggerB1->reset();
  triggerB2->reset();

  // simulate event four

  wvs.m_enhancedBiasWeight = 10;

  triggerA1->setPassedAndExecute(true, true, wvs);
  triggerA2->setPassedAndExecute(false, true, wvs);

  triggerB1->setPassedAndExecute(true, true, wvs);
  triggerB2->setPassedAndExecute(false, true, wvs);

  groupA->execute(wvs);
  groupB->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();
  triggerB1->reset();
  triggerB2->reset();

  // end

  std::cout << triggerA1->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << triggerA2->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << triggerB1->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << triggerB2->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << groupA->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << groupB->printRate(wvs.m_eventLiveTime) << std::endl;

  delete triggerA1;
  delete triggerA2;
  delete triggerB1;
  delete triggerB2;
  delete groupA;
  delete groupB;

}
