/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TH1 * TestAlgorithm()
{ 
    dqm_algorithms::Simple_gauspluspol1_Fit * algorithm = new dqm_algorithms::Simple_gauspluspol1_Fit();  
    TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -2, 2 );
    histogram->FillRandom( "gaus", 1000 );

    //dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
    dqm_algorithms::tools::SimpleAlgorithmConfig * aconfig = new dqm_algorithms::tools::SimpleAlgorithmConfig( );
   aconfig->addGreenThreshold("AbsMean",0.01);
   aconfig->addRedThreshold("AbsMean",0.05);
   aconfig->addGreenThreshold("Sigma",1);
   aconfig->addRedThreshold("Sigma",5);
   aconfig->addGreenThreshold("Constant",1);
   aconfig->addRedThreshold("Constant",5);
   aconfig->addGreenThreshold("Chi2_per_NDF",1);
   aconfig->addRedThreshold("Chi2_per_NDF",5);
   aconfig->addParameter("xmin",-1);
   aconfig->addParameter("xmax",1);
   aconfig->addParameter("SubtractFromMean",0.2);
   //aconfig->addParameter("Verbose",);
                                                        
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
    std::cout << "Result " << result->status_ << std::endl;
    //Usage:
    //dqm_algorithms::tools::DumpConfig(string ParameterName, dqm_core::test::DummyAlgorithmConfig& config, string algorithmname, string histogramname, string reffilename = , string refhistogramname = , float weight = 1., string regionname = )
    dqm_algorithms::tools::DumpConfig *n=new dqm_algorithms::tools::DumpConfig("test",*aconfig,"Simple_gaus_Fit","histogram","","",1.0,"regname");  
    n->DumpOnlineConfig("test.data.xml");
    return histogram;
}
