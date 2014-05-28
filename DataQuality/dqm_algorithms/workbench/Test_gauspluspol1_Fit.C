/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * Test_gauspluspol1_Fit()
{ 
  dqm_algorithms::Simple_gauspluspol1_Fit * algorithm = new dqm_algorithms::Simple_gauspluspol1_Fit();  
 
  TH1F * histogram = new TH1F( "histogram", "Simple pol1sian Histogram", 100, -2, 2 );
  histogram->FillRandom( "gaus", 1000 );
  histogram->FillRandom( "pol1", 1000 );
  
  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
  
  // Optional Parameters, Fit range
  aconfig->addParameter("MinStat",1000);
  aconfig->addParameter("xmin",-1);
  aconfig->addParameter("xmax",1);
  
  // Thresholds for Fit Parameters...Only put thresholds on values you'd like to check....others will not be checked
  
  // Threshold for gaus fit parameters
  aconfig->addGreenThreshold("Sigma",2);
  aconfig->addRedThreshold("Sigma",3);
  aconfig->addGreenThreshold("AbsMean",1);
  aconfig->addRedThreshold("AbsMean",5);
  // Threshold names if want to check actual Mean and not it's absolute value
  //aconfig->addGreenThreshold("Mean",1);
  //aconfig->addRedThreshold("Mean",5);
  //aconfig->addGreenThreshold("Constant",1);
  //aconfig->addRedThreshold("Constant",5);
  
  //Threshold for pol1 fit parameters
  //aconfig->addGreenThreshold("pol1[0]",9);
  //aconfig->addRedThreshold("pol1[0]",20);
  //aconfig->addGreenThreshold("pol1[1]",9);
  //aconfig->addRedThreshold("pol1[1]",20);
  
  dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
  
  std::cout << "Result " << result->status_ << std::endl;
  
  return histogram;
}
