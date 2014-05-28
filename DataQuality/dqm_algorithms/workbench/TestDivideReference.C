/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestDivideReference()
{
  TH1F* hin = new TH1F("hin","hin",100,-1,1);
  hin->FillRandom("gaus",10000);
  hin->SetBinContent(50,0);
  hin->SetBinContent(51,0);  
  hin->SetBinContent(52,0);
  hin->SetLineWidth(2);
  hin->Draw();
  TH1F* href= new TH1F("href","href",100,-1,1);
  for ( Int_t bin = href->GetXaxis()->GetFirst() ; bin<=href->GetXaxis()->GetLast();++bin)
    href->SetBinContent(bin,10);

  href->SetLineStyle(2);
  href->SetLineWidth(2);
  href->SetLineColor(kRed);
  href->Draw("same");
  //dqm_algorithms::AddReference * algorithm = new dqm_algorithms::AddReference_All_Bins_Filled();
  //dqm_algorithms::DivideReference * algorithm = new dqm_algorithms::DivideReference_Bins_Diff_FromAvg();
  dqm_algorithms::DivideReference * algorithm = new dqm_algorithms::DivideReference_BinContentComp();
  //Let's now create a reference of two histograms
  TObjArray* references = new TObjArray(2);
  references->Add(href);
  TH1* hh = hin->Clone();
  hh->Divide(href);
  references->Add(hh);
  dqm_core::test::DummyAlgorithmConfig* aconfig = new dqm_core::test::DummyAlgorithmConfig(references);
  //dqm_core::test::DummyAlgorithmConfig* aconfig = new dqm_core::test::DummyAlgorithmConfig(href);
  aconfig->addParameter("MinStat",100);
  aconfig->addParameter("NSigma",0.1);
  aconfig->addParameter("Coeff",1);
  aconfig->addGreenThreshold("NBins",10);
  aconfig->addRedThreshold("NBins",20);
  dqm_core::Result* result = algorithm->execute("test",*hin,*aconfig);
  std::cout<<"Result "<<result->status_<<std::endl;
  TObject* o = result->getObject();
  std::cout<<o->GetName()<<" "<<((TObjArray*)o)->GetEntries()<<std::endl;
  TObjArray* results = (TObjArray*)o;
  TH1* modifiedhisto = (TH1*)results->At(1);
  modifiedhisto->SetLineColor(kBlue);
  modifiedhisto->Draw("same");
  algorithm->printDescription();
}
