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
   
  TCanvas c3;
  
  TCut muonHits("abs(Hits_MM_particleEncoding)==13");       
  TCut isSideA( "Hits_MM_detector_globalPositionZ>0");
  TCut isSideC( "Hits_MM_detector_globalPositionZ<0");
 
  nt->SetAlias("isMultiplet1", "Hits_MM_sim_multilayer==1" );  
  nt->SetAlias("isMultiplet2", "Hits_MM_sim_multilayer==2" );  

  for(int iPcsl=0; iPcsl<2; iPcsl++){
    TString typeSel;
    switch(iPcsl){
    case 0 : typeSel="isMultiplet1"; break;
    case 1 : typeSel="isMultiplet2";  break;
    }
     
    for(int iLayer=1; iLayer<=4; ++iLayer){
      c3->Clear();
      c3->cd();
      TString lSt;
      lSt.Form("Hits_MM_sim_layer==%d", iLayer);
      TCut layer(lSt) , typeSelCut(typeSel) ; 
    
      TString cTitle, hName;
      //    TString drawCommand("Hits_MM_hitGlobalPositionR:Hits_MM_hitGlobalPositionZ >>");
      TString drawCommand("Hits_MM_hitGlobalPositionZ/1000. >>");

      //////////////////////////////////
      cTitle.Form("Hits_MM_sideA_layer_%d_%s",iLayer, typeSel.Data());
     
      cout<<cTitle<<endl;
      hName.Form("h_%s",cTitle.Data());

      c3->Clear();
      c3->cd(); 

      TH1F hA(hName, cTitle ,20,7.,8.);
      cout<<"drawCommand : "<<drawCommand<<endl;
      cout<<"selection   : "<<(isSideA && typeSelCut && layer).GetTitle()<<endl;
      nt->Draw(drawCommand + hName, isSideA && typeSelCut && layer && muonHits );
      hA.Draw();
      c3->Update();
      c3->Print(plotDir+"/"+cTitle+".png");

      //////////////////////////////

      cTitle.Form("Hits_MM_sideC_layer_%d_%s",iLayer, typeSel.Data());
     
      cout<<cTitle<<endl;
      hName.Form("h_%s",cTitle.Data());

      c3->Clear();
      c3->cd(); 

      TH1F hC(hName, cTitle ,20,-8.,-7.);
      cout<<"drawCommand : "<<drawCommand<<endl;
      cout<<"selection   : "<<(isSideC && typeSelCut && layer).GetTitle()<<endl;
      nt->Draw(drawCommand + hName, isSideC && typeSelCut && layer && muonHits);
      hC.Draw();
      c3->Update();
      c3->Print(plotDir+"/"+cTitle+".png"); 
    } // end of multiplet 
  } // end for(iLayer)
 
}
