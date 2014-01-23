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

  TCanvas c1;
  TCanvas c2;
  TCanvas c3;
  TCanvas c4;
 
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
		
  nt->SetAlias("isSmall",      "abs(Hits_MM_detector_globalPositionZ)>7400 && (Hits_MM_sim_stationPhi==1" 
	       "||Hits_MM_sim_stationPhi==3"
	       "||Hits_MM_sim_stationPhi==5"
	       "||Hits_MM_sim_stationPhi==7"
	       "||Hits_MM_sim_stationPhi==9"
	       "||Hits_MM_sim_stationPhi==11"
	       "||Hits_MM_sim_stationPhi==13"
	       "||Hits_MM_sim_stationPhi==15 )") ;

  nt->SetAlias("isLarge", "abs(Hits_MM_detector_globalPositionZ)<7400 && (Hits_MM_sim_stationPhi==2" 
	       "||Hits_MM_sim_stationPhi==4"
	       "||Hits_MM_sim_stationPhi==6"
	       "||Hits_MM_sim_stationPhi==8"
	       "||Hits_MM_sim_stationPhi==10"
	       "||Hits_MM_sim_stationPhi==12"
	       "||Hits_MM_sim_stationPhi==14"
	       "||Hits_MM_sim_stationPhi==16 )" ); 
   
   
  nt->SetAlias("isSmallSideA", "abs(Hits_MM_detector_globalPositionZ)>7400 && (Hits_MM_sim_stationPhi==1" 
	       "||Hits_MM_sim_stationPhi==3"
	       "||Hits_MM_sim_stationPhi==5"
	       "||Hits_MM_sim_stationPhi==7"
	       "||Hits_MM_sim_stationPhi==9"
	       "||Hits_MM_sim_stationPhi==11"
	       "||Hits_MM_sim_stationPhi==13"
	       "||Hits_MM_sim_stationPhi==15 ) && Hits_MM_side>0") ;

  nt->SetAlias("isLargeSideA", "abs(Hits_MM_detector_globalPositionZ)<7400 && (Hits_MM_sim_stationPhi==2" 
	       "||Hits_MM_sim_stationPhi==4"
	       "||Hits_MM_sim_stationPhi==6"
	       "||Hits_MM_sim_stationPhi==8"
	       "||Hits_MM_sim_stationPhi==10"
	       "||Hits_MM_sim_stationPhi==12"
	       "||Hits_MM_sim_stationPhi==14"
	       "||Hits_MM_sim_stationPhi==16 )&& Hits_MM_detector_globalPositionZ>0" );  
   

  nt->SetAlias("isSmallSideC", "abs(Hits_MM_detector_globalPositionZ)>7400 && (Hits_MM_sim_stationPhi==1" 
	       "||Hits_MM_sim_stationPhi==3"
	       "||Hits_MM_sim_stationPhi==5"
	       "||Hits_MM_sim_stationPhi==7"
	       "||Hits_MM_sim_stationPhi==9"
	       "||Hits_MM_sim_stationPhi==11"
	       "||Hits_MM_sim_stationPhi==13"
	       "||Hits_MM_sim_stationPhi==15 ) && Hits_MM_side<0") ;

  nt->SetAlias("isLargeSideC", "abs(Hits_MM_detector_globalPositionZ)<7400 && (Hits_MM_sim_stationPhi==2" 
	       "||Hits_MM_sim_stationPhi==4"
	       "||Hits_MM_sim_stationPhi==6"
	       "||Hits_MM_sim_stationPhi==8"
	       "||Hits_MM_sim_stationPhi==10"
	       "||Hits_MM_sim_stationPhi==12"
	       "||Hits_MM_sim_stationPhi==14"
	       "||Hits_MM_sim_stationPhi==16 )&& Hits_MM_detector_globalPositionZ<0" );  


  for(int iPcsl=0; iPcsl<6; iPcsl++){
    TString typeSel;
    switch(iPcsl){
    case 0 : typeSel="isSmall"; break;
    case 1 : typeSel="isLarge";  break;
    case 2 : typeSel="isSmallSideA";  break;
    case 3 : typeSel="isLargeSideA";  break;
    case 4 : typeSel="isSmallSideC";  break;
    case 5 : typeSel="isLargeSideC";  break;
    }

    for(int iLayer=1; iLayer<=4; ++iLayer){
      c3->Clear();
      c3->cd();
      TString lSt;
      lSt.Form("Hits_MM_sim_layer==%d", iLayer);
      TCut layer(lSt) , typeSelCut(typeSel) ;
      TString cTitle, hName;
      TString drawCommand("Hits_MM_hitGlobalPositionR:Hits_MM_hitGlobalPositionZ >>");

      cTitle.Form("Hits_MM_oddSector_layer_%d_%s",iLayer, typeSel.Data());
  
      cout<<cTitle<<endl;
      hName.Form("h_%s",cTitle.Data());

      c3->Clear();
      c3->cd();

      TH2F hOdd(hName, cTitle, 100,-8000,8000,100,-8000,8000);
      cout<<"drawCommand : "<<drawCommand<<endl;
      cout<<"selection   : "<<(oddSectors && layer && typeSelCut ).GetTitle()<<endl;
      nt->Draw(drawCommand + hName, oddSectors && layer && typeSelCut && muonHits );
      hOdd.Draw();
      c3->Update();
      c3->Print(plotDir+"/"+cTitle+".png");
 
      cTitle.Form("Hits_MM_evenSector_layer%d_%s",iLayer , typeSel.Data()  );
      cout<<cTitle<<endl;
      hName.Form("h_%s",cTitle.Data());
      c3->Clear();
      c3->cd();

      TH2F hEven(hName, cTitle, 100,-8000,8000,100,-8000,8000);
      cout<<"drawCommand : "<<drawCommand<<endl;
      cout<<"selection   : "<<(evenSectors && layer && typeSelCut).GetTitle()<<endl;
      nt->Draw(drawCommand + hName, evenSectors && layer && typeSelCut && muonHits);
      hEven.Draw();
      c3->Update();
      c3->Print(plotDir+"/"+cTitle+".png");
    } // end for(iLayer)
  } // end for(iPcsl) 
}
