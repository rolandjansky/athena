/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * TestStat()
{ 
  //   dqm_algorithms::CheckHisto_RMS * algorithm = new dqm_algorithms::CheckHisto_RMS();  
  dqm_algorithms::CheckHisto_Mean * algorithm = new dqm_algorithms::CheckHisto_Mean();  
  
  
  TH2F * histogram = new TH2F( "histogram", "Simple Gaussian Histogram", 100, -2, 2,100,-2,2);
  histogram->FillRandom( "gaus", 1000 );
  
  
  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
  
  // These are optional Parameters
  aconfig->addParameter("MinStat",1000);
  aconfig->addParameter("xmin",-2);
  aconfig->addParameter("xmax",1.4);
  aconfig->addParameter("ymin",-1.8);
  aconfig->addParameter("ymax",1.6);
  
  // Threshold names for CheckHisto_Mean() algorithm 
  aconfig->addGreenThreshold("AbsXMean",1);
  aconfig->addRedThreshold("AbsXMean",5);
  aconfig->addGreenThreshold("AbsYMean",1);
  aconfig->addRedThreshold("AbsYMean",5);
  
  // Threshold names for CheckHisto_Mean() algorithm if want to check actual Mean and not it's absolute value
  //aconfig->addGreenThreshold("XMean",1);
  //aconfig->addRedThreshold("XMean",5);
  //aconfig->addGreenThreshold("YMean",1);
  //aconfig->addRedThreshold("YMean",5);
  
  // Threshold names for CheckHisto_RMS() algorithm
  //   aconfig->addGreenThreshold("XRMS",1);
  //   aconfig->addRedThreshold("XRMS",5);
  //   aconfig->addGreenThreshold("YRMS",1);
  //   aconfig->addRedThreshold("YRMS",5);
  
  dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
  
  std::cout << "Result " << result->status_ << std::endl;
  
  return histogram;
}
