/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * TestDivideBin()
{ 
  
  dqm_algorithms::DivideBin * algorithm = new dqm_algorithms::DivideBin();  
 
  TH1F* hin = new TH1F("hin","hin",100,-1,1);
  hin->FillRandom("gaus",10000);
  hin->SetBinContent(50,100);
  hin->SetBinContent(51,200);  
  hin->SetBinContent(52,300);  
  hin->Draw();


  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig(hin);
  aconfig->addParameter("MinStat",100);
  aconfig->addParameter("xmin",51);
  aconfig->addParameter("xmax",52);
  aconfig->addGreenThreshold("Threshold",0.95);
  aconfig->addRedThreshold("Threshold",0.75);
 
  dqm_core::Result * result = algorithm->execute( "test", *hin, *aconfig );
  algorithm->printDescription();  

  return hin;
}
