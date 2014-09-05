/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void tile_tower() {

  TString path="/Tile/Tower";

  file->cd(path);

  //===== switch for outputs =====
  //  bool gifOn=false; 
  //  bool epsOn=true;
  
  gROOT->SetStyle("Plain");
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadTopMargin   (0.20);
  gStyle->SetStatFontSize  (0.05);
  gStyle->SetStatStyle     (0);
  gStyle->SetOptStat(111111);  
  gStyle->SetOptFit(1100);  
  gStyle->SetTitleH         (0.1);
  gStyle->SetCanvasDefW(900);
  gStyle->SetCanvasDefH(600);
  gStyle->SetPalette(1);



 

  TString StrNames[10];
  StrNames[0]= "tilenTowers"; StrNames[1]= "tileTowersnCells" ; StrNames[2]= "tileAllTowerEnergy";
  StrNames[3]="tileTowerEnergy";   StrNames[4]= "tileTowerEt";
  StrNames[5]= "tileTowerEta"; StrNames[6]= "tileTowerPhi"; StrNames[7]= "tileTowerEtaPhi";
  StrNames[8]= "tileTowerEtaPhiDiff"; StrNames[9]="tileTowerEneDiff"; 


  int index;
  TCanvas *c_towergen=new TCanvas("c_towergen","Tile Tower General");
  c_towergen->Divide(2,2);

  TVirtualPad* tvpad;
  for (int i=0;i<4;i++) {

      index=i+1;
      tvpad= c_towergen->cd(index);

      TH1D* towergen = (TH1D*)file->Get( path+"/"+StrNames[i] );  
      //      cout << "cellsamp=" << cellsamp <<"\tcellsamp->GetTitle()=" << cellsamp->GetTitle();

      towergen->SetFillColor(38); 
      towergen->Draw("histo");
      c_towergen->Update();
      if (i==2 || i==3) {tvpad->SetLogy(1);}
      gSystem->Sleep(200);
   
  }


  TCanvas *c_towerene=new TCanvas("c_towerene","Tile Tower histograms");
  c_towerene->Divide(2,3);

  for (int i=4;i<10;i++) {
    index=i-3;
    c_towerene->cd(index);
    TH1D* towerene =  (TH1D*)file->Get( path+"/"+StrNames[i] ); 
    towerene->SetFillColor(38); if (i==4) {gPad->SetLogy(1);}
    if (i!=7 && i!=8)  {towerene->Draw("histo");}
    else {towerene->Draw("zcol");}
    c_towerene->Update();
    gSystem->Sleep(200);

  }

}





