/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

TH1 * Test_BinsDiffFromPreviousLBs()
{ 
  dqm_algorithms::BinsDiffFromPreviousLBs * algorithm = new dqm_algorithms::BinsDiffFromPreviousLBs();  

  //Modify your parameters here, please
  int totLBs = 1000;
  double goodEffValue = 0.96;
  double badEffValue = 0.80;
  double binError = 0.04;
  double badLBs = 3;

  //Ideally, this histogram is filled with  flat lines (i.e. hif efficiency histogram).
  TH1F * histogram = new TH1F( "histogram", "Simple Efficiency Histogram", totLBs, 0, totLBs);
  for(int i=0; i < totLBs;i++){
    if(i<=50) histogram->SetBinContent(i+1,goodEffValue);
    if(i>50&&i<=50+badLBs) histogram->SetBinContent(i+1,badEffValue);
    if(i>50+badLBs)histogram->SetBinContent(i+1,goodEffValue);
    histogram->SetBinError(i+1,binError);
  }

  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );

  aconfig->addParameter("PublishBins",1);
  aconfig->addParameter("NSigma",1);
  aconfig->addParameter("BinThreshold",0.97);
  aconfig->addGreenThreshold("NBins",5);
  aconfig->addRedThreshold("NBins",10);
  //aconfig->addParameter("doCheckOnSingleLB",1);
  
  dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
  TH1* h=(TH1F*)result->getObject();
  h->Print("All");
  std::cout << "Result " << result->status_ << std::endl;
  
  h->Draw();
  histogram->SetLineColor(2);
  histogram->SetMarkerColor(2);
  histogram->Draw("same");
  return histogram;
}
