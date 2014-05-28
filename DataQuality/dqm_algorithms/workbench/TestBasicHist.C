/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * TestBasicHist()
{ 
  //dqm_algorithms::All_Bins_Filled * algorithm = new dqm_algorithms::All_Bins_Filled();  
  //dqm_algorithms::Histogram_Not_Empty * algorithm = new dqm_algorithms::Histogram_Not_Empty();  
  dqm_algorithms::Histogram_Empty * algorithm = new dqm_algorithms::Histogram_Empty();  
  //dqm_algorithms::No_OverFlows * algorithm = new dqm_algorithms::No_OverFlows();  
  //dqm_algorithms::No_UnderFlows * algorithm = new dqm_algorithms::No_UnderFlows();  
  
  
  //TH2F * histogram = new TH2F( "histogram", "Simple Gaussian Histogram", 100, -2, 2,100,-2,2);
  TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -2, 2);
  //histogram->FillRandom( "gaus", 1000 );
  //histogram->SetBinContent(0,1);
  //histogram->SetBinContent(100,101,1);

  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
  
  
  //aconfig->addParameter("MinStat",1000);
  dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
  
  std::cout << "Result " << result->status_ << std::endl;
  
  return histogram;
}
