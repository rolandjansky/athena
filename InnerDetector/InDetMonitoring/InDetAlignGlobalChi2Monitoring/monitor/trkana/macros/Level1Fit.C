/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
void Level1Fit(char nname[80], int level=3)
{
  Set_style();
  
  TFile *f0 = new TFile(nname);
  TH2F *HD;
  char nameh[80];
  sprintf(nameh,"trkana/hRecoD0_RecoPhi0_%d",level);
  HD = (TH2F*)f0->Get(nameh); 
  
  HD->GetYaxis()->SetAxisColor(2);
  HD->GetXaxis()->SetAxisColor(2);
  HD->GetYaxis()->SetLabelSize(0.03);
  HD->GetYaxis()->SetTitleSize(0.03);
  HD->GetYaxis()->SetTitleOffset(1.5);
  HD->GetXaxis()->SetLabelSize(0.03);
  HD->GetZaxis()->SetLabelSize(0.02);

  TCanvas *CanL1F = new TCanvas("CanL1F","D0 vs #phi0",585,585);
  CanL1F->SetGridx();
  CanL1F->SetGridy();

  HD->Draw("colz");

  TProfile *HProfi = HD->ProfileX("",-1,-1,""); 
  HProfi->Draw("same");
  //HProfi->Draw();

  // definim la funcio a ajustar
  TF1 *Vtx2DFun = new TF1 ("Vtx2D", Vtx2D_fitfun, 0., 0., 2);
  Vtx2DFun->SetLineColor(1);
  Vtx2DFun->SetParameters(0.0, 0., 0.);
  int D0MaxBin = HProfi->GetMaximumBin();
  cout << "bin " << D0MaxBin << endl;

  Vtx2DFun->SetParName(0,"X_v");

  Vtx2DFun->SetParName(1,"Y_v");
  //Vtx2DFun->SetParameter(1,HProfi->GetBinContent(D0MaxBin));

  //Vtx2DFun->SetParName(1,"Phi00");
  //Vtx2DFun->SetParameter(1,-HProfi->GetBinCenter(D0MaxBin));

  HProfi->Fit("Vtx2D","n");
  Vtx2DFun->Draw("same");
  
  //  cout << " X_v = " <<  -Vtx2DFun->GetParameter(0)*sin(Vtx2DFun->GetParameter(1)) << endl;
  // cout << " Y_v = " <<   Vtx2DFun->GetParameter(0)*cos(Vtx2DFun->GetParameter(1)) << endl;

  CanL1F->Print("Level1Fit.eps");
}
//////////////////////////////////////////////////
void Set_style()
{
  // style
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  //gStyle->SetOptFit(10);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  //  gStyle->SetStatFontSize(0);
  gStyle->SetStatColor(0);
  gStyle->SetStatW(0.22);
  //gStyle->SetStatH(0.22);
}

//////////////////////////////////////////////////
double Vtx2D_fitfun(double *x, double *par)
{
  double Vtx2D;

  
  double X_V = par[0];
  double Y_V = par[1];
  double R_V = 0.;

  Vtx2D =  - X_V * sin(x[0]) + Y_V * cos(x[0]) + R_V;
  /*
  double R_v = par[0];
  double Phi00 = par[1];

  Vtx2D =  R_v * sin(x[0]+Phi00);
  */
  return Vtx2D;
}
