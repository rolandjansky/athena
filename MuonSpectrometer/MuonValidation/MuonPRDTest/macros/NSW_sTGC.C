/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  
  std::string fileName = "/afs/cern.ch/work/j/jomeyer/public/NSW/MC12.107209.ParticleGenerator_dimu_Pt10_100/NSWPRDValAlg_allDet.root";
  TString plotDir = "./plots";
  TFile *_file0 = new TFile( fileName.c_str() );
  std::cout << "Opening file " << fileName.c_str() << std::endl;
  
  TTree *nt = (TTree*)_file0->Get("NSWHitsTree");  
  std::cout<<"Added "<<nt->GetEntries()<<" entries" << std::endl;

 
 
  TH1F* h0= new TH1F("h0","h0",1000,7000,8000);
  TCanvas c2;
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h0");
  h0->Draw();
	
  /*
    TH2F* h1= new TH2F("h_large_front","h_large_front",10000,-5000,5000,10000,-5000,5000);
    TH2F* h2= new TH2F("h_large_back","h_large_back",10000,-5000,5000,10000,-5000,5000);
    TH2F* h3= new TH2F("h_small_front","h_small_front",10000,-5000,5000,10000,-5000,5000);
    TH2F* h4= new TH2F("h_small_back","h_small_back",10000,-5000,5000,10000,-5000,5000);
	
    nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h_large_front","Hits_sTGC_hitGlobalPositionZ>7060 && Hits_sTGC_hitGlobalPositionZ<7100");
    nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h_large_back","Hits_sTGC_hitGlobalPositionZ>7360 && Hits_sTGC_hitGlobalPositionZ<7400");
    nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h_small_front","Hits_sTGC_hitGlobalPositionZ>7460 && Hits_sTGC_hitGlobalPositionZ<7500");
    nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h_small_back","Hits_sTGC_hitGlobalPositionZ>7760 && Hits_sTGC_hitGlobalPositionZ<7800");
	
    TCanvas c1;
    c1->Divide(2,2);
    c1->cd(1);
    h1->Draw();
    c1->cd(2);
    h2->Draw();
    c1->cd(3);
    h3->Draw();
    c1->cd(4);
    h4->Draw();
	
    TCanvas c2;
    h0->Draw();
    TCanvas c2;
    h0->Draw();
  */
	
  //plotting local coordinate for one wedge (large, #05)
	 
  TH2F* h5= new TH2F("h_global_large_front_05","h_global_large_front_05",2000,-1000,1000,5000,0,5000);
  TH2F* h6= new TH2F("h_local_large_front_05","h_local_large_front_05",2000,-1000,1000,2000,-1000,1000);
	
  TCut sel05("Hits_sTGC_hitGlobalPositionZ>7060 && Hits_sTGC_hitGlobalPositionZ<7100 && ((Hits_sTGC_hitGlobalPositionY<1600 && Hits_sTGC_hitGlobalPositionY>1000 && abs(Hits_sTGC_hitGlobalPositionX)<500) || (Hits_sTGC_hitGlobalPositionY >=1600 && abs(Hits_sTGC_hitGlobalPositionX)<1000 )) ");
	
  TCut sel05_U1("Hits_sTGC_hitGlobalPositionZ>7060 && Hits_sTGC_hitGlobalPositionZ<7100 && ((Hits_sTGC_hitGlobalPositionY<1000+816 && Hits_sTGC_hitGlobalPositionY>1000 && abs(Hits_sTGC_hitGlobalPositionX)<890/2)) ");
	
  TCut sel_layer1("Hits_sTGC_hitGlobalPositionZ>7060 && Hits_sTGC_hitGlobalPositionZ<7070");
	
  TCanvas c3; 
  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h_global_large_front_05",sel05_U1 && sel_layer1);
  h5->Draw();
	
  TCanvas c4;
  nt->Draw("Hits_sTGC_hitLocalPositionX:Hits_sTGC_hitLocalPositionY>>h_local_large_front_05",sel05_U1 && sel_layer1);
  h6->Draw();
	
}
	
