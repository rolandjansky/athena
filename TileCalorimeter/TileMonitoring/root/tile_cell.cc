/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void tile_cell() {

  TString path="/Tile/Cell";

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


 TCanvas *c_cellgen=new TCanvas("c_cellgen","Tile Cell General plots");
  c_cellgen->Divide(2,2);

  c_cellgen->cd(1);
  TH1D* ncell = (TH1D*)file->Get( path+"/tilenCells");  
  ncell->SetFillColor(38);
  ncell->Draw("histo");
  c_cellgen->Update();
  gSystem->Sleep(20);

  c_cellgen->cd(2);
  TH1D* cellene = (TH1D*)file->Get( path+"/tileCellEnergy");  
  cellene->SetFillColor(38);
  cellene->SetMinimum(0.);
  cellene->Draw("histo");
  c_cellgen->Update();
  gSystem->Sleep(20);

  c_cellgen->cd(3);
  TH1D* celltime = (TH1D*)file->Get( path+"/tileCellTime");  
  celltime->SetFillColor(38);
  celltime->Draw("histo");
  c_cellgen->Update();
  gSystem->Sleep(20);


  TCanvas *c_cellsamp=new TCanvas("c_cellsamp","Tile Cell Energy per Sampling");
  c_cellsamp->Divide(2,3);

  int index;

  enum Sampling {
    SampA     =0,
    SampB     =1,
    SampD     =2,
    SampE     =3,
    SampUnk   =4,
    AllSamp   =5,
    TotalSamp =6
  };

  TString SampStrNames[TotalSamp];
  SampStrNames[SampA]= "SampA"; SampStrNames[SampB]= "SampB" ; SampStrNames[SampD]= "SampD";
  SampStrNames[SampE]= "SampE"; SampStrNames[SampUnk]="SampUnk"; SampStrNames[AllSamp]= "AllSamp";

  for (int i=0;i<3;i++) {
    for (int j=0; j<2; j++) {

      index=i*2+(j+1);
      c_cellsamp->cd(index);

      TH1D* cellsamp = (TH1D*)file->Get( path+"/tileCellEne"+SampStrNames[index-1] );  
      //      cout << "cellsamp=" << cellsamp <<"\tcellsamp->GetTitle()=" << cellsamp->GetTitle();

      cellsamp->SetFillColor(38);
      cellsamp->Draw("histo");
      c_cellsamp->Update();

      gSystem->Sleep(20);
    }
  }


  TCanvas *c_celletaphi=new TCanvas("c_celletaphi","Tile Cell Eta Phi distribution");
  c_celletaphi->Divide(2,2);

  c_celletaphi->cd(1);
  TH1D* celleta = (TH1D*)file->Get( path+"/tileCellEneEta");  
  celleta->SetFillColor(38);
  celleta->SetMinimum(0.);
  celleta->Draw("histo");
  c_celletaphi->Update();
  gSystem->Sleep(20);

  c_celletaphi->cd(2);
  TH1D* cellphi = (TH1D*)file->Get( path+"/tileCellEnePhi");  
  cellphi->SetFillColor(38);
  cellphi->SetMinimum(0.);
  cellphi->Draw("histo");
  c_celletaphi->Update();
  gSystem->Sleep(20);

  c_celletaphi->cd(3);
  TH2D* celletaphi = (TH2D*)file->Get( path+"/tileCellEneEtaPhi");  
  celletaphi->Draw("zcol");
  c_celletaphi->Update();
  gSystem->Sleep(20);

  c_celletaphi->cd(4);
  TH2D* cellovt = (TH2D*)file->Get( path+"/tileCellEtaPhiOvThr");  
  cellovt->Draw("zcol");
  c_celletaphi->Update();
  gSystem->Sleep(20);

}





