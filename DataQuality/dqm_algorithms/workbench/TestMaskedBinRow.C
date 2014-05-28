/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<map>
#include<vector>
#include<string>

dqm_core::Result * TestMaskedBinRowAlgorithm()
//void TestMaskedBinRowAlgorithm()
{   
    dqm_algorithms::MaskedBinRow * algorithm = new dqm_algorithms::MaskedBinRow();  
	
   TH2F *histogram=new TH2F("histogram","histogram",10,-1,1,10,-1,1);
   TH2F *ref=new TH2F("ref","ref",10,-1,1,10,-1,1);

	for(int i=1; i<11;i++){   
	for(int j=1; j<11;j++){   
   histogram->SetBinContent(i,j,0);
   histogram->SetBinContent(i,1,4);
   histogram->SetBinContent(4,10,3);
   histogram->SetBinContent(3,3,5);
   histogram->SetBinContent(4,3,5);
   histogram->SetBinContent(5,3,5);

   ref->SetBinContent(i,j,0);
   ref->SetBinContent(i,3,2);
	
	}
}
ref->Draw("text");
new TCanvas();
histogram->Draw("text");


   dqm_core::test::DummyAlgorithmConfig *aconfig = new dqm_core::test::DummyAlgorithmConfig(ref);
 
    aconfig->addParameter("MaskedBin",10);
    aconfig->addParameter("OkBin",1);
    aconfig->addParameter("TestRows",0);
    aconfig->addGreenThreshold("BinThreshold",1);
    aconfig->addRedThreshold("BinThreshold",10);
    dqm_core::Result * result = algorithm->execute( "test", *histogram, *aconfig );
   
    std::cout << "Result " << result->status_<< std::endl;    
    return result;
}
