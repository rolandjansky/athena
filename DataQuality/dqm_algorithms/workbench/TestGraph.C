/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestGraph()
{
  dqm_algorithms::GraphTest* algorithm = new dqm_algorithms::GraphTest();
  TFile* f=TFile::Open("../share/Test.root");//##filename##);
  TObject* obj=f->Get("LBA19_hi_tails_5");//##histoname##);
  ((TGraphAsymmErrors*)obj)->SetPointError(23,0,0,0.5,0.5);// Set big errors on one point
  // Creating a reference object...
  TGraphAsymmErrors* ref = new TGraphAsymmErrors(48);
  ref->SetNameTitle("reference","reference");
  for ( int i = 0;i<48; ++i)
    {
      ref->SetPoint(i,i+0.5,1.);
      ref->SetPointError(i,0,0,0.15,0.15);
    }
  // Turn off three deadempty channels
  ref->SetPointError(31,0,0,0,0);
  ref->SetPointError(32,0,0,0,0);
  ref->SetPointError(43,0,0,0,0);
  TCanvas* c=new TCanvas;
  obj->Draw("AP");
  ref->Draw("P");
  
  dqm_core::test::DummyAlgorithmConfig* aconfig=new dqm_core::test::DummyAlgorithmConfig();//ref);
  //aconfig->addGreenThreshold("XErrHigh",0.);
  //aconfig->addGreenThreshold("XErrLow",0.);
  aconfig->addGreenThreshold("YErrHigh",0.3);
  aconfig->addGreenThreshold("YErrLow",0.3);
  //aconfig->addGreenThreshold("NBins",0);
  //aconfig->addGreenThreshold("DistFactor",1.);
  //aconfig->addRedThreshold("XErrHigh",0.);
  //aconfig->addRedThreshold("XErrLow",0.);
  aconfig->addRedThreshold("YErrHigh",0.4);
  aconfig->addRedThreshold("YErrLow",0.4);
  //aconfig->addRedThreshold("NBins",3);
  //aconfig->addRedThreshold("DistFactor",1.);
  dqm_core::Result* result = algorithm->execute("test",*obj,*aconfig);
  std::cout<<result->status_<<std::endl;
  
}
