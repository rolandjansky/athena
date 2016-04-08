/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//

TH1F *HD;
int level;
char name[80];
//
void EndcapPulls(char nname[80], int nivell = 3)
{
  level = nivell;
  
  Set_style();
  
  TFile *f0 = new TFile(nname);
  char nameh[80];
  TCanvas *PullsSCTEndcapA = new TCanvas("PullsSCTEndcapA","PullsSCTEndcapA",900,900);
  PullsSCTEndcapA->Divide(3,3);
  for(int disc=0;disc<9;disc++){
    sprintf(nameh,"trkana/SCTRes/hSCTPulEndCap0Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsSCTEndcapA->cd(disc+1);
    pinta_m();
  }
  char nameh[80];
  TCanvas *PullsSCTEndcapC = new TCanvas("PullsSCTEndcapC","PullsSCTEndcapC",900,900);
  PullsSCTEndcapC->Divide(3,3);
  for(int disc=0;disc<9;disc++){
    sprintf(nameh,"trkana/SCTRes/hSCTPulEndCap1Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsSCTEndcapC->cd(disc+1);
    pinta_m();
  }
  TCanvas *PullsPIXEndcapA = new TCanvas("PullsPIXEndcapA","PullsPIXEndcapA",900,600);
  PullsPIXEndcapA->Divide(3,2);
  for(int disc=0;disc<3;disc++){
    sprintf(nameh,"trkana/PIXRes/hPIXPulPhiEndCap0Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapA->cd(disc+1);
    pinta_m();
    sprintf(nameh,"trkana/PIXRes/hPIXPulEtaEndCap0Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapA->cd(disc+4);
    pinta_m();
  }
  
  TCanvas *PullsPIXEndcapC = new TCanvas("PullsPIXEndcapC","PullsPIXEndcapC",900,600);
  PullsPIXEndcapC->Divide(3,2);
  for(int disc=0;disc<3;disc++){
    sprintf(nameh,"trkana/PIXRes/hPIXPulPhiEndCap1Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapC->cd(disc+1);
    pinta_m();
    sprintf(nameh,"trkana/PIXRes/hPIXPulEtaEndCap1Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapC->cd(disc+4);
    pinta_m();
  }
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
void pinta_m()
{
  HD->Draw();
  HD->GetYaxis()->SetAxisColor(2);
  HD->GetXaxis()->SetAxisColor(2);
  HD->GetYaxis()->SetLabelSize(0.03);
  HD->GetYaxis()->SetTitleSize(0.03);
  HD->GetYaxis()->SetTitleOffset(1.5);
  HD->GetXaxis()->SetLabelSize(0.03);
  if(HD->GetEntries()!=0){
    HD->Fit("gaus");
    TF1 *fit = HD->GetFunction("gaus");
    float mean=fit->GetParameter(1);
    float sigma=fit->GetParameter(2);
    TPaveText *fitParams = new TPaveText(0.64,0.68,0.98,0.78,"brNDC");
    fitParams->SetFillColor(0);
    fitParams->SetTextAlign(12);
    fitParams->SetTextSize(0.033);
    sprintf(name," - Mean: %2.3f mm",mean);
    fitParams->AddText(name);
    sprintf(name," - Sigma: %2.3f mm",sigma);
    fitParams->AddText(name);
    fitParams->Draw();
  }
}

