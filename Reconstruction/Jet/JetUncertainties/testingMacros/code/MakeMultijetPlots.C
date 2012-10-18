/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "TFile.h"
#include "TH1D.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include <iostream>
#include <vector>

using std::cout;

int main(int argc, char **argv) {

  TString JESConfig="JES_2012/JESUncertainty2012_FirstData.config";
  TString mJES="JES_2012/MultijetJES_2012.config";
  if (argc>2) { JESConfig=argv[1]; mJES=argv[1]; }

  TString MCTag = "MC12a";
  
  TString jetAlgo="AntiKt4TopoEM";
  TString pdf=TString("MultijetJES_")+gSystem->BaseName(JESConfig); pdf.ReplaceAll(".config",".pdf");
  TString thePath="../share/";

  TCanvas *c = new TCanvas();
  TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04); tex->SetTextFont(42);
  c->Print(pdf+"[");
  c->SetLogx();

  //log binning
  int N = 100; double min = 9, max = 2500;
  std::vector<double> ptBins; 
  double dx=(log(max)-log(min))/N;
  for (int i=0;i<=N;++i) 
    ptBins.push_back(exp(log(min)+i*dx));

  for (int ei=0;ei<2;++ei) {
    double eta=ei==0?0.5:2.2;
    
  for (int ji=0;ji<4;++ji) {
    int jetR = ji%2==0?4:6;
    TString constScale = ji<2?"EM":"LC";
    jetAlgo=Form("AntiKt%dTopo",jetR);
    jetAlgo+=constScale;
//     MultijetJESUncertaintyProvider j(mJES,"InsituJES2011_AllNuisanceParameters.config",jetAlgo,"FrozenShowers")
    MultijetJESUncertaintyProvider j(mJES,JESConfig,jetAlgo,MCTag, thePath);
    j.useGeV();
    double closeR = jetR == 4? 0.7 : 0.9;
  TH1D *h_jes = new TH1D(Form("%s_%.1fJES",jetAlgo.Data(),eta),"",N,&ptBins[0]);
  TH1D *h_closeby = new TH1D(Form("%s_%.1fcloseby",jetAlgo.Data(),eta),"",N,&ptBins[0]);
  TH1D *h_fComp = new TH1D(Form("%s_%.1ffComb",jetAlgo.Data(),eta),"",N,&ptBins[0]);
  TH1D *h_fResp = new TH1D(Form("%s_%.1ffResp",jetAlgo.Data(),eta),"",N,&ptBins[0]);
  TH1D *h_pileup = new TH1D(Form("%s_%.1ffPileup",jetAlgo.Data(),eta),"",N,&ptBins[0]);
  
  //this is for everything added in quadrature
  for (int bin=1;bin<=h_jes->GetNbinsX();++bin) {
    
    double pt=h_jes->GetBinCenter(bin);
    h_jes->SetBinContent(bin,j.JESUncertaintyProvider::getRelUncert(pt,eta));
    h_closeby->SetBinContent(bin,j.getRelClosebyUncert(pt,closeR));
    //symmetrise the upwards and downwards uncertainties
    h_fComp->SetBinContent(bin,(j.getRelFlavorCompUncert(pt,eta,true)+j.getRelFlavorCompUncert(pt,eta,false))/2);
    h_fResp->SetBinContent(bin,j.getRelFlavorResponseUncert(pt,eta));
    h_pileup->SetBinContent(bin,j.JESUncertaintyProvider::getRelOffsetUncert(pt,eta,7,9));
    //double muncpos = j.getRelPosUncert(ptBins[iPtBin], etaBins[iEtaBin], 0.6);
    //double muncneg = j.getRelNegUncert(ptBins[iPtBin], etaBins[iEtaBin], 0.6);
    //gUnc->SetPoint(counter, ptBins[iPtBin], unc);
    //gMUncPos->SetPoint(counter, ptBins[iPtBin], muncpos);    
    //gMUncNeg->SetPoint(counter, ptBins[iPtBin], muncneg);     
    //counter++;
  }
  h_jes->SetLineColor(kRed+1); h_jes->SetLineWidth(3); h_closeby->SetLineWidth(3);
  h_fComp->SetLineWidth(2);
  h_fResp->SetLineWidth(2);
  h_jes->SetXTitle("jet #font[52]{p}_{T} [GeV]"); h_jes->SetYTitle("Fractional JES uncertainty");
  h_jes->GetXaxis()->SetTitleOffset(1.2); h_jes->GetYaxis()->SetTitleOffset(1.35);
  h_jes->SetMinimum(0); h_jes->SetMaximum(0.10); h_jes->SetStats(0);
  h_jes->Draw("l");
  h_closeby->SetLineColor(kCyan+2); h_closeby->SetLineStyle(2); 
  h_pileup->SetLineColor(kOrange+2); h_pileup->SetLineStyle(3); h_pileup->SetLineWidth(3); 
  h_closeby->Draw("samel"); h_pileup->Draw("sameL");

  h_fComp->SetLineColor(kGreen+2); h_fComp->Draw("sameL");
  h_fResp->SetLineColor(kBlue); h_fResp->Draw("sameL");

  std::vector<TH1D*> hists; hists.push_back(h_jes); hists.push_back(h_closeby); hists.push_back(h_pileup); hists.push_back(h_fComp); hists.push_back(h_fResp);
  TH1D *tot = (TH1D*)hists[0]->Clone(TString(hists[0]->GetName())+"tot");
  for (int xi=1;xi<=tot->GetNbinsX();++xi) {
    double sum_u2=0;
    for (int i=0;i<hists.size();++i)
      sum_u2 += pow(hists[i]->GetBinContent(xi),2);
    tot->SetBinContent(xi,sqrt(sum_u2));
  }
  tot->SetLineColor(kBlack);
  tot->SetLineStyle(2); tot->SetLineWidth(3); tot->Draw("sameL");

  TLegend *leg = new TLegend(0.6,0.56,0.91,0.91);
  leg->SetBorderSize(1);
  leg->AddEntry(h_jes,"Insitu JES","L");
  leg->AddEntry(h_closeby,Form("Close-by jet, #DeltaR=%.1f",closeR),"L");
  leg->AddEntry(h_fComp,"Flavour composition","L");
  leg->AddEntry(h_fResp,"Flavour response","L");
  leg->AddEntry(h_pileup,"Pileup N_{PV}=7, #mu=8","L");
  leg->AddEntry(tot,"Total","L");
  leg->SetFillColor(kWhite); leg->SetTextFont(42);
  leg->Draw();
  
  tex->DrawLatex(0.14,0.85,Form("Anti k_{t} R = 0.%d %s+JES",jetR,constScale.Data()));
  tex->DrawLatex(0.14,0.8,Form("jet #eta = %.1f",eta));
  c->Print(pdf); 
  }
}
  
  c->Print(pdf+"]");
  printf("\n  Produced %s\n\n  All done.\n",pdf.Data());
  return 0;
}
