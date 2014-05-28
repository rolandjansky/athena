/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestReferenceMasking()
{
  bool test1 = true;
  //Make an histogram with 3 "hot" bins
  TH1F* hin = new TH1F("hin","hin",100,-1,1);
  if (test1) hin->FillRandom("gaus",10000);
  else hin->FillRandom("pol0",10000);
  hin->SetBinContent(50,200);
  hin->SetBinContent(51,200);
  hin->SetBinContent(52,200);
  hin->SetLineWidth(2);
  hin->Draw();
  //The reference contains the bins to be "masked"
  TH1F* href= new TH1F("href","href",100,-1,1);
  href->SetBinContent(50,100);//It's important that the content is !=0
  href->SetBinContent(51,100);//Doesn't matter what is the content
  href->SetBinContent(52,100);
  href->SetLineStyle(2);
  href->SetLineWidth(2);
  href->SetLineColor(kRed);
  href->Draw("same");
  dqm_core::Algorithm* algorithm;
  //Count number of bins abobe a threshold
  if (test1) algorithm = new dqm_algorithms::ReferenceMasking_Bins_GreaterThan_Threshold();
  else algorithm = new dqm_algorithms::ReferenceMasking_Bins_Diff_FromAvg();
  
  dqm_core::test::DummyAlgorithmConfig* aconfig = new dqm_core::test::DummyAlgorithmConfig(href);
  aconfig->addParameter("MinStat",100);
  aconfig->addParameter("C",0);//Fill masked bins with "C", in this case 0
  if ( test1 ) {
    aconfig->addParameter("BinThreshold",150);
    aconfig->addGreenThreshold("NBins",1);
    aconfig->addRedThreshold("NBins",4);
  } else {
  aconfig->addParameter("NSigma",3);
  aconfig->addGreenThreshold("NBins",10);
  aconfig->addRedThreshold("NBins",20);
  }
  dqm_core::Result* result = algorithm->execute("test",*hin,*aconfig);
  std::cout<<"Result "<<result->status_<<std::endl;
  TH1* modifiedhisto = (TH1*)result->getObject();
  modifiedhisto->SetLineColor(kBlue);
  modifiedhisto->SetLineStyle(3);
  modifiedhisto->Draw("same");
  algorithm->printDescription();
}
