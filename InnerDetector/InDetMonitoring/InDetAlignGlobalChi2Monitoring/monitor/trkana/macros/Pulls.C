/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//

TH1F *HD;
int level;
char name[80];
char GifFile[80];
//
void Pulls(char nname[80], int level = 3)
{
  Set_style();

  
  TFile *f0 = new TFile(nname);
  char nameh[80];
  TCanvas *PullsSCTBarrel = new TCanvas("PullsSCTBarrel","PullsSCTBarrel",600,600);
  PullsSCTBarrel->Divide(2,2);
  for(int layer=0;layer<4;layer++){
    sprintf(nameh,"trkana/SCTRes/hSCTPulBaLay%d_%d",layer,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsSCTBarrel->cd(layer+1);
    pinta_m();
  }
  sprintf(GifFile,"PullsSCTBarrelSel%d.gif",level);
  PullsSCTBarrel->Print(GifFile);
  
  TCanvas *PullsSCTEndcapA = new TCanvas("PullsSCTEndcapA","PullsSCTEndcapA",900,900);
  PullsSCTEndcapA->Divide(3,3);
  for(int disc=0;disc<9;disc++){
    sprintf(nameh,"trkana/SCTRes/hSCTPulEndcap0Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsSCTEndcapA->cd(disc+1);
    pinta_m();
  }
  sprintf(GifFile,"PullsSCTEndcapASel%d.gif",level);
  PullsSCTEndcapA->Print(GifFile);

  TCanvas *PullsSCTEndcapC = new TCanvas("PullsSCTEndcapC","PullsSCTEndcapC",900,900);
  PullsSCTEndcapC->Divide(3,3);
  for(int disc=0;disc<9;disc++){
    sprintf(nameh,"trkana/SCTRes/hSCTPulEndcap1Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsSCTEndcapC->cd(disc+1);
    pinta_m();
  }
  sprintf(GifFile,"PullsSCTEndcapCSel%d.gif",level);
  PullsSCTEndcapC->Print(GifFile);
  
  TCanvas *PullsPIXBarrel = new TCanvas("PullsPIXBarrel","PullsPIXBarrel",900,600);
  PullsPIXBarrel->Divide(3,2);
  for(int layer=0;layer<3;layer++){
    sprintf(nameh,"trkana/PIXRes/hPIXPhiPulBaLay%dWide_%d",layer,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXBarrel->cd(layer+1);
    pinta_m();
    sprintf(nameh,"trkana/PIXRes/hPIXEtaPulBaLay%dWide_%d",layer,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXBarrel->cd(layer+4);
    pinta_m();
  }
  sprintf(GifFile,"PullsPIXBarrelSel%d.gif",level);
  PullsPIXBarrel->Print(GifFile);
  
  TCanvas *PullsPIXEndcapA = new TCanvas("PullsPIXEndcapA","PullsPIXEndcapA",900,600);
  PullsPIXEndcapA->Divide(3,2);
  for(int disc=0;disc<3;disc++){
    sprintf(nameh,"trkana/PIXRes/hPIXPhiPulEndcap0Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapA->cd(disc+1);
    pinta_m();
    sprintf(nameh,"trkana/PIXRes/hPIXEtaPulEndcap0Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapA->cd(disc+4);
    pinta_m();
  }
  sprintf(GifFile,"PullsPIXEndcapASel%d.gif",level);
  PullsPIXEndcapA->Print(GifFile);

  TCanvas *PullsPIXEndcapC = new TCanvas("PullsPIXEndcapC","PullsPIXEndcapC",900,600);
  PullsPIXEndcapC->Divide(3,2);
  for(int disc=0;disc<3;disc++){
    sprintf(nameh,"trkana/PIXRes/hPIXPhiPulEndcap1Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapC->cd(disc+1);
    pinta_m();
    sprintf(nameh,"trkana/PIXRes/hPIXEtaPulEndcap1Disc%d_%d",disc,level);
    HD = (TH1F*)f0->Get(nameh); 
    PullsPIXEndcapC->cd(disc+4);
    pinta_m();
  }
  sprintf(GifFile,"PullsPIXEndcapCSel%d.gif",level);
  PullsPIXEndcapC->Print(GifFile);
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
    TPaveText *fitParams = new TPaveText(0.54,0.68,0.98,0.78,"brNDC");
    fitParams->SetFillColor(0);
    fitParams->SetTextAlign(12);
    fitParams->SetTextSize(0.053);
    sprintf(name," - Mean: %2.3f",mean);
    fitParams->AddText(name);
    sprintf(name," - Sigma: %2.3f",sigma);
    fitParams->AddText(name);
    fitParams->Draw();
  }
}

