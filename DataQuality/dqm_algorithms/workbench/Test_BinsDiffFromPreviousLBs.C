/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * Test_BinsDiffFromPreviousLBs()
{ 
  dqm_algorithms::BinsDiffFromPreviousLBs * algorithm = new dqm_algorithms::BinsDiffFromPreviousLBs();  
  
  //Ideally, this histogram is filled with  flat lines (i.e. hif efficiency histogram).
  TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, 0, 100);
  for(int i=1; i < 101;i++){
    if(i<=50) histogram->SetBinContent(i,0.96);
    if(i>50&&i<=55) histogram->SetBinContent(i,0.80);
    if(i>55)histogram->SetBinContent(i,0.96);
    histogram->SetBinError(i,0.02);
  }

  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );

  aconfig->addParameter("PublishBins",1);
  aconfig->addParameter("NSigma",1);
  aconfig->addParameter("BinThreshold",0.97);
  aconfig->addGreenThreshold("NBins",0);
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
