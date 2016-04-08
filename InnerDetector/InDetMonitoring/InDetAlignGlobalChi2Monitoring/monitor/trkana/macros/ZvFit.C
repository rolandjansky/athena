/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
void ZvFit(char nname[80], int level=3)
{
  Set_style();
  

  // primer pas
  // eta
  TFile *f0 = new TFile(nname);
  TH2F *HD, *HT;
  char nameh[80];
  sprintf(nameh,"trkana/hRecoZ0_RecoEta_%d",level);
  HD = (TH2F*)f0->Get(nameh); 
  
  HD->GetYaxis()->SetAxisColor(2);
  HD->GetXaxis()->SetAxisColor(2);
  HD->GetYaxis()->SetLabelSize(0.03);
  HD->GetYaxis()->SetTitleSize(0.03);
  HD->GetYaxis()->SetTitleOffset(1.5);
  HD->GetXaxis()->SetLabelSize(0.03);
  HD->GetZaxis()->SetLabelSize(0.02);

  TCanvas *Canv = new TCanvas("Canv","Z0 vs #eta",585,585);
  Canv->SetGridx();
  Canv->SetGridy();

  HD->Draw("colz");

  TProfile *HProfi = HD->ProfileX("",-1,-1,""); 
  HProfi->Draw("same");
  HProfi->Fit("pol1","","",-0.9,0.9);

  HD->Draw("col");
  HProfi->Draw("same");
  HProfi->GetFunction("pol1")->SetLineColor(4);
  Canv->Print("ZvFit_eta.eps");
  
  // segon pas
  // theta
  TCanvas *Canw = new TCanvas("Z0_theta","Z0 vs #theta",585,585);
  sprintf(nameh,"trkana/hRecoZ0_RecoTheta_%d",level);
  HT = (TH2F*)f0->Get(nameh); 
  
  HT->GetYaxis()->SetAxisColor(2);
  HT->GetXaxis()->SetAxisColor(2);
  HT->GetYaxis()->SetLabelSize(0.03);
  HT->GetYaxis()->SetTitleSize(0.03);
  HT->GetYaxis()->SetTitleOffset(1.5);
  HT->GetXaxis()->SetLabelSize(0.03);
  HT->GetZaxis()->SetLabelSize(0.02);

  Canw->SetGridx();
  Canw->SetGridy();

  HT->Draw("colz");

  TProfile *HProft = HT->ProfileX();//"",-1,-1,""); 
  HProft->Draw("same");

  TF1 *MyRecta = new TF1 ("MyRecta", MyRecta, 1.2, 2.0, 2);
  MyRecta->SetLineColor(2);
  MyRecta->SetParameters(0.0, 0.);

  HProft->Fit("MyRecta","Rn");
  MyRecta->Draw("same");

  Canw->Print("ZvFit_theta.eps");
}


//////////////////////////////////////////////////
double MyRecta(double *x, double *par)
{
  double recta = par[0] + par[1]*(x[0]-TMath::PiOver2());
  return recta;
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

