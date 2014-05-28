/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * Test_Bins_diffavg()
{ 
  dqm_algorithms::Bins_Diff_FromAvg * algorithm = new dqm_algorithms::Bins_Diff_FromAvg();  
  
  
 
  TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -2, 2);
  histogram->FillRandom( "pol0", 1000 );

  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
  
  //These are optional parameterms
  aconfig->addParameter("MinStat",1000);
  aconfig->addParameter("xmin",-2);
  aconfig->addParameter("xmax",-1);
  aconfig->addParameter("ymin",-1.8);
  aconfig->addParameter("ymax",-1.6);
  aconfig->addParameter("GreaterThan",1);
  aconfig->addParameter("LessThan",1);
  aconfig->addParameter("PublishBins",1);

  //Needed Parameter; check for bins N  sigma away from average
  aconfig->addParameter("NSigma",2);
  
  // Threshold for number of bins N sigma aways from average
  aconfig->addGreenThreshold("NBins",2);
  aconfig->addRedThreshold("NBins",10);
  
  dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
  TH1* h=result->getObject();
  h->Print("All");
  std::cout << "Result " << result->status_ << std::endl;
  
  return histogram;
}
