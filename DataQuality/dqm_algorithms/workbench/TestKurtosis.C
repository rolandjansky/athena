/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestKurtosis()
{

    dqm_algorithms::KurtosisTest_LessThanAbs * algorithm = new dqm_algorithms::KurtosisTest_LessThanAbs();  
    TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -2, 2 );
    histogram->FillRandom( "gaus", 10000 );
    std::cout<<"Kurtosis Value:"<<histogram->GetKurtosis()<<std::endl;

    dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig();
    aconfig->addGreenThreshold("K",0.2);
    aconfig->addRedThreshold("K",0.5);
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
    algorithm->printDescription();  
    std::cout << "Result " << result->status_<< std::endl;    
}
