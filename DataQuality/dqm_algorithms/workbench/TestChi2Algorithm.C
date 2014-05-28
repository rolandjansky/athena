/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestChi2Algorithm()
{

    dqm_algorithms::Chi2Test_Chi2 * algorithm = new dqm_algorithms::Chi2Test_Chi2();  
    TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -2, 2 );
    histogram->FillRandom( "gaus", 1000 );
    

    dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig(histogram);
    aconfig->addGreenThreshold("Chi2",.9);
    aconfig->addRedThreshold("Chi2",.3);
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
    algorithm->printDescription();  
    std::cout << "Result " << result->status_<< std::endl;    
}
