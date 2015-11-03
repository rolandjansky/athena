#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>

#include "TCanvas.h"
#include "TProfile.h"
#include "TH1.h"
#include "TFile.h"
#include "TStyle.h"
#include "TLegend.h"
#include "THStack.h"
#include "TPaveText.h"

using namespace std;


void plotRadLen()
{
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptStat(0);

  THStack *hs = new THStack("hs", "Inner Detector Material");

  TFile *hfile = new TFile("xx0.root"); 

  TH1F * hbp = (TH1F *) BeamPipe->ProjectionX(); hbp->SetName("hbp");
  TH1F * hpixel = (TH1F *) Pixel_Total->ProjectionX(); hpixel->SetName("hpixel");
  TH1F * hsct = (TH1F *) SCT_Total->ProjectionX(); hsct->SetName("hsct");
  TH1F * hother = (TH1F *) Other->ProjectionX(); hother->SetName("hother");
  TH1F * hserv = (TH1F *) InDetServMat_Total->ProjectionX(); hserv->SetName("hserv");
  TH1F * htot = (TH1F *) Total->ProjectionX(); htot->SetName("htot");

  double minVal = -5.;
  double maxVal = 5.;
  double hMaxDraw = 3.2;

  hbp->SetFillColor(kYellow);
  hbp->GetXaxis()->SetRangeUser(minVal,maxVal);
  hs->Add(hbp);

  hpixel->SetFillColor(kMagenta);
  hpixel->GetXaxis()->SetRangeUser(minVal,maxVal);
  hs->Add(hpixel);

  hsct->SetFillColor(kGreen);
  hsct->GetXaxis()->SetRangeUser(minVal,maxVal);
  hs->Add(hsct);

  hother->SetFillColor(kCyan);
  hother->GetXaxis()->SetRangeUser(minVal,maxVal);
  hs->Add(hother);

  hserv->SetFillColor(kRed);
  hserv->GetXaxis()->SetRangeUser(minVal,maxVal);
  hs->Add(hserv);

  hs->SetMinimum(0);
  hs->SetMaximum(hMaxDraw);

  hs->Draw("hist");


  double aver1 = htot->GetIntegral();
  double aver2 = htot->GetEntries();
  double aver3 =aver1/aver2;
  double ntotbins = htot->GetNbinsX();

  double bsum=0.;
  for(int i_bins=0; i_bins < htot->GetNbinsX();i_bins++){
  double bin_1 = htot->GetBinContent(i_bins);
  bsum = bsum + bin_1;
                    }
  aver3 = bsum/ntotbins;
  std::cout << ntotbins << "  " << bsum << " " << aver3 << std::endl;
  htot->SetLineStyle(2);
  htot->Draw("hist same");
  hs->GetXaxis()->SetRangeUser(minVal,maxVal);
  hs->GetXaxis()->SetTitle("#eta");
//  hs->GetXaxis()->SetTitle("R, mm");
//  hs->GetXaxis()->SetTitle("#phi, rad");
//  hs->GetXaxis()->SetTitle("z_0, mm");
  hs->GetYaxis()->SetTitle("Radiation length [X/X_{0}]");


//  TLegend *leg = new TLegend(0.45,0.68,0.63,0.93);
  TLegend *leg = new TLegend(0.55,0.58,0.93,0.93);
  leg->SetBorderSize(0);
  leg->SetLineWidth(0);
  leg->AddEntry(hserv,"Services","f"); 
  leg->AddEntry(hother,"Other","f");
  leg->AddEntry(hsct,"SCT","f");
  leg->AddEntry(hpixel,"Pixel","f");
  leg->AddEntry(hbp,"Beam-pipe","f");
  leg->AddEntry(htot,"Total","l");
  leg->SetFillColor(0);
  leg->Draw();

   TText t;
   t.SetTextSize(0.04);
   t.DrawText(-4.5,3.,"ATLAS-SLHC-01-03-00");

  c1->Print("c1.eps");


}


