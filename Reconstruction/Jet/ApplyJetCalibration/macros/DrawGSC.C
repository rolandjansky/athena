/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TROOT.h>
#include <TLorentzVector.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TLine.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <vector>
#include "JES_Plots/AtlasStyle.C"
#include "Root/ApplyJetCalibration.cxx"
#include "Root/GSC.cxx"
#include "Root/NPVBeamspotCorrection.cxx"
#include "Root/KojiOriginCorrection.cxx"

typedef TString Str;
typedef std::vector<TString> StrV;
typedef TGraphErrors Graph;

/*******************/
/***Global option**/
bool _plotresponse=true; //set to true if you want to plot the correction instead of the response!

void error(TString msg) { 
  printf("\n\nERROR:\n\n %s\n\n",msg.Data());
  abort;
}

using namespace JetAnalysisCalib;
JetCalibrationTool *myJES;
GSCTool *myGSC;
Str _GSCFile;
Str _JESFile;

int colors[]  = { kGreen+2, kBlack, kOrange+1, kBlue, kRed+1, kGray+2, kGreen+2, kBlack, kRed+1, kBlue };
int markers[] = { 20, 24, 21, 25, 22, 26, 28, 23, 20, 22, 23 };
double msizes[]  = { 1.0, 1.0, 0.8, 0.8, 1.2, 1.0, 1.2, 1.2, 1.0, 1.2, 1.0 };

double etabins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34};
double pTbins[] = {30,100,500,1000};

double _GeV=1000;
double _trackWIDTHMax, _nTrkMax, _Tile0Max, _EM3Max;

int Nbins;
int Neta;
TCanvas *Can;
TLatex *tex;
Str ps;

void DrawGSC_vs_trackWIDTH(Str jetAlgo, double eta);
void DrawGSC_vs_nTrk(Str jetAlgo, double eta);
void DrawGSC_vs_Tile0(Str jetAlgo, double eta);
void DrawGSC_vs_EM3(Str jetAlgo, double eta);

Graph *GettrackWIDTHGraph(double pT, double eta, double jetPropMax, Str jetAlgo);
Graph *GetnTrkGraph(double pT, double eta, double jetPropMax, Str jetAlgo);
Graph *GetTile0Graph(double pT, double eta, double jetPropMax, Str jetAlgo);
Graph *GetEM3Graph(double pT, double eta, double jetPropMax, Str jetAlgo);

void DrawLabel(TString txt, double x, double y, int mstyle, int col, double msize);
void DrawLabel(TString txt, double x, double y, int style);
void FormatGraph(Graph *g, int style);

Str GetConstScale(Str jetAlgo) { return jetAlgo.Contains("EM")?"EM":"LCW"; }
Str GetCalib(Str jetAlgo) { return GetConstScale(jetAlgo)+"+JES"; }
Str GetAlgo(Str jetAlgo) { return "Anti-k_{t}"; }
Str GetJetR(Str jetAlgo) { 
  for (int r=20;r>0;--r) 
    if (jetAlgo.Contains(Form("%d",r))) 
      return Form("#it{R} = %.1f",0.1*r); 
  error("Cannot extrat R from "+jetAlgo); return "";
}
Str GetEta(double eta) { return Form("#eta = %.1f",eta/10); }
Str GetJetDesc(Str jA) { return GetAlgo(jA)+" "+GetJetR(jA)+", "+GetCalib(jA); }

int main(int argc, char** argv) {

  Str JESFile="GSC_testing_November2012.config";
  Str GSCFile="GSC_factors_Nov2012.root";

  //if (argc>1) JESFile=argv[1];
  //_JESFile=JESFile;

  if (argc>1) GSCFile=argv[1];
  _GSCFile=GSCFile;

  SetAtlasStyle();

  tex=new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04);
  Can = new TCanvas("Can","",800,600);
  Can->Divide(2,2);

  ps="JES_Plots/"+Str(gSystem->BaseName(GSCFile));
  ps.ReplaceAll(".root",".ps");
  Can->Print(ps+"[");

  Neta=sizeof(etabins)/sizeof(double);
  StrV jetAlgos;
  jetAlgos.push_back("AntiKt4TopoEM");
  jetAlgos.push_back("AntiKt6TopoEM");
  jetAlgos.push_back("AntiKt4LCTopo");
  jetAlgos.push_back("AntiKt6LCTopo");

  for(int ialgo=0; ialgo<jetAlgos.size(); ++ialgo) {
    myGSC = new GSCTool(jetAlgos[ialgo],_GSCFile);
    for(int ieta=0; ieta<Neta; ++ieta) {
      _trackWIDTHMax = myGSC->GetjetPropertyMax("trackWIDTH",ieta);
      _nTrkMax = myGSC->GetjetPropertyMax("nTrk",ieta);
      _Tile0Max = myGSC->GetjetPropertyMax("Tile0",ieta);
      _EM3Max = myGSC->GetjetPropertyMax("EM3",ieta);
      DrawGSC_vs_trackWIDTH(jetAlgos[ialgo], ieta);
      DrawGSC_vs_nTrk(jetAlgos[ialgo], ieta);
      DrawGSC_vs_Tile0(jetAlgos[ialgo], ieta);
      DrawGSC_vs_EM3(jetAlgos[ialgo], ieta);
      Can->Print(ps);
    }
  }

  Can->Clear();

  Can->Print(ps+"]");
  // Convert to pdf - kinda stupid, but my Mac
  // prefers making ps then convert to pdf
  // instead of making a pdf directly
  TString pdf=TString(ps).ReplaceAll(".ps",".pdf");
  gSystem->Exec("ps2pdf "+ps+" "+pdf);
  gSystem->Exec("rm -f "+ps);
  printf("\n  Plots saved in:\n    %s\n\n",pdf.Data());

  return 0;
}

void DrawGSC_vs_trackWIDTH(Str jetAlgo, double eta) {

  Nbins=sizeof(pTbins)/sizeof(double);

  TH1F *h = new TH1F("","",100,0,_trackWIDTHMax);
  h->SetXTitle("trackWIDTH");
  if(_plotresponse) h->SetYTitle("Jet response at "+GetConstScale(jetAlgo)+" scale");
  else h->SetYTitle("Jet correction at "+GetConstScale(jetAlgo)+" scale");
  h->SetMaximum(1.12); h->SetMinimum(0.85);

  Can->cd(1);
  h->Draw();

  TLine *l = new TLine();
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);

  for (int ibin=0; ibin<Nbins; ++ibin) {
    Graph *g = GettrackWIDTHGraph(pTbins[ibin], eta, _trackWIDTHMax, jetAlgo);
    FormatGraph(g,ibin);
    g->Draw("PL");
    double x=0.7, y=0.75-0.05*(ibin-3);
    DrawLabel(Form("pT = %.0f GeV",pTbins[ibin]),x,y,ibin);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
  tex->DrawLatex(0.18,0.85,GetEta(eta));

}

void DrawGSC_vs_nTrk(Str jetAlgo, double eta) {

  Nbins=sizeof(pTbins)/sizeof(double);

  TH1F *h = new TH1F("","",100,0,_nTrkMax);
  h->SetXTitle("nTrk");
  if(_plotresponse) h->SetYTitle("Jet response at "+GetConstScale(jetAlgo)+" scale");
  else h->SetYTitle("Jet correction at "+GetConstScale(jetAlgo)+" scale");
  h->SetMaximum(1.12); h->SetMinimum(0.85);

  Can->cd(2);
  h->Draw();

  TLine *l = new TLine();
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);

  for (int ibin=0; ibin<Nbins; ++ibin) {
    Graph *g = GetnTrkGraph(pTbins[ibin], eta, _nTrkMax, jetAlgo);
    FormatGraph(g,ibin);
    g->Draw("PL");
    double x=0.7, y=0.75-0.05*(ibin-3);
    DrawLabel(Form("pT = %.0f GeV",pTbins[ibin]),x,y,ibin);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
  tex->DrawLatex(0.18,0.85,GetEta(eta));

}

void DrawGSC_vs_Tile0(Str jetAlgo, double eta) {

  Nbins=sizeof(pTbins)/sizeof(double);

  TH1F *h = new TH1F("","",100,0,_Tile0Max);
  h->SetXTitle("f_{Tile0}");
  if(_plotresponse) h->SetYTitle("Jet response at "+GetConstScale(jetAlgo)+" scale");
  else h->SetYTitle("Jet correction at "+GetConstScale(jetAlgo)+" scale");
  h->SetMaximum(1.12); h->SetMinimum(0.85);

  Can->cd(3);
  h->Draw();

  TLine *l = new TLine();
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);

  for (int ibin=0; ibin<Nbins; ++ibin) {
    Graph *g = GetTile0Graph(pTbins[ibin], eta, _Tile0Max, jetAlgo);
    FormatGraph(g,ibin);
    g->Draw("PL");
    double x=0.7, y=0.75-0.05*(ibin-3);
    DrawLabel(Form("pT = %.0f GeV",pTbins[ibin]),x,y,ibin);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
  tex->DrawLatex(0.18,0.85,GetEta(eta));

}

void DrawGSC_vs_EM3(Str jetAlgo, double eta) {

  Nbins=sizeof(pTbins)/sizeof(double); 

  TH1F *h = new TH1F("","",100,0,_EM3Max);
  h->SetXTitle("f_{EM3}");
  if(_plotresponse) h->SetYTitle("Jet response at "+GetConstScale(jetAlgo)+" scale");
  else h->SetYTitle("Jet correction at "+GetConstScale(jetAlgo)+" scale");
  h->SetMaximum(1.12); h->SetMinimum(0.85);

  Can->cd(4);
  h->Draw();

  TLine *l = new TLine();
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);

  for (int ibin=0; ibin<Nbins; ++ibin) {
    Graph *g = GetEM3Graph(pTbins[ibin], eta, _EM3Max, jetAlgo);
    FormatGraph(g,ibin);
    g->Draw("PL");
    double x=0.7, y=0.75-0.05*(ibin-3);
    DrawLabel(Form("pT = %.0f GeV",pTbins[ibin]),x,y,ibin);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
  tex->DrawLatex(0.18,0.85,GetEta(eta));

}

Graph *GettrackWIDTHGraph(double pT, double eta, double jetPropMax, Str jetAlgo) {

  Graph *g = new Graph();
  int Np=0;
  for (int i=0;i<100;++i) {
    double jetProp=(jetPropMax/100)*i;
    double GSC=myGSC->GettrackWIDTHResponse(pT, eta, jetProp);
    if(_plotresponse) g->SetPoint(Np++,jetProp,GSC);
    else g->SetPoint(Np++, jetProp, 1./GSC);
  }
  return g;
}

Graph *GetnTrkGraph(double pT, double eta, double jetPropMax, Str jetAlgo) {

  Graph *g = new Graph();
  int Np=0;
  for (int i=0;i<100;++i) {
    double jetProp=(jetPropMax/100)*i;
    double GSC=myGSC->GetnTrkResponse(pT, eta, jetProp);
    if(_plotresponse) g->SetPoint(Np++,jetProp,GSC);
    else g->SetPoint(Np++, jetProp, 1./GSC);
  }
  return g;
}

Graph *GetTile0Graph(double pT, double eta, double jetPropMax, Str jetAlgo) {

  Graph *g = new Graph();
  int Np=0;
  for (int i=0;i<100;++i) {
    double jetProp=(jetPropMax/100)*i;
    double GSC=myGSC->GetTile0Response(pT, eta, jetProp);
    if(_plotresponse) g->SetPoint(Np++,jetProp,GSC);
    else g->SetPoint(Np++, jetProp, 1./GSC);
  }
  return g;
}

Graph *GetEM3Graph(double pT, double eta, double jetPropMax, Str jetAlgo) {

  Graph *g = new Graph();
  int Np=0;
  for (int i=0;i<100;++i) {
    double jetProp=(jetPropMax/100)*i;
    double GSC=myGSC->GetEM3Response(pT, eta, jetProp);
    if(_plotresponse) g->SetPoint(Np++,jetProp,GSC);
    else g->SetPoint(Np++, jetProp, 1./GSC);
  }
  return g;
}

void FormatGraph(Graph *g, int style) {
  g->SetLineColor(colors[style]); g->SetMarkerColor(colors[style]);
  g->SetMarkerStyle(markers[style]); g->SetMarkerSize(msizes[style]);
}

void DrawLabel(TString txt, double x, double y, int mstyle, int col, double msize) {
  TMarker *m = new TMarker(x,y,mstyle);
  m->SetNDC();
  m->SetMarkerSize(msize); m->SetMarkerColor(col);
  TLine *l = new TLine();
  l->SetLineWidth(2); l->SetLineColor(col);
  m->Draw();
  tex->SetTextSize(0.04);
  tex->SetTextAlign(12); tex->SetTextColor(col);
  tex->DrawLatex(x+0.025,y,txt);
  tex->SetTextColor(kBlack);
}

void DrawLabel(TString txt, double x, double y, int style) {
  DrawLabel(txt,x,y,markers[style],colors[style],msizes[style]);
}
