/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestAddReference()
{
  TH1F* hin = new TH1F("hin","hin",100,-1,1);
  hin->FillRandom("gaus",10000);
  hin->SetBinContent(50,0);
  hin->SetBinContent(51,0);  
  hin->SetBinContent(52,0);
  hin->SetLineWidth(2);
  hin->Draw();
  TH1F* href= new TH1F("href","href",100,-1,1);
  href->SetBinContent(50,100);
  href->SetBinContent(51,100);
  href->SetBinContent(52,100);
  href->SetLineStyle(2);
  href->SetLineWidth(2);
  href->SetLineColor(kRed);
  href->Draw("same");
  //dqm_algorithms::AddReference * algorithm = new dqm_algorithms::AddReference_All_Bins_Filled();
  dqm_algorithms::AddReference * algorithm = new dqm_algorithms::AddReference_Bins_Diff_FromAvg();
  
  dqm_core::test::DummyAlgorithmConfig* aconfig = new dqm_core::test::DummyAlgorithmConfig(href);
  aconfig->addParameter("MinStat",100);
  aconfig->addParameter("Coeff",1);
  aconfig->addParameter("NSigma",1);
  aconfig->addGreenThreshold("NBins",10);
  aconfig->addRedThreshold("NBins",20);
  dqm_core::Result* result = algorithm->execute("test",*hin,*aconfig);
  std::cout<<"Result "<<result->status_<<std::endl;
  TObject* o = result->getObject();
  std::cout<<o->GetName()<<" "<<((TObjArray*)o)->GetEntries()<<std::endl;
  //TH1* modifiedhisto = (TH1*)result->getObject();
  //modifiedhisto->SetLineColor(kBlue);
  //modifiedhisto->Draw("same");
  algorithm->printDescription();
}
