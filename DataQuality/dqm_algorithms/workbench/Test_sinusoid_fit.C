/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Test_sinusoid_fit.C
// Macro to test the sinusoid fit
// Cristobal Cuenca Almenar, November 29th 2009

int  Test_sinusoid_fit()
{ 

  //load create 
  gSystem->Load( "libdqm_core.so" );
  gSystem->Load( "libdqm_dummy.so" );
  gSystem->Load( "libdqm_tools.so" );
  gSystem->Load( "libdqm_algorithms.so" );
  gROOT->ProcessLine( ".include .." );

  
  TCanvas* c = new TCanvas();
  c->Divide(2,1);

	
  //algo
  dqm_algorithms::Simple_sinusoid_Fit * algorithm = new dqm_algorithms::Simple_sinusoid_Fit();  
 

  //create function to fill the histogram
  c->cd(1);
  TF1* f_sinusoid = new TF1("pep","[0]*sin(x) + [1]*cos(x)",-TMath::Pi(),TMath::Pi());
  f_sinusoid->SetParameters(4.0,2.0);
  f_sinusoid->Draw();


  //create histogram
  TH1F * histogram = new TH1F( "histogram", "Simple sinusoid histogram", 100, -4.0, 4.0 );
  //TH1F * histogram = new TH1F( "histogram", "Simple sinusoid histogram", 100, -TMath::Pi(),TMath::Pi() );
  for (int iBin=1; iBin<=100; iBin++) {
    histogram->SetBinContent(iBin, f_sinusoid->Eval(histogram->GetBinLowEdge(iBin)));
  }
  c->cd(2);
  histogram->Draw();
  

  //dummy configuration
  dqm_core::test::DummyAlgorithmConfig * aconfig = new dqm_core::test::DummyAlgorithmConfig( );
  
  // Optional Parameters, Fit range
  aconfig->addParameter("MinStat",100);
  //aconfig->addParameter("xmin",-1);
  //aconfig->addParameter("xmax",1);
  
  // Thresholds for Fit Parameters...Only put thresholds on values you'd like to check....others will not be checked
  // Threshold for s1 and s2
  aconfig->addGreenThreshold("s1",3.5);
  aconfig->addRedThreshold("s1",2.5);
  aconfig->addGreenThreshold("s2",1.0);
  aconfig->addRedThreshold("s2",0.0);


  //run check
  dqm_core::Result * result = algorithm->execute( "sinusoid1", *histogram, *aconfig );
  std::cout << "Result " << result->status_ << std::endl;

  //can't read maps... so no tags
//   std::map<std::string,double> tags = result->tags_;
//   std::cout<<" tags.size()="<<tags.size();

//   std::map<std::string,double>::iterator it=tags.begin();
//   int i=0;
//   for ( ; it!=tags.end(); it++, i++)
//     std::cout<<"Tag "<<i
// 	     <<" named "<<it->first
// 	     <<" value "<<it->second<<std::endl;
   
  
  return 1;
}
