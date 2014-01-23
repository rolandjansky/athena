/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
 
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
    
  TCanvas c2;
  
  //** Check sectorNumber **//
  TH2F* sector1= new TH2F("sector1","Sector1",100,-5000,5000,100,-5000,5000);
  TH2F* sector2= new TH2F("sector2","Sector2",100,-5000,5000,100,-5000,5000);
  TH2F* sector3= new TH2F("sector3","Sector3",100,-5000,5000,100,-5000,5000); 
  TH2F* sector4= new TH2F("sector4","Sector4",100,-5000,5000,100,-5000,5000);
  TH2F* sector5= new TH2F("sector5","Sector5",100,-5000,5000,100,-5000,5000);
  TH2F* sector6= new TH2F("sector6","Sector6",100,-5000,5000,100,-5000,5000);
  TH2F* sector7= new TH2F("sector7","Sector7",100,-5000,5000,100,-5000,5000);
  TH2F* sector8= new TH2F("sector8","Sector8",100,-5000,5000,100,-5000,5000);
  TH2F* sector9= new TH2F("sector9","Sector9",100,-5000,5000,100,-5000,5000);
  TH2F* sector10= new TH2F("sector10","Sector10",100,-5000,5000,100,-5000,5000);
  TH2F* sector11= new TH2F("sector11","Sector11",100,-5000,5000,100,-5000,5000);
  TH2F* sector12= new TH2F("sector12","Sector12",100,-5000,5000,100,-5000,5000);
  TH2F* sector13= new TH2F("sector13","Sector13",100,-5000,5000,100,-5000,5000);
  TH2F* sector14= new TH2F("sector14","Sector14",100,-5000,5000,100,-5000,5000);
  TH2F* sector15= new TH2F("sector15","Sector15",100,-5000,5000,100,-5000,5000);
  TH2F* sector16= new TH2F("sector16","Sector16",100,-5000,5000,100,-5000,5000);

  c2->cd();

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector1","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==1" );
  sector1->Draw();
  c2->Print(plotDir+"/MM_sector_1.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector2","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==2");
  sector2->Draw();
  c2->Print(plotDir+"/MM_sector_2.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector3","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==3");
  sector3->Draw();
  c2->Print(plotDir+"/MM_sector_3.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector4","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==4");
  sector4->Draw();
  c2->Print(plotDir+"/MM_sector_4.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector5","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==5");
  sector5->Draw();
  c2->Print(plotDir+"/MM_sector_5.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector6","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==6");
  sector6->Draw();
  c2->Print(plotDir+"/MM_sector_6.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector7","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==7");
  sector7->Draw();
  c2->Print(plotDir+"/MM_sector_7.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector8","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==8");
  sector8->Draw();
  c2->Print(plotDir+"/MM_sector_8.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector9","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==9");
  sector9->Draw();
  c2->Print(plotDir+"/MM_sector_9.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector10","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==10");
  sector10->Draw();
  c2->Print(plotDir+"/MM_sector_10.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector11","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==11");
  sector11->Draw();
  c2->Print(plotDir+"/MM_sector_11.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector12","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==12");
  sector12->Draw();
  c2->Print(plotDir+"/MM_sector_12.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector13","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==13");
  sector13->Draw();
  c2->Print(plotDir+"/MM_sector_13.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector14","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==14");
  sector14->Draw();
  c2->Print(plotDir+"/MM_sector_14.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector15","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)>7400 && Hits_MM_sim_stationPhi==15");
  sector15->Draw();
  c2->Print(plotDir+"/MM_sector_15.png");

  nt->Draw("Hits_MM_hitGlobalPositionY:Hits_MM_hitGlobalPositionX>>sector16","abs(Hits_MM_particleEncoding)==13 && abs(Hits_MM_hitGlobalPositionZ)<7400 && Hits_MM_sim_stationPhi==16");
  sector16->Draw();
  c2->Print(plotDir+"/MM_sector_16.png");
  
}
