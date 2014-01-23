/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  gStyle->SetMarkerColor(1);
  gStyle->SetOptStat(1);
  
  // ATLAS Style

  Int_t icol = 0;
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);
	
  // set the paper & margin sizes
  gStyle->SetPaperSize(20, 26);
	
  // set margin sizes
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);
	
  // set title offsets (for axis label)
  gStyle->SetTitleXOffset(1.4);
  gStyle->SetTitleYOffset(1.4);
	
  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font = 42; // Helvetica
  Double_t tsize = 0.05;
  gStyle->SetTextFont(font);
	
  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font, "x");
  gStyle->SetTitleFont(font, "x");
  gStyle->SetLabelFont(font, "y");
  gStyle->SetTitleFont(font, "y");
  gStyle->SetLabelFont(font, "z");
  gStyle->SetTitleFont(font, "z");
	
  gStyle->SetLabelSize(tsize, "x");
  gStyle->SetTitleSize(tsize, "x");
  gStyle->SetLabelSize(tsize, "y");
  gStyle->SetTitleSize(tsize, "y");
  gStyle->SetLabelSize(tsize, "z");
  gStyle->SetTitleSize(tsize, "z");
	
  // use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth((Width_t) 2.);
  gStyle->SetLineStyleString(2, "[12 12]"); // postscript dashes
	
  // get rid of X error bars
  //gStyle->SetErrorX(0.001);
  // get rid of error bar caps
  //gStyle->SetEndErrorSize(0.);
	
  // do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);
	
  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
	
  // end of ATLAS-style  
  
  std::string fileName = "/afs/cern.ch/work/j/jomeyer/public/NSW/MC12.107209.ParticleGenerator_dimu_Pt10_100/NSWPRDValAlg_allDet.root";
  TString plotDir = "./plots";
  TFile *_file0 = new TFile( fileName.c_str() );
  std::cout << "Opening file " << fileName.c_str() << std::endl;
  
  TTree *nt = (TTree*)_file0->Get("NSWHitsTree");  
  std::cout<<"Added "<<nt->GetEntries()<<" entries" << std::endl;

  TCanvas c3;
 
  TH2F* h33= new TH2F("h33","Odd Sectors-detector",100,-5000,5000,100,-5000,5000);
  TH2F* h34= new TH2F("h34","Even Sectors-detector",100,-5000,5000,100,-5000,5000);
  TH2F* h35= new TH2F("h35","Odd Sectors",100,-5000,5000,100,-5000,5000);
  TH2F* h36= new TH2F("h36","Even Sectors",100,-5000,5000,100,-5000,5000);

  c3->cd();

  TCut muonHits("abs(Hits_MM_particleEncoding)==13");

  TCut oddSectors( "abs(Hits_MM_detector_globalPositionZ)>7400 && (Hits_MM_sim_stationPhi==1" 
		   "||Hits_MM_sim_stationPhi==3"
		   "||Hits_MM_sim_stationPhi==5"
		   "||Hits_MM_sim_stationPhi==7"
		   "||Hits_MM_sim_stationPhi==9"
		   "||Hits_MM_sim_stationPhi==11"
		   "||Hits_MM_sim_stationPhi==13"
		   "||Hits_MM_sim_stationPhi==15 )"
		   );

  TCut evenSectors( "abs(Hits_MM_detector_globalPositionZ)<7400 && (Hits_MM_sim_stationPhi==2" 
		    "||Hits_MM_sim_stationPhi==4"
		    "||Hits_MM_sim_stationPhi==6"
		    "||Hits_MM_sim_stationPhi==8"
		    "||Hits_MM_sim_stationPhi==10"
		    "||Hits_MM_sim_stationPhi==12"
		    "||Hits_MM_sim_stationPhi==14"
		    "||Hits_MM_sim_stationPhi==16 )"
		    );


  nt->Draw("Hits_MM_detector_globalPositionY:Hits_MM_detector_globalPositionX>>h33", oddSectors && muonHits);
  h33->Draw();
  c3->Print(plotDir+"/Hits_MM_sectorsOdd-detector.png");

  nt->Draw("Hits_MM_detector_globalPositionY:Hits_MM_detector_globalPositionX>>h34", evenSectors && muonHits );
  h34->Draw();
  c3->Print(plotDir+"/Hits_MM_sectorsEven-detector.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>h35", oddSectors && muonHits);
  h35->Draw();
  c3->Print(plotDir+"/Hits_MM_sectorsOdd.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>h36", evenSectors && muonHits );
  h36->Draw();
  c3->Print(plotDir+"/Hits_MM_sectorsEven.png"); 
}
