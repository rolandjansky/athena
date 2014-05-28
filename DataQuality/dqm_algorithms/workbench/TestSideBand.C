/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1* TestSideBand()
{
  // Create and fill a gaussian histogram
  TH1* hrnd = new TH1F("hRandom","Test Histogram",100,-5,5);
  hrnd->FillRandom("gaus",10000);
  // Create algorithms and configure them
  dqm_algorithms::SideBand_Absolute* algo1 = new dqm_algorithms::SideBand_Absolute();
  dqm_algorithms::SideBand* algo2 = new dqm_algorithms::SideBand_Relative();
  
  dqm_core::test::DummyAlgorithmConfig* conf1 = new dqm_core::test::DummyAlgorithmConfig();
  dqm_core::test::DummyAlgorithmConfig* conf2 = new dqm_core::test::DummyAlgorithmConfig();
  
  conf1->addParameter("UseUnderFlow",1);
  conf1->addParameter("UseOverFlow",1);
  conf1->addParameter("Min",-3);
  conf1->addParameter("Max",3);
  conf2->addParameter("UseUnderFlow",1);
  conf2->addParameter("UseOverFlow",1);
  conf2->addParameter("Min",-3);
  conf2->addParameter("Max",3);
  
  conf1->addGreenThreshold("Threshold",20);
  conf1->addRedThreshold("Threshold",30);
  conf2->addGreenThreshold("Threshold",0.1);
  conf2->addRedThreshold("Threshold",0.05);
  
  //dqm_core::Result* result1 = algo1->execute("absoluteTest",*hrnd,*conf1);
  dqm_core::Result* result2 = algo2->execute("relativeTest",*hrnd,*conf2);
  //std::cout<<"Result absoluteTest: "<<result1->status_<<std::endl;
  std::cout <<"Result relativeTest: "<<result2->status_<<std::endl;
  return hrnd;
}
