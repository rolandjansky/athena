/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#include <iostream.h>
#include <iomanip.h>
//
#include <TH2F.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TF1.h>
#include <TF2.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TStyle.h>

void ResVsPt(char nname[80], int level=4)
{
  Set_style();
  TFile *f0 = new TFile(nname);
  TProfile *HD;
  TH2F *HDxy;
  char nameh[80];
  char pngfile[20];
  TCanvas *CanResVsPt = new TCanvas("ResVsPt","Res vs Reco.Pt",0,0,1000,800);
  CanResVsPt->Divide(2,2);

  for(int layer=0;layer<4;layer++){  
    CanResVsPt->cd(layer+1);
    sprintf(nameh,"/trkana/Track_Parameters/hResRecoPt_SCTBaLay%d_%d",layer, level);
    HDxy = (TH2F*)f0->Get(nameh); 
    HDxy->GetYaxis()->SetAxisColor(2);
    HDxy->GetXaxis()->SetAxisColor(2);
    HDxy->GetYaxis()->SetLabelSize(0.03);
    HDxy->GetYaxis()->SetTitleSize(0.03);
    HDxy->GetYaxis()->SetTitleOffset(1.5);
    HDxy->GetXaxis()->SetLabelSize(0.03);
    HDxy->GetZaxis()->SetLabelSize(0.02);
    HDxy->Draw("colz");
    HD = HDxy->ProfileX();
    HD->SetLineColor(1);
    HD->SetMarkerColor(1);
    HD->SetMarkerStyle(7);
    HD->Draw("sameE1");
    
  }
  sprintf(pngfile,"CanResVsPt_%d.gif",level);
  CanResVsPt->Print(pngfile);
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

