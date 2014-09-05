/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void tile_muid() {

  TString path="/Tile/Muid";

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



 

  TString StrNames[19];
  StrNames[0]= "tileMuidTags"; StrNames[1]= "tileMuidEnergy" ; StrNames[2]= "tileMuidQuality";
  StrNames[3]="tileMuidEneQuality";   StrNames[4]= "tileMuidEta";  StrNames[5]= "tileMuidPhi";
  StrNames[6]= "tileMuidEtaPhi";
  StrNames[7]= "tileMuid1TagEnergy" ; StrNames[8]= "tileMuid1TagQuality";
  StrNames[9]="tileMuid1TagEneQuality";   StrNames[10]= "tileMuid1TagEta";  StrNames[11]= "tileMuid1TagPhi";
  StrNames[12]= "tileMuid1TagEtaPhi"; 
  StrNames[13]= "tileMuid2TagEnergy" ; StrNames[14]= "tileMuid2TagQuality";
  StrNames[15]="tileMuid2TagEneQuality";   StrNames[16]= "tileMuid2TagEta";  StrNames[17]= "tileMuid2TagPhi";
  StrNames[18]= "tileMuid2TagEtaPhi"; 

  int index;
  TCanvas *c_muidgen=new TCanvas("c_muidgen","Tile Muid General");
  c_muidgen->Divide(2,4);

  for (int i=0;i<7;i++) {

      index=i+1;
      c_muidgen->cd(index);

      TH1D* muidgen = (TH1D*)file->Get( path+"/"+StrNames[i] );  
      //      cout << "cellsamp=" << cellsamp <<"\tcellsamp->GetTitle()=" << cellsamp->GetTitle();


      if (i!=6 && i!=3) {muidgen->SetFillColor(38);muidgen->Draw("histo");}
      else { muidgen->Draw("zcol");}
      c_muidgen->Update();
      gSystem->Sleep(200);
   
  }


  TCanvas *c_muidene=new TCanvas("c_muidene","Tile Muid nTAGS=1 histograms");
  c_muidene->Divide(2,3);

  for (int i=7;i<13;i++) {
    index=i-6;
    c_muidene->cd(index);
    TH1D* muidene =  (TH1D*)file->Get( path+"/"+StrNames[i] ); 
    if (i!=9 && i!=12 ) { muidene->SetFillColor(38); muidene->Draw("histo");}
    else {muidene->Draw("zcol");}
    c_muidene->Update();
    gSystem->Sleep(200);

  }


  TCanvas *c_muidene2=new TCanvas("c_muidene2","Tile Muid nTAGS=2 histograms");
  c_muidene2->Divide(2,3);

  for (int i=13;i<19;i++) {
    index=i-12;
    c_muidene2->cd(index);
    TH1D* muidene2 =  (TH1D*)file->Get( path+"/"+StrNames[i] ); 
    if (i!=15 && i!=18 ) { muidene2->SetFillColor(38); muidene2->Draw("histo");}
    else {muidene2->Draw("zcol");}
    c_muidene2->Update();
    gSystem->Sleep(200);

  }

}





