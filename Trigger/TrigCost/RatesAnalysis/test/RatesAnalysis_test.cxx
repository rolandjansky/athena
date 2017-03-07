
#include "../RatesAnalysis/RatesTrigger.h"
#include "../RatesAnalysis/RatesScanTrigger.h"
#include "../RatesAnalysis/RatesGroup.h"

#include "GaudiKernel/MsgStream.h"

#include <iostream>

int main() {

  MsgStream log(nullptr, "RatesAnalysis_test");

  RatesTrigger* triggerA1 = new RatesTrigger("TriggerA1", log, 1, 1, "SeedA", 2);
  RatesTrigger* triggerA2 = new RatesTrigger("TriggerA2", log, 2, 1, "SeedA", 2);

  RatesTrigger* triggerB1 = new RatesTrigger("TriggerB1", log, 3, 1, "SeedB", 4);
  RatesTrigger* triggerB2 = new RatesTrigger("TriggerB2", log, 4, 1, "SeedB", 4);

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

  triggerA1->setPassedAndExecute(true, wvs);
  triggerA2->setPassedAndExecute(true, wvs);
  triggerB1->setPassedAndExecute(true, wvs);
  triggerB2->setPassedAndExecute(true, wvs);

  groupA->execute(wvs);
  groupB->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();
  triggerB1->reset();
  triggerB2->reset();

  // simulate event two

  wvs.m_enhancedBiasWeight = 1000;

  triggerA1->setPassedAndExecute(true, wvs);
  triggerA2->setPassedAndExecute(true, wvs);

  groupA->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();

  // simulate event three

  wvs.m_enhancedBiasWeight = 500;

  triggerB1->setPassedAndExecute(true, wvs);
  triggerB2->setPassedAndExecute(true, wvs);

  groupB->execute(wvs);

  triggerB1->reset();
  triggerB2->reset();

  // simulate event four

  wvs.m_enhancedBiasWeight = 10;

  triggerA1->setPassedAndExecute(true, wvs);
  triggerB1->setPassedAndExecute(true, wvs);

  groupA->execute(wvs);
  groupB->execute(wvs);

  triggerA1->reset();
  triggerB1->reset();

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
