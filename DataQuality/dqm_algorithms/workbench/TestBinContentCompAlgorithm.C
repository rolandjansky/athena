/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<map>
#include<vector>
#include<string>

dqm_core::Result * TestBinContentCompAlgorithm()
{   
    dqm_algorithms::BinContentComp * algorithm = new dqm_algorithms::BinContentComp();  
	
   TH1F *histogram=new TH1F("histogram","histogram",100,-1,1);
   TH1F *ref=new TH1F("ref","ref",100,-1,1);
   
   histogram->FillRandom("gaus",1000);
   ref->FillRandom("pol1",10000);

   // dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig(ref);
   dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig();
 
    aconfig->addParameter("NSigma",1);
    aconfig->addParameter("Ignore0",1);
    aconfig->addParameter("xmin",-2.0);
    aconfig->addParameter("xmax",-1.0);
    aconfig->addParameter("ymin",1.0);
    aconfig->addParameter("ymax",2.0);
    aconfig->addParameter("Value",100);
    aconfig->addParameter("PublishBins",1);
    aconfig->addGreenThreshold("NBins",1);
    aconfig->addRedThreshold("NBins",33);
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
   
    std::cout << "Result " << result->status_<< std::endl;    
    return result;
}
