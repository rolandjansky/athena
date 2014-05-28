/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestJarqueBera()
{

    dqm_algorithms::JarqueBeraTest_Prob * algorithm = new dqm_algorithms::JarqueBeraTest_Prob();  
    TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -10, 10 );
    histogram->FillRandom( "gaus", 1000 );
    
    histogram->Draw();

    dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig();
    aconfig->addGreenThreshold("P",0.90);
    aconfig->addRedThreshold("P",0.1);
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
    algorithm->printDescription();  
    std::cout << "Result " << result->status_<< std::endl;
    //std::cout<<*result<<std::endl;
}
