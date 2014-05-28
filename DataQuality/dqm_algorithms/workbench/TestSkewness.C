/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

TestSkewness()
{

    dqm_algorithms::SkewnessTest_LessThan * algorithm = new dqm_algorithms::SkewnessTest_LessThan();  
    TH1F * histogram = new TH1F( "histogram", "Simple Gaussian Histogram", 100, -5, 5 );
    histogram->FillRandom( "gaus", 5000 );
    for (int i =0 ; i<100 ; ++i)
      histogram->Fill( -2.5 );
    histogram->Draw();
     
    std::cout<<"Skewness Value:"<<histogram->GetSkewness()<<std::endl;

    dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig();
    aconfig->addGreenThreshold("S",-0.1);
    aconfig->addRedThreshold("S",-0.09);
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
    algorithm->printDescription();  
    std::cout << "Result " << result->status_<< std::endl;    
}
