/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Tiny executable to draw the basic JER
 *  and its uncertainty
 */
#include "JetResolution/JERProvider.h"
#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TString.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMath.h>
#include <algorithm>
#include <set>
#include <vector>

typedef TString Str; // Yes! I'm lazy
typedef std::vector<double> VecD;
typedef std::vector<Str> StrV;
typedef TGraphErrors Graph;
using std::vector;

int cols[] = {kRed,kBlue,kOrange+2,kGreen+1,kCyan-2,kGray+2};
int year;

template <class T> vector<T> add(vector<T> v, T a) { v.push_back(a); return v; }
template <class T> vector<T> vec(T a) { std::vector<T> v; v.push_back(a); return v; }
template <class T> vector<T> vec(T a, T b) { return add(vec(a),b); }
template <class T> vector<T> vec(T a, T b, T c) { return add(vec(a,b),c); }
template <class T> vector<T> vec(T a, T b, T c, T d) { return add(vec(a,b,c),d); }
template <class T> vector<T> vec(T a, T b, T c, T d, T e) { return add(vec(a,b,c,d),e); }
template <class T> vector<T> vec(T a, T b, T c, T d, T e, T f) { return add(vec(a,b,c,d,e),f); }
template <class T> vector<T> vec(T a, T b, T c, T d, T e, T f, T g) { return add(vec(a,b,c,d,e,f),g); }

void DrawText(double x, double y, Str txt, int col=kBlack, int align=12) {
  static TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.034); tex->SetTextAlign(align);
  tex->SetTextFont(42); tex->SetTextColor(col); tex->DrawLatex(x,y,txt); }
void DrawTextL(Str txt, double y=0.91, int col=kBlack) { DrawText(0.15,y,txt,col); }
void DrawTextR(Str txt, double y=0.91, int col=kBlack) { DrawText(0.94,y,txt,col,31); }
void DrawJetAlgo(Str jA, double y) {
  DrawTextL(Form("Anti #it{k}_{t} #it{R} = 0.%d %s+JES",jA.Contains("Kt4")?4:6,jA.Contains("LC")?"LCW":"EM"),y);
}

void DrawJER(JERProvider &jer, double eta);
void DrawJERvsEta(JERProvider &jer, double pt);

void DrawSmearingFactors(JERProvider &jer, VecD etaBins, bool AF2=false, bool alt2=false);
void DrawSmearingFactorsVsEta(JERProvider &jer, VecD pts, bool AF2=false, bool alt2=false);

//int main(int argc, char **argv) {
int main() {
  gErrorIgnoreLevel=2000; // turn off "Current canvas added to ..." messages
  Str pdf("JER_plots.pdf"), pdfUnc("JER_plots_summary.pdf");
  TCanvas *can = new TCanvas(); 
  can->SetMargin(0.12,0.04,0.14,0.04);
  can->Print(pdf+"["); can->Print(pdfUnc+"[");
  
  StrV jetAlgos=vec(Str("AntiKt4TopoJES"),Str("AntiKt4LCTopoJES"),Str("AntiKt6TopoJES"),Str("AntiKt6LCTopoJES"));
  VecD etaBins=vec(0.0,0.8,1.2,2.1,2.8,3.6,4.5); 
  VecD pts=vec(30.0,80.0,400.0);

  for (size_t ji=0;ji<jetAlgos.size();++ji) {
    for (year=2011;year<=2012;++year) {
      JERProvider myJER(jetAlgos[ji].Data(),"Truth",
			  Form("$ROOTCOREBIN/data/JetResolution/JERProviderPlots_%d.root",year));
      if (year==2011) myJER.is7TeV();
      myJER.useGeV(true);
      myJER.init();
      can->SetLogx();
      for (size_t ieta=1;ieta<etaBins.size();++ieta) {
        double etaMin=etaBins[ieta-1], etaMax=etaBins[ieta], eta=(etaMin+etaMax)/2;
        DrawJER(myJER,eta); 
        DrawJetAlgo(jetAlgos[ji],0.92); DrawTextL(Form("%.1f #leq #eta < %.1f",etaMin,etaMax),0.87);
        DrawTextR("Data JER = FS truth JER + data-MC offset from #it{in situ}");
        if (year==2012)DrawTextR("JER in 8 TeV data",0.85); 
        if (year==2011)DrawTextR("JER in 7 TeV data",0.85); 
        DrawTextR("MC fullsim JER (truth)",0.8,kRed+1);
        if (year==2012) DrawTextR("MC12 AFII JER (truth)",0.75,kBlue);

        can->Print(pdf);
      }

      can->SetLogx(0);
      for (size_t pti=0;pti<pts.size();++pti) {
        double pt=pts[pti];
        DrawJERvsEta(myJER,pt); DrawJetAlgo(jetAlgos[ji],0.92); 
        DrawTextL(Form("#it{p}_{T} = %.0f GeV",pt),0.87);
        DrawTextR("Data JER = FS truth JER + data-MC offset from #it{in situ}");
        if (year==2012) DrawTextR("JER in 8 TeV data",0.85); 
        if (year==2011) DrawTextR("JER in 7 TeV data",0.85); 
        DrawTextR("MC fullsim JER (truth)",0.8,kRed+1);
        if (year==2012) DrawTextR("MC12 AFII JER (truth)",0.75,kBlue);
        can->Print(pdf);
      }

      Str sqrts=year==2011?"#sqrt{#it{s}} = 7 TeV":"#sqrt{#it{s}} = 8 TeV";
      can->SetLogx(1);
      DrawSmearingFactors(myJER,etaBins);
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);

      /*
      DrawSmearingFactors(myJER,etaBins,0,1);
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);
      */
      if (year==2012) {
        DrawSmearingFactors(myJER,etaBins,1); 
        DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
        can->Print(pdf); can->Print(pdfUnc);
      }
      /*
      DrawSmearingFactors(myJER,etaBins,1,1); 
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);
      */
      can->SetLogx(0);
      DrawSmearingFactorsVsEta(myJER,pts);
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);

      /*
      DrawSmearingFactorsVsEta(myJER,pts,0,1);
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);
      */
      if (year==2012) {
      DrawSmearingFactorsVsEta(myJER,pts,1); 
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);
      }
      /*
      DrawSmearingFactorsVsEta(myJER,pts,1,1); 
      DrawJetAlgo(jetAlgos[ji],0.87); DrawTextL(sqrts,0.82);
      can->Print(pdf); can->Print(pdfUnc);
      */
    }
  }// for each year
  can->Print(pdf+"]");
  can->Print(pdfUnc+"]");
  printf("Produced:\n   %s\n   %s\n",pdf.Data(),pdfUnc.Data());
}

void DrawJER(JERProvider &jer, double eta) {
  double ptMax=2500.0/cosh(eta);
  static TH1F *pt_axis = new TH1F("pt_axis","",1000,10,2500); pt_axis->SetStats(0);
  pt_axis->SetXTitle("jet #it{p}_{T} [GeV]"); pt_axis->SetYTitle("Relative jet energy resolution");
  pt_axis->GetXaxis()->SetMoreLogLabels(); pt_axis->GetYaxis()->SetRangeUser(0,0.4);
  pt_axis->GetXaxis()->SetTitleOffset(1.2); pt_axis->GetYaxis()->SetTitleOffset(1.2);
  pt_axis->GetXaxis()->SetRangeUser(10,ptMax);
  Graph *mc = new Graph(), *data = new Graph(), *mc_up = new Graph();
  Graph *af2 = new Graph(), *af2_up = new Graph();
  for (int i=10;i<2500;++i) {
    int point=mc->GetN(); double pt=i;
    if (pt>ptMax) continue;
    mc->SetPoint(point,pt,jer.getRelResolutionMC(pt,eta));
    //mc->SetPointError(point,0,jer.getInsituUncertMC(pt,eta));
    mc->SetPointError(point,0,0);
    if (year==2012) af2->SetPoint(point,pt,jer.getRelResolutionMC_AFII(pt,eta));
    data->SetPoint(point,pt,jer.getRelResolutionData(pt,eta));
    //data->SetPointError(point,0,jer.getInsituUncertData(pt,eta));
    if (year==2012) data->SetPointError(point,0,jer.getInsituUncert(pt,eta));
    else data->SetPointError(point,0,jer.getInsituDiffDataMCerror(pt,eta));
    mc_up->SetPoint(point,pt,jer.getRelResolutionMC(pt,eta)+jer.getResolutionUncert(pt,eta));
    if (year==2012) af2_up->SetPoint(point,pt,jer.getRelResolutionMC_AFII(pt,eta)+jer.getResolutionUncert_AFII(pt,eta));
  }
  pt_axis->Draw(); 
  data->SetFillStyle(3005); mc->SetFillStyle(3004); mc->SetFillColor(kRed+1);
  data->SetLineWidth(2);
  data->Draw("3"); 
  data->Draw("lx");
  mc->SetLineColor(kRed+1); mc->SetLineWidth(2); mc->Draw("lx");
  if (year==2012){ af2->SetLineColor(kBlue); af2->SetLineWidth(2); af2->Draw("l");} 
  mc_up->SetLineColor(kRed+1); mc_up->SetLineStyle(2); mc_up->Draw("l");
  if (year==2012){ af2_up->SetLineColor(kBlue); af2_up->SetLineStyle(2); af2_up->Draw("l");}
}

void DrawJERvsEta(JERProvider &jer, double pt) { 
  static TH1F *eta_axis = new TH1F("eta_axis","",100,-4.5,4.5); eta_axis->SetStats(0);
  eta_axis->SetXTitle("jet #it{#eta}"); eta_axis->SetYTitle("Relative jet energy resolution");
  eta_axis->GetXaxis()->SetMoreLogLabels(); eta_axis->GetYaxis()->SetRangeUser(0,0.4);
  eta_axis->GetXaxis()->SetTitleOffset(1.2); eta_axis->GetYaxis()->SetTitleOffset(1.2);
  //eta_axis->GetXaxis()->SetRangeUser(10,ptMax);
  Graph *mc = new Graph(), *data = new Graph(), *mc_up = new Graph();
  Graph *af2 = new Graph(), *af2_up = new Graph();
  for (int i=-500;i<500;++i) {
    int point=mc->GetN(); double eta=0.01*i;
    if (pt*cosh(eta)>2500) continue;
    mc->SetPoint(point,eta,jer.getRelResolutionMC(pt,eta));
    if (year==2012) af2->SetPoint(point,eta,jer.getRelResolutionMC_AFII(pt,eta));
    data->SetPoint(point,eta,jer.getRelResolutionData(pt,eta));
    //data->SetPointError(point,0,jer.getInsituUncertData(pt,eta));
    if (year==2012) data->SetPointError(point,0,jer.getInsituUncert(pt,eta));
    else data->SetPointError(point,0,jer.getInsituDiffDataMCerror(pt,eta));
    mc_up->SetPoint(point,eta,jer.getRelResolutionMC(pt,eta)+jer.getResolutionUncert(pt,eta));
    if (year==2012) af2_up->SetPoint(point,eta,jer.getRelResolutionMC_AFII(pt,eta)+jer.getResolutionUncert_AFII(pt,eta));
  }
  eta_axis->Draw();
  data->SetFillStyle(3005);
  data->SetLineWidth(2); data->Draw("3"); data->Draw("lx");
  mc->SetLineColor(kRed+1); mc->SetLineWidth(2); mc->Draw("l"); 
  if (year==2012){ af2->SetLineColor(kBlue); af2->SetLineWidth(2); af2->Draw("l"); }
  mc_up->SetLineColor(kRed+1); mc_up->SetLineStyle(2); mc_up->Draw("l");
  if (year==2012){ af2_up->SetLineColor(kBlue); af2_up->SetLineStyle(2); af2_up->Draw("l");}
} 

void DrawSmearingFactors(JERProvider &jer, VecD etaBins, bool AF2, bool alt2) {
  static TH1F *pt_axis = new TH1F("pt_axis2","",1000,10,2500); pt_axis->SetStats(0);
  pt_axis->SetXTitle("jet #it{p}_{T} [GeV]"); pt_axis->SetYTitle("JER smearing factor");
  pt_axis->GetXaxis()->SetMoreLogLabels(); pt_axis->GetYaxis()->SetRangeUser(0,0.3);
  pt_axis->GetXaxis()->SetTitleOffset(1.2); pt_axis->GetYaxis()->SetTitleOffset(1.2);
  vector<Graph*> sfs; for (size_t i=1;i<etaBins.size();++i) sfs.push_back(new Graph());
  for (int pti=10;pti<2500;++pti) {
    double pt=pti;
    for (size_t i=1;i<etaBins.size();++i) {
      double eta=(etaBins[i-1]+etaBins[i])/2; if (pt*cosh(eta)>2500) continue;
      //this is not how the provider gets the sm. factors
//      double JERmc = AF2 ? 
//	jer.getRelResolutionMC_AFII(pt,eta) :
//	jer.getRelResolutionMC(pt,eta);
//      double JERunc = AF2 ? 
//	jer.getResolutionUncert_AFII(pt,eta,alt2) :
//	jer.getResolutionUncert(pt,eta,alt2);
//      sfs[i-1]->SetPoint(point,pt,sqrt(pow(JERmc+JERunc,2)-JERmc*JERmc));
      double smF = 0;
      if(AF2){
        if(alt2)smF = jer.getSmearingFactorMC_AFII_Alt2(pt,eta);
        else smF = jer.getSmearingFactorMC_AFII(pt,eta);
      }
      else if(alt2)smF = jer.getSmearingFactorMC_Alt2(pt,eta);
      else smF = jer.getSmearingFactorMC(pt,eta);
      int point=sfs[i-1]->GetN(); 
       sfs[i-1]->SetPoint(point,pt,smF);
    }
  }
  pt_axis->Draw();
  for (size_t i=1;i<etaBins.size();++i) {
    sfs[i-1]->SetLineColor(cols[i-1]); sfs[i-1]->SetLineWidth(2); sfs[i-1]->Draw(); 
    DrawTextR(Form("%.1f < |#eta| < %.1f",etaBins[i-1],etaBins[i]),0.95-0.05*i,cols[i-1]);
  }
  if (AF2)
    DrawTextL("AFII JER smearing factors \"effective JER systematic uncertainty\"");
  else
    DrawTextL("Fullsim JER smearing factors \"effective JER systematic uncertainty\"");

  if (alt2==true)
    DrawText(0.4,0.82,"#bf{Alternative 2}");
}

void DrawSmearingFactorsVsEta(JERProvider &jer, VecD pts, bool AF2, bool alt2) {
  static TH1F *eta_axis = new TH1F("eta_axis2","",100,-4.5,4.5); eta_axis->SetStats(0);
  eta_axis->SetXTitle("jet #eta"); eta_axis->SetYTitle("JER smearing factor");
  eta_axis->GetXaxis()->SetMoreLogLabels(); eta_axis->GetYaxis()->SetRangeUser(0,0.3);
  eta_axis->GetXaxis()->SetTitleOffset(1.2); eta_axis->GetYaxis()->SetTitleOffset(1.2);
  vector<Graph*> sfs; for (size_t i=0;i<pts.size();++i) sfs.push_back(new Graph());
  for (int etai=-50;etai<50;++etai) {
    double eta=0.1*etai;
    for (size_t i=0;i<pts.size();++i) {
      double pt=pts[i]; if (pt*cosh(eta)>2500) continue;
//      double JERmc = AF2 ? 
//	jer.getRelResolutionMC_AFII(pt,eta) :
//	jer.getRelResolutionMC(pt,eta);
//      double JERunc = AF2 ? 
//	jer.getResolutionUncert_AFII(pt,eta,alt2) :
//	jer.getResolutionUncert(pt,eta,alt2);
//      sfs[i]->SetPoint(point,eta,sqrt(pow(JERmc+JERunc,2)-JERmc*JERmc));
      double smF = 0;
      if(AF2){
        if(alt2)smF = jer.getSmearingFactorMC_AFII_Alt2(pt,eta);
        else smF = jer.getSmearingFactorMC_AFII(pt,eta);
      }
      else if(alt2)smF = jer.getSmearingFactorMC_Alt2(pt,eta);
      else smF = jer.getSmearingFactorMC(pt,eta);
      int point=sfs[i]->GetN(); 
      sfs[i]->SetPoint(point,eta,smF);
    }
  }
  eta_axis->Draw();
  for (size_t i=0;i<pts.size();++i) {
    sfs[i]->SetLineColor(cols[i]); sfs[i]->SetLineWidth(2); sfs[i]->Draw(); 
    DrawTextR(Form("#it{p}_{T} = %.0f GeV",pts[i]),0.9-0.05*i,cols[i]);
  }
  if (AF2)
    DrawTextL("AFII JER smearing factors \"effective JER systematic uncertainty\"");
  else
    DrawTextL("Fullsim JER smearing factors \"effective JER systematic uncertainty\"");

  if (alt2==true)
    DrawText(0.4,0.82,"#bf{Alternative 2}");
}

