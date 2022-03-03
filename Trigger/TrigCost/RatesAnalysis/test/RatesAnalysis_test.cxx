/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "../RatesAnalysis/RatesTrigger.h"
#include "../RatesAnalysis/RatesScanTrigger.h"
#include "../RatesAnalysis/RatesGroup.h"

#include "GaudiKernel/IMessageSvc.h"

#include "CxxUtils/ubsan_suppress.h"
#include "TInterpreter.h"

#include <iostream>

int main() {
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });

  IMessageSvc* log = nullptr;  // using cout

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

  RatesGroup* groupA = new RatesGroup("GroupA", log);
  groupA->addToGroup(triggerA1);
  groupA->addToGroup(triggerA2);

  RatesGroup* groupB = new RatesGroup("GroupB", log);
  groupB->addToGroup(triggerB1);
  groupB->addToGroup(triggerB2);

  RatesGroup* groupAll = new RatesGroup("GroupAll", log);
  groupAll->setDoCachedWeights(true);
  groupAll->addToGroup(triggerA1);
  groupAll->addToGroup(triggerA2);
  groupAll->addToGroup(triggerB1);
  groupAll->addToGroup(triggerB2);

  RatesGroup* uniqueGroupA1 = new RatesGroup("UniqueGroupA1", log, /*histograms*/ false);
  triggerA1->setUniqueGroup(uniqueGroupA1);
  uniqueGroupA1->duplicateChildren(groupAll);
  uniqueGroupA1->setUniqueTrigger(triggerA1);
  uniqueGroupA1->removeOtherL1(triggerA1);
  uniqueGroupA1->removeFromGroup(triggerA1);
  uniqueGroupA1->setUseCachedWeights(true);

  RatesGroup* uniqueGroupA2 = new RatesGroup("UniqueGroupA2", log, /*histograms*/ false);
  triggerA2->setUniqueGroup(uniqueGroupA2);
  uniqueGroupA2->duplicateChildren(groupAll);
  uniqueGroupA2->setUniqueTrigger(triggerA2);
  uniqueGroupA2->removeOtherL1(triggerA2); 
  uniqueGroupA2->removeFromGroup(triggerA2);
  uniqueGroupA2->setUseCachedWeights(true);

  RatesGroup* uniqueGroupB1 = new RatesGroup("UniqueGroupB1", log, /*histograms*/ false);
  triggerB1->setUniqueGroup(uniqueGroupB1);
  uniqueGroupB1->duplicateChildren(groupAll);
  uniqueGroupB1->setUniqueTrigger(triggerB1);
  uniqueGroupB1->removeOtherL1(triggerB1); 
  uniqueGroupB1->removeFromGroup(triggerB1);
  uniqueGroupB1->setUseCachedWeights(true);

  RatesGroup* uniqueGroupB2 = new RatesGroup("UniqueGroupB2", log, /*histograms*/ false);
  triggerB2->setUniqueGroup(uniqueGroupB2);
  uniqueGroupB2->duplicateChildren(groupAll);
  uniqueGroupB2->setUniqueTrigger(triggerB2);
  uniqueGroupB2->removeOtherL1(triggerB2); 
  uniqueGroupB2->removeFromGroup(triggerB2);
  uniqueGroupB2->setUseCachedWeights(true);

  // simulate event one

  triggerA1->setPassedAndExecute(true, true, wvs);
  triggerA2->setPassedAndExecute(true, true, wvs);

  triggerB1->setPassedAndExecute(true, true, wvs);
  triggerB2->setPassedAndExecute(true, true, wvs);

  groupAll->execute(wvs); // Must be before the unique trigger groups
  groupA->execute(wvs);
  groupB->execute(wvs);
  uniqueGroupA1->execute(wvs);
  uniqueGroupA2->execute(wvs);
  uniqueGroupB1->execute(wvs);
  uniqueGroupB2->execute(wvs);

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

  groupAll->execute(wvs); // Must be before the unique trigger groups
  groupA->execute(wvs);
  groupB->execute(wvs);
  uniqueGroupA1->execute(wvs);
  uniqueGroupA2->execute(wvs);
  uniqueGroupB1->execute(wvs);
  uniqueGroupB2->execute(wvs);

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

  groupAll->execute(wvs); // Must be before the unique trigger groups
  groupA->execute(wvs);
  groupB->execute(wvs);
  uniqueGroupA1->execute(wvs);
  uniqueGroupA2->execute(wvs);
  uniqueGroupB1->execute(wvs);
  uniqueGroupB2->execute(wvs);

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

  groupAll->execute(wvs); // Must be before the unique trigger groups
  groupA->execute(wvs);
  groupB->execute(wvs);
  uniqueGroupA1->execute(wvs);
  uniqueGroupA2->execute(wvs);
  uniqueGroupB1->execute(wvs);
  uniqueGroupB2->execute(wvs);

  triggerA1->reset();
  triggerA2->reset();
  triggerB1->reset();
  triggerB2->reset();

  // end

  std::cout << triggerA1->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << triggerA2->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << triggerB1->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << triggerB2->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << groupAll->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << groupA->printRate(wvs.m_eventLiveTime) << std::endl;
  std::cout << groupB->printRate(wvs.m_eventLiveTime) << std::endl;

  delete triggerA1;
  delete triggerA2;
  delete triggerB1;
  delete triggerB2;
  delete groupAll;
  delete groupA;
  delete groupB;
  delete uniqueGroupA1;
  delete uniqueGroupA2;
  delete uniqueGroupB1;
  delete uniqueGroupB2;
}
