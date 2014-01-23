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

  TCanvas c1;
  TCanvas c2;
  TCanvas c3;
  TCanvas c4;

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

 
  TH2F* h33= new TH2F("h33","Odd Sectors-detector",100,-5000,5000,100,-5000,5000);
  TH2F* h34= new TH2F("h34","Even Sectors-detector",100,-5000,5000,100,-5000,5000);
  TH2F* h35= new TH2F("h35","Odd Sectors",100,-5000,5000,100,-5000,5000);
  TH2F* h36= new TH2F("h36","Even Sectors",100,-5000,5000,100,-5000,5000);


  TCut muonHits("abs(Hits_sTGC_particleEncoding)==13");

  TCut oddSectors( "abs(Hits_sTGC_detector_globalPositionZ)>7400 && (Hits_sTGC_sim_stationPhi==1" 
		   "||Hits_sTGC_sim_stationPhi==3"
		   "||Hits_sTGC_sim_stationPhi==5"
		   "||Hits_sTGC_sim_stationPhi==7"
		   "||Hits_sTGC_sim_stationPhi==9"
		   "||Hits_sTGC_sim_stationPhi==11"
		   "||Hits_sTGC_sim_stationPhi==13"
		   "||Hits_sTGC_sim_stationPhi==15 )"
		   );

  TCut evenSectors( "abs(Hits_sTGC_detector_globalPositionZ)<7400 && (Hits_sTGC_sim_stationPhi==2" 
		    "||Hits_sTGC_sim_stationPhi==4"
		    "||Hits_sTGC_sim_stationPhi==6"
		    "||Hits_sTGC_sim_stationPhi==8"
		    "||Hits_sTGC_sim_stationPhi==10"
		    "||Hits_sTGC_sim_stationPhi==12"
		    "||Hits_sTGC_sim_stationPhi==14"
		    "||Hits_sTGC_sim_stationPhi==16 )"
		    );


  c2->cd();

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector1","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==1" );
  sector1->Draw();
  c2->Print(plotDir+"/sTGC_sector_1.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector2","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==2");
  sector2->Draw();
  c2->Print(plotDir+"/sTGC_sector_2.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector3","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==3");
  sector3->Draw();
  c2->Print(plotDir+"/sTGC_sector_3.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector4","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==4");
  sector4->Draw();
  c2->Print(plotDir+"/sTGC_sector_4.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector5","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==5");
  sector5->Draw();
  c2->Print(plotDir+"/sTGC_sector_5.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector6","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==6");
  sector6->Draw();
  c2->Print(plotDir+"/sTGC_sector_6.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector7","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==7");
  sector7->Draw();
  c2->Print(plotDir+"/sTGC_sector_7.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector8","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==8");
  sector8->Draw();
  c2->Print(plotDir+"/sTGC_sector_8.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector9","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==9");
  sector9->Draw();
  c2->Print(plotDir+"/sTGC_sector_9.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector10","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==10");
  sector10->Draw();
  c2->Print(plotDir+"/sTGC_sector_10.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector11","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==11");
  sector11->Draw();
  c2->Print(plotDir+"/sTGC_sector_11.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector12","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==12");
  sector12->Draw();
  c2->Print(plotDir+"/sTGC_sector_12.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector13","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==13");
  sector13->Draw();
  c2->Print(plotDir+"/sTGC_sector_13.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector14","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==14");
  sector14->Draw();
  c2->Print(plotDir+"/sTGC_sector_14.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector15","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)>7400 && Hits_sTGC_sim_stationPhi==15");
  sector15->Draw();
  c2->Print(plotDir+"/sTGC_sector_15.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>sector16","abs(Hits_sTGC_particleEncoding)==13 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 && Hits_sTGC_sim_stationPhi==16");
  sector16->Draw();
  c2->Print(plotDir+"/sTGC_sector_16.png");

  c3->cd();

  nt->Draw("Hits_sTGC_detector_globalPositionY:Hits_sTGC_detector_globalPositionX>>h33", oddSectors && muonHits);
  h33->Draw();
  c3->Print(plotDir+"/sTGC_sectorsOdd-detector.png");

  nt->Draw("Hits_sTGC_detector_globalPositionY:Hits_sTGC_detector_globalPositionX>>h34", evenSectors && muonHits );
  h34->Draw();
  c3->Print(plotDir+"/sTGC_sectorsEven-detector.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h35", oddSectors && muonHits);
  h35->Draw();
  c3->Print(plotDir+"/sTGC_sectorsOdd.png");

  nt->Draw("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX>>h36", evenSectors && muonHits );
  h36->Draw();
  c3->Print(plotDir+"/sTGC_sectorsEven.png"); 



  nt->SetAlias("isSmall",      "abs(Hits_sTGC_hitGlobalPositionZ)<7400");
  nt->SetAlias("isLarge",      "(!isSmall)");
  nt->SetAlias("isSmallPivot", "(isSmall && abs(Hits_sTGC_hitGlobalPositionZ)>7200 && abs(Hits_sTGC_hitGlobalPositionZ)<7400 )");
  nt->SetAlias("isSmallConf",  "(isSmall && !isSmallPivot)");
  nt->SetAlias("isLargePivot", "(isLarge && ( abs(Hits_sTGC_hitGlobalPositionZ)>7400 && abs(Hits_sTGC_hitGlobalPositionZ)<7500 )");
  nt->SetAlias("isLargeConf",  "(isLarge && !isLargePivot)");



  for(int iPcsl=0; iPcsl<4; iPcsl++){
    TString typeSel;
    switch(iPcsl){
    case 0 : typeSel="isSmallPivot"; break;
    case 1 : typeSel="isSmallConf";  break;
    case 2 : typeSel="isLargePivot"; break;
    case 3 : typeSel="isLargeConf";  break;
    }
	
    for(int iLayer=1; iLayer<=4; ++iLayer){
      c1->Clear();
      c1->cd();
      TString lSt;
      lSt.Form("Hits_sTGC_sim_layer==%d", iLayer);
      TCut layer(lSt), typeSelCut(typeSel);
      TString cTitle, hName;
      TString drawCommand("Hits_sTGC_hitGlobalPositionY:Hits_sTGC_hitGlobalPositionX >>");
	
      cTitle.Form("Hits_sTGC_oddSector_layer%d_%s",iLayer, typeSel.Data());
      cout<<cTitle<<endl;
      hName.Form("h_%s",cTitle.Data());
	
      c1->Clear();
      c1->cd();
	
 
      TH2F hOdd(hName, cTitle, 10000,-5000,5000,10000,-5000,5000);
      cout<<"drawCommand : "<<drawCommand<<endl;
      cout<<"selection   : "<<(oddSectors && layer && typeSelCut && muonHits).GetTitle()<<endl;
      nt->Draw(drawCommand + hName, oddSectors && layer && typeSelCut && muonHits);
      hOdd.Draw();
      c1->Update();
      c1->Print(plotDir+"/"+cTitle+".png");
	
      cTitle.Form("Hits_sTGC_evenSector_layer%d_%s",iLayer, typeSel.Data());
      cout<<cTitle<<endl;
      hName.Form("h_%s",cTitle.Data());
 
      c1->Clear();
      c1->cd();
	
      TH2F hEven(hName, cTitle, 10000,-5000,5000,10000,-5000,5000);
      cout<<"drawCommand : "<<drawCommand<<endl;
      cout<<"selection   : "<<(evenSectors && layer && typeSelCut && muonHits).GetTitle()<<endl;
      nt->Draw(drawCommand + hName, evenSectors && layer && typeSelCut && muonHits);
      hEven.Draw();
      c1->Update();
      c1->Print(plotDir+"/"+cTitle+".png");
 
    } // end for(iLayer)
  } // end for(iPcsl)
	
 
  //** Check wedgeType **//
  TH1F* h7= new TH1F("h7","wedgePivot",1000,7000,8000);
  TH1F* h8= new TH1F("h8","wedgeConfirm",1000,7000,8000);
	
  c4->Divide(2,1);
  c4->cd(1);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h7","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_wedgeType==0");
  h7->Draw();
	
  c4->cd(2);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h8","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_wedgeType==1");
  h8->Draw();
	
  c4->Print(plotDir+"/sTGC_wedgeTypes.png");
	
	
  //** Check wedgeId **//
  TH1F* h5= new TH1F("h5","wedgeSmall",1000,7000,8000);
  TH1F* h6= new TH1F("h6","wedgeLarge",1000,7000,8000);
	
  c4->cd(1);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h5","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_wedgeId==0");
  h5->Draw();
	
  c4->cd(2);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h6","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_wedgeId==1");
  h6->Draw();
	
  c4->Print(plotDir+"/sTGC_wedgeIds.png");
	
	
  //** Check detectorNumber **//
  TH1F* h9= new TH1F("h9","Detector1_Small",5000,0,5000);
  TH1F* h10= new TH1F("h10","Detector2_Small",5000,0,5000);
  TH1F* h11= new TH1F("h11","Detector3_Small",5000,0,5000);
  TH1F* h12= new TH1F("h12","Detector4_Small",5000,0,5000);
  TH1F* h29= new TH1F("h29","Detector1_Large",5000,0,5000);
  TH1F* h30= new TH1F("h30","Detector2_Large",5000,0,5000);
  TH1F* h31= new TH1F("h31","Detector3_Large",5000,0,5000);
  TH1F* h32= new TH1F("h32","Detector4_Large",5000,0,5000);
	
  c1->Divide(2,2);
  c1->cd(1);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h9", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==1 && Hits_sTGC_wedgeId==0");
  h9->Draw();
	
  c1->cd(2);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h10", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==2 && Hits_sTGC_wedgeId==0");
  h10->Draw();
	
  c1->cd(3);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h11", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==3 && Hits_sTGC_wedgeId==0");
  h11->Draw();
	
  c1->cd(4);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h12", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==4 && Hits_sTGC_wedgeId==0");
  h12->Draw();
	
  c1->Print(plotDir+"/sTGC_detectorsSmall.png");
	
  c1->cd(1);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h29", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==1 && Hits_sTGC_wedgeId==1");
  h29->Draw();
	
  c1->cd(2);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h30", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==2 && Hits_sTGC_wedgeId==1");
  h30->Draw();
	
  c1->cd(3);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h31", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==3 && Hits_sTGC_wedgeId==1");
  h31->Draw();
	
  c1->cd(4);
  nt->Draw("(Hits_sTGC_hitGlobalPositionX^2+Hits_sTGC_hitGlobalPositionY^2)^(1/2)>>h32", "abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_detectorNumber==4 && Hits_sTGC_wedgeId==1");
  h32->Draw();
	
  c1->Print(plotDir+"/sTGC_detectorsLarge.png");
	
	
  //** Check layer **//
  TH1F* h1= new TH1F("h1","Layer1",800,7000,7800);
  TH1F* h2= new TH1F("h2","Layer2",800,7000,7800);
  TH1F* h3= new TH1F("h3","Layer3",800,7000,7800);
  TH1F* h4= new TH1F("h4","Layer4",800,7000,7800);
	
  c1->cd(1);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h1","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_sim_layer==1");
  h1->Draw();
	
  c1->cd(2);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h2","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_sim_layer==2");
  h2->Draw();
	
  c1->cd(3);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h3","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_sim_layer==3");
  h3->Draw();
	
  c1->cd(4);
  nt->Draw("Hits_sTGC_hitGlobalPositionZ>>h4","abs(Hits_sTGC_particleEncoding)==13 && Hits_sTGC_sim_layer==4");
  h4->Draw();
	
  c1->Print(plotDir+"/sTGC_layers.png");

}
