/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TCanvas.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <TMarker.h>
#include <TLine.h>
#include <TROOT.h>
#include <TH1F.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <vector>
#include "JES_Plots/AtlasStyle.C"
#include "Root/ApplyJetCalibration.cxx"
#include "Root/GSC.cxx"
#include "Root/NPVBeamspotCorrection.cxx"

/*
 *   This macro draws the the varios jet calibration correction factors versus jet energy, eta, etc.
 *
 *   Dag Gillberg
 */

typedef TString Str;
typedef std::vector<TString> StrV;
typedef TGraphErrors Graph;

bool isApproved = true; // whether to draw ATLAS Preliminary
void error(TString msg) {
  printf("\n\nERROR:\n\n  %s\n\n",msg.Data()); abort();
}

using namespace JetAnalysisCalib;
JetCalibrationTool *myJES;

int colors[]  = { kGreen+2, kBlack, kOrange+1, kBlue, kRed+1, kGray+2, kGreen+2, kBlack, kRed+1, kBlue };
int markers[] = { 20, 24, 21, 25, 22, 26, 28, 23, 20, 22, 23 };
double msizes[]  = { 1.0, 1.0, 0.8, 0.8, 1.2, 1.0, 1.2, 1.2, 1.0, 1.2, 1.0 };

TString _jesFile;
double Ebins[] = {30, 60, 110, 400, 2000 };
int Nbins;
TCanvas *Can;
TLatex *tex;
Str ps;

/// Subrutines
void DrawJES_vs_Eta(Str jetAlgo);
void DrawJESAsymm_vs_Eta(Str jetAlgo);
void DrawJMS_vs_Eta(Str jetAlgo);
void DrawInsituCorr_vs_Eta(Str jetAlgo);
void DrawInsituCorrAsymm_vs_Eta(Str jetAlgo);
void DrawInsituCorr_vs_Pt(Str jetAlgo);
bool DrawPileupCorr(Str jetAlgo);
void DrawPileupCorrVsMuNPV(Str jetAlgo);

double GetJES(double E, double eta);
Graph *GetJESAsymmGraph(double E);
double GetJMS(double E, double eta);
Graph *GetJESGraph(double E);
Graph *GetJMSGraph(double E);
Graph *GetInsituGraph(double pT);
Graph *GetInsituAsymmGraph(double pT);
Graph *GetInsituGraphVsPt(double eta);

void DrawLabel(TString txt, double x, double y, int mstyle, int col, double msize);
void DrawLabel(TString txt, double x, double y, int style);
void FormatGraph(Graph *g, int style);

void add(StrV &v, Str a) { v.push_back(a); }
void add(StrV &v, Str a, Str b) { add(v,a); add(v,b); }
void add(StrV &v, Str a, Str b, Str c) { add(v,a); add(v,b); add(v,c); }
void add(StrV &v, Str a, Str b, Str c, Str d) { add(v,a,b); add(v,c,d); }
void add(StrV &v, Str a, Str b, Str c, Str d, Str e, Str f) { add(v,a,b); add(v,c,d); add(v,e,f); }
void add(StrV &v, Str a, Str b, Str c, Str d, Str e, Str f, Str g, Str h, Str i, Str j) { add(v,a,b); add(v,c,d); add(v,e,f); add(v,g,h); add(v,i,j); }

Str GetConstScale(Str jetAlgo) { return jetAlgo.Contains("EM")?"EM":"LCW"; }
Str GetCalib(Str jetAlgo) { return GetConstScale(jetAlgo)+"+JES"; }
Str GetAlgo(Str jetAlgo) { return "Anti-k_{t}"; }
Str GetJetR(Str jetAlgo) { 
  for (int r=20;r>0;--r) 
    if (jetAlgo.Contains(Form("%d",r))) 
      return Form("#it{R} = %.1f",0.1*r); 
  error("Cannot extrat R from "+jetAlgo); return "";
}
Str GetJetDesc(Str jA) { return GetAlgo(jA)+" "+GetJetR(jA)+", "+GetCalib(jA); }

/******************************************
 *   Main code starts here
 */
int main(int argc, char** argv) {

  bool drawCalibCurves=true;
  TString JESfile="Rel17_JES_OffsetSubtracted.config";
  gErrorIgnoreLevel=2000;

  if (argc>1) JESfile=argv[1];
  _jesFile=JESfile;

  SetAtlasStyle();

  tex=new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04);
  Can = new TCanvas("Can","",800,600);

  ps="JES_Plots/"+Str(gSystem->BaseName(JESfile));
  ps.ReplaceAll(".config",".ps");
  Can->Print(ps+"[");

  StrV jetAlgos;
  if(_jesFile.Contains("GSC")) { std::cout << "Ignoring GSC config file." << std::endl; abort();
  } else if (_jesFile.Contains("FatJet_JES_2012")) {
    add(jetAlgos,"AntiKt10LCTopoTrimmedPtFrac5SmallR30");
  } else if (_jesFile.Contains("FatJet")) {
      add(jetAlgos,
	  "AntiKt10LCTopo",
	  "AntiKt10LCTopoTrimmedPtFrac3SmallR30",
	  "AntiKt10LCTopoTrimmedPtFrac5SmallR30",
	  "CamKt12LCTopo",
	  "CamKt12LCTopoTrimmedPtFrac5SmallR30",
	  "CamKt12LCTopoSplitFilteredmassFraction67minSplitR30");
  } else if (_jesFile.Contains("Full2012dataset_Preliminary_AFII")) {
    add(jetAlgos,"AntiKt4TopoEM","AntiKt4LCTopo","AntiKt6TopoEM");
  } else if (_jesFile.Contains("NotForPhysics_May8")) {
    add(jetAlgos,"AntiKt2LCTopo","AntiKt3LCTopo","AntiKt4TopoEM","AntiKt4LCTopo","AntiKt5LCTopo","AntiKt6TopoEM","AntiKt6LCTopo","AntiKt7LCTopo","AntiKt8LCTopo","AntiKt10LCTopo");
  } else if (_jesFile.Contains("2012_AFII")) {
    add(jetAlgos,"AntiKt4TopoEM","AntiKt4LCTopo");
  } else if (_jesFile.Contains("2012_FirstCalibration_AFII")) {
    add(jetAlgos,"AntiKt4TopoEM","AntiKt4LCTopo","AntiKt6TopoEM");
  } else if (_jesFile.Contains("Rel17.2_JES_muScan")) {
    add(jetAlgos,"AntiKt4LCTopo");
    } else {
    add(jetAlgos,"AntiKt4TopoEM","AntiKt4LCTopo","AntiKt6TopoEM","AntiKt6LCTopo");
  }
  
  // 1. Draw JES
  for (int ji=0;ji<jetAlgos.size();++ji) {
    DrawJES_vs_Eta(jetAlgos[ji]); Can->Print(ps);
  }

  // 1.5 Draw asymmetry in the JES
  //for (int ji=0;ji<jetAlgos.size();++ji) {
  //  DrawJESAsymm_vs_Eta(jetAlgos[ji]); Can->Print(ps);
  //}

  // 2. Draw JMS
  if (_jesFile.Contains("FatJet"))
    for (int ji=0;ji<jetAlgos.size();++ji) {
      DrawJMS_vs_Eta(jetAlgos[ji]); Can->Print(ps);
    }
  
  // 3. Draw insitu correction
  for (int ji=0;ji<jetAlgos.size();++ji) {
    DrawInsituCorr_vs_Eta(jetAlgos[ji]); Can->Print(ps);
  }

  // 3.5 Draw aymmetry in the insitu correction
  //for (int ji=0;ji<jetAlgos.size();++ji) {
  //  DrawInsituCorrAsymm_vs_Eta(jetAlgos[ji]); Can->Print(ps);
  //}


  Can->Clear();
  Can->SetLogx();
  // 4. Draw insitu correction versus pT
  for (int ji=0;ji<jetAlgos.size();++ji) {
    DrawInsituCorr_vs_Pt(jetAlgos[ji]);
    Can->Print(ps);
  }
  Can->SetLogx(0);
  
  JetCalibrationTool *theJES = new JetCalibrationTool(jetAlgos[0],_jesFile,false);
  TH1F *axis = new TH1F("","",51,-0.5,50.5);
  axis->SetXTitle("#it{N}_{PV}^{raw}"); axis->SetYTitle("#it{N}_{PV}^{corr}"); 
  axis->SetMaximum(50); axis->Draw();
  Graph *g = new Graph(), *grat = new Graph();
  for (int i=0;i<51;++i) {
    g->SetPoint(i,i,theJES->GetNPVBeamspotCorrection(i));
    if (i)
      grat->SetPoint(i-1,i,theJES->GetNPVBeamspotCorrection(i)/i);
    printf("NPV = %2d : corr: %.2f, ratio: %.3f\n",
	   i,theJES->GetNPVBeamspotCorrection(i),theJES->GetNPVBeamspotCorrection(i)/i);
  }
  g->SetMarkerStyle(22); g->SetMarkerColor(kBlue); g->Draw("PL");
  Can->Print(ps);
  axis->GetYaxis()->SetRangeUser(0.8,1.1); axis->SetYTitle("#it{N}_{PV}^{corr} / #it{N}_{PV}^{raw}"); 
  axis->Draw();
  grat->SetMarkerStyle(22); grat->SetMarkerColor(kBlue); grat->Draw("PL");
  Can->Print(ps);

  // 5. Draw pile-up offset correction
  for (int j=0;j<jetAlgos.size();++j)
    if (DrawPileupCorr(jetAlgos[j])) Can->Print(ps);

  for (int j=0;j<jetAlgos.size();++j)
    DrawPileupCorrVsMuNPV(jetAlgos[j]);

  /*
  for (int j=0;j<jetAlgos.size();++j)
    if (DrawPileupCorr(jetAlgos[j],true)) Can->Print(ps);

  for (int j=0;j<jetAlgos.size();++j)
    DrawPileupCorrVsMuNPV(jetAlgos[j],false);

  */

  if (drawCalibCurves) {
    Can->SetLogx();
    TH1F *axis = new TH1F("","",1993,7,2000);
    axis->SetXTitle("Pile-up subtracted scale #it{E} [GeV]"); 
    axis->GetXaxis()->SetMoreLogLabels();
    std::vector<JetCalibrationTool*> tools;
    std::vector<Graph*> jes_vec;
    for (int j=0;j<jetAlgos.size();++j) {
      tools.push_back(new JetCalibrationTool(jetAlgos[j],_jesFile));
      tools[j]->UseGeV();
      jes_vec.push_back(new Graph()); jes_vec[j]->SetLineColor(colors[j]);
      jes_vec[j]->SetLineWidth(2); jes_vec[j]->SetLineStyle(j);
    }
    for (int ai=0;ai<45;++ai) {
      for (int si=0;si<2;++si) {
	double etaMin=0.1*ai, etaMax=etaMin+0.1, eta=etaMin+0.05;
	if (si==0) { etaMin=-etaMax; etaMax=etaMin+0.1; eta*=-1; }
	axis->SetYTitle("JES calibration factor"); 
	axis->GetYaxis()->SetRangeUser(1,3.2);
	axis->GetXaxis()->SetRangeUser(7*cosh(eta),2000);
	axis->Draw();
	for (int j=0;j<jetAlgos.size();++j) {
	  jes_vec[j]->Clear(); double ptmin=7, ptmax=2000/cosh(eta), dpt=(log(ptmax)-log(ptmin))/100;
	  for (int p=0;p<=100;++p) {
	    double E=exp(log(7)+dpt*p)*cosh(eta);
	    jes_vec[j]->SetPoint(p,E,tools[j]->GetJES(E,eta));
	  }
	  jes_vec[j]->Draw("L");
	  tex->SetTextColor(colors[j]);
	  tex->DrawLatex(0.6,0.85-0.05*j,GetJetDesc(jetAlgos[j]));
	}
	tex->SetTextColor(kBlack);
	tex->DrawLatex(0.6,0.9,Form("%.1f < #it{#eta}_{det} < %.1f",etaMin,etaMax));
	tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
	TLine *l = new TLine(); l->SetLineStyle(4); l->SetLineWidth(3);
	//l->SetLineWidth(2); l->SetLineColor(kGray+2); 
	l->DrawLine(cosh(eta)*10,1,cosh(eta)*10,2.9);
	l->DrawLine(cosh(eta)*20,1,cosh(eta)*20,2.9);
	tex->SetTextAlign(22); tex->SetNDC(0);
	tex->DrawLatex(cosh(eta)*10,3.0,"#it{p}_{T} = 10");
	tex->DrawLatex(cosh(eta)*20,3.0,"#it{p}_{T} = 20");
	tex->SetTextAlign(11); tex->SetNDC();
	Can->Print(ps);

	// Below: check of the calibration curves
	// 
	continue;

	axis->SetYTitle("d#it{E}^{corr} / d#it{E}^{const}"); 
	axis->GetYaxis()->SetRangeUser(-0.1,2.0);
	axis->Draw();

	for (int j=0;j<jetAlgos.size();++j) {
	  jes_vec[j]->Clear(); double ptmin=7, ptmax=2000/cosh(eta), dpt=(log(ptmax)-log(ptmin))/100;
	  for (int p=0;p<=100;++p) {
	    double E1=exp(log(7)+dpt*p)*cosh(eta), E2=exp(log(7)+dpt*(p+1))*cosh(eta);
	    jes_vec[j]->SetPoint(p,E1,(tools[j]->GetJES(E2,eta)*E2 - tools[j]->GetJES(E1,eta)*E1)/(E2-E1));
	  }
	  jes_vec[j]->Draw("L");
	  tex->SetTextColor(colors[j]);
	  tex->DrawLatex(0.6,0.85-0.05*j,GetJetDesc(jetAlgos[j]));
	}

	tex->SetTextColor(kBlack);
	tex->DrawLatex(0.6,0.9,Form("%.1f < #it{#eta}_{det} < %.1f",etaMin,etaMax));
	tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());

	l->DrawLine(cosh(eta)*10,0,cosh(eta)*10,1);
	l->DrawLine(cosh(eta)*15,0,cosh(eta)*15,1);
	l->DrawLine(cosh(eta)*20,0,cosh(eta)*20,1);
	tex->SetTextAlign(22); tex->SetNDC(0);
	tex->DrawLatex(cosh(eta)*10,1.1,"#it{p}_{T} = 10");
	tex->DrawLatex(cosh(eta)*20,1.1,"#it{p}_{T} = 20");
	tex->SetTextAlign(11); tex->SetNDC();

	Can->Print(ps);
      }
    } // for each abs eta bin
  }
  
  Can->Print(ps+"]");
  // Convert to pdf - kinda stupid, but my Mac
  // prefers making ps then convert to pdf
  // instead of making a pdf directly
  TString pdf=TString(ps).ReplaceAll(".ps",".pdf");
  gSystem->Exec("ps2pdf "+ps+" "+pdf);
  gSystem->Exec("rm -f "+ps);
  printf("\n  Plots saved in:\n    %s\n\n",pdf.Data());
}

/*
 *   Main code ends here
 ******************************************
 */




/*
 *  Subrutines
 */

void DrawJMS_vs_Eta(Str jetAlgo) {
  double massEbins[] = {50, 100, 250, 500, 750, 1500};
  double massNbins = sizeof(massEbins)/sizeof(double);
  myJES = new JetCalibrationTool(jetAlgo,_jesFile);
  TH1F *h = new TH1F("","",100,-4.8,4.8);
  h->SetXTitle("Jet #eta_{det}"); h->SetYTitle("Jet mass response");
  h->SetMaximum(1.4); h->SetMinimum(0.6);
  h->Draw();
  
  TLine *l = new TLine(); 
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);
  l->DrawLine(-4.65,1,4.65,1);

  for (int i=0;i<massNbins;++i) {
    Graph *g = GetJMSGraph(massEbins[i]);
    FormatGraph(g,i);
    g->Draw("PL");
    double x=0.52, y=0.3-0.05*i;
    if (i>2) { x=0.74; y=0.3-0.05*(i-3); }
    DrawLabel(Form("E = %.0f GeV",massEbins[i]),x,y,i);
  }
  
  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
  tex->DrawLatex(0.18,0.9,jetAlgo);
}

void DrawJES_vs_Eta(Str jetAlgo) {
  Nbins=sizeof(Ebins)/sizeof(double);
  //TString alg[] = {"TopoEM","LCTopo"};
  //TString calib[] = {"EM","LC"};
  //TString jAlgo=Form("AntiKt%d%s",jetR,alg[jetalg].Data());
  myJES = new JetCalibrationTool(jetAlgo,_jesFile);
  
  TH1F *h = new TH1F("","",100,-4.8,4.8);
  h->SetXTitle("Jet #eta_{det}");
  h->SetYTitle("Jet response at "+GetConstScale(jetAlgo)+" scale");
  if (jetAlgo.Contains("EM")) { h->SetMaximum(1.0); h->SetMinimum(0.3); }
  else { h->SetMaximum(1.1); h->SetMinimum(0.6); }

  if (_jesFile.Contains("FatJet")) {
    h->SetMaximum(1.4); h->SetMinimum(0.6);
  }

  h->Draw();
  TLine *l = new TLine(); 
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);
  if (jetAlgo.Contains("LC")) l->DrawLine(-4.65,1,4.65,1);
  
  for (int i=0;i<Nbins;++i) {
    Graph *g = GetJESGraph(Ebins[i]);
    FormatGraph(g,i);
    g->Draw("PL");
    double x=0.52, y=0.3-0.05*i;
    if (i>2) { x=0.74; y=0.3-0.05*(i-3); }
    DrawLabel(Form("E = %.0f GeV",Ebins[i]),x,y,i);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);
  
  /*
  double ymin=0.5+Dy, ymax=0.9+Dy;
  l->DrawLine(0.3,ymin,0.3,ymax);
  l->DrawLine(0.8,ymin,0.8,ymax);
  l->DrawLine(1.2,ymin,1.2,ymax);
  l->DrawLine(1.2,ymin,1.2,ymax);
  l->DrawLine(2.1,ymin,2.1,ymax);
  l->DrawLine(2.8,ymin,2.8,ymax);
  l->DrawLine(3.6,ymin,3.6,ymax);  
  l->DrawLine(4.5,ymin,4.5,ymax);
    
  tex->SetTextAlign(22);
  tex->SetNDC(0);
  tex->DrawLatex(4.05,0.94+Dy,"FCal");
  tex->DrawLatex(3.2,0.952+Dy,"HEC-FCal");
  tex->DrawLatex(3.2,0.92+Dy,"Transition");
  tex->DrawLatex(2.45,0.94+Dy,"HEC");
  tex->DrawLatex(1.45,0.952+Dy,"Barrel-Endcap"); 
  tex->DrawLatex(1.45,0.92+Dy,"Transition");
  tex->DrawLatex(0.4,0.94+Dy,"Barrel");
  */
  
  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
  //Form("Anti-k_{t} #font[52]{R} = 0.%d, %s+JES",jetR,calib[jetalg].Data()));

  //ATLASLabel(0.192,0.886,0,kBlack);
}

void DrawJESAsymm_vs_Eta(Str jetAlgo) {
  Nbins=sizeof(Ebins)/sizeof(double);
  myJES = new JetCalibrationTool(jetAlgo,_jesFile);
  
  TH1F *h = new TH1F("","",100,0,4.8);
  h->SetXTitle("Jet |#eta_{det}|");
  h->SetYTitle("Asymmetry in the jet response at "+GetConstScale(jetAlgo)+" scale");
  h->SetMaximum(0.1); h->SetMinimum(-0.1);

  h->Draw();
  TLine *l = new TLine(); 
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);
  if (jetAlgo.Contains("LC")) l->DrawLine(0,0,4.65,0);
  
  for (int i=0;i<Nbins;++i) {
    Graph *g = GetJESAsymmGraph(Ebins[i]);
    FormatGraph(g,i);
    g->Draw("PL");
    double x=0.52, y=0.3-0.05*i;
    if (i>2) { x=0.74; y=0.3-0.05*(i-3); }
    DrawLabel(Form("E = %.0f GeV",Ebins[i]),x,y,i);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);
  
  tex->SetNDC(); tex->SetTextAlign(12);
  tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
}

void DrawInsituCorr_vs_Pt(Str jetAlgo) {
  myJES = new JetCalibrationTool(jetAlgo,_jesFile);
  TH1F *h = new TH1F("","",100,15,2500);
  h->SetXTitle("Jet p_{T}");
  h->SetYTitle("Residual correction for data only");
  h->SetMaximum(1.1); h->SetMinimum(0.85);
  h->Draw();
  TLine *l = new TLine(); 
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);
  l->DrawLine(16,1,2400,1);
  for (int i=0;i<6;++i) {
    double eta=4.0*i/5;
    Graph *g = GetInsituGraphVsPt(eta);
    FormatGraph(g,i); g->Draw("PL");
    double x=0.52, y=0.3-0.05*i;
    if (i>2) { x=0.74; y=0.3-0.05*(i-3); }
    DrawLabel(Form("#eta = %.1f",eta),x,y,i);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  //tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
}

void DrawInsituCorr_vs_Eta(Str jetAlgo) {
  Nbins=sizeof(Ebins)/sizeof(double);
  myJES = new JetCalibrationTool(jetAlgo,_jesFile);

  TH1F *h = new TH1F("","",100,-4.8,4.8);
  h->SetXTitle("Jet #eta_{det}");
  h->SetYTitle("Residual correction for data only");
  h->SetMaximum(1.1); h->SetMinimum(0.85);

  h->Draw();
  TLine *l = new TLine(); 
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);
  l->DrawLine(-4.65,1,4.65,1);
  
  for (int i=0;i<Nbins;++i) {
    Graph *g = GetInsituGraph(Ebins[i]);
    FormatGraph(g,i); g->Draw("PL");
    double x=0.52, y=0.3-0.05*i;
    if (i>2) { x=0.74; y=0.3-0.05*(i-3); }
    DrawLabel(Form("p_{T} = %.0f GeV",Ebins[i]),x,y,i);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  //tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
}

void DrawInsituCorrAsymm_vs_Eta(Str jetAlgo) {
  Nbins=sizeof(Ebins)/sizeof(double);
  myJES = new JetCalibrationTool(jetAlgo,_jesFile);

  TH1F *h = new TH1F("","",50,0,4.8);
  h->SetXTitle("Jet |#eta_{det}|");
  h->SetYTitle("Asymmetry in residual data correction");
  h->SetMaximum(0.03); h->SetMinimum(-0.03);

  h->Draw();
  TLine *l = new TLine(); 
  l->SetLineWidth(2); l->SetLineColor(kGray+2); l->SetLineStyle(4);
  l->DrawLine(0,0,4.65,0);
  
  for (int i=0;i<Nbins;++i) {
    Graph *g = GetInsituAsymmGraph(Ebins[i]);
    FormatGraph(g,i); g->Draw("PL");
    double x=0.52, y=0.3-0.05*i;
    if (i>2) { x=0.74; y=0.3-0.05*(i-3); }
    DrawLabel(Form("p_{T} = %.0f GeV",Ebins[i]),x,y,i);
  }

  l->SetLineWidth(2);
  l->SetLineColor(kGray+2);
  l->SetLineStyle(1);

  tex->SetNDC(); tex->SetTextAlign(12);
  //tex->DrawLatex(0.18,0.975,myJES->AbsoluteJES_Description());
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
}


/*
 *   Accessing the JES graphs
 */

// Newton's method to get the JES for a
// fixed "truth"-jet energy
// (i.e. JES(Ecalib) instead of usual JES(Euncorr)
double GetJES(double E, double eta){
  double JES=myJES->GetJES(E*1000,eta);
  //  return JES;
  JES=myJES->GetJES(E*1000/JES,eta);
  JES=myJES->GetJES(E*1000/JES,eta);
  JES=myJES->GetJES(E*1000/JES,eta);
  return JES;
}

// takes the calibrated jet energy as input
double GetJMS(double E, double eta){
  double JMS=myJES->GetMassCorr(E*1000,eta);
  return JMS;
}

Graph *GetJESGraph(double E) {
  Graph *g = new Graph();
  int Np=0;
  for (int i=-45;i<45;++i) {
    double eta=0.05+0.1*i;
    double JES=GetJES(E,eta);
    //JES+=GetJES(E,-eta); JES/=2;
    double pT=E/cosh(eta);
    if (pT/JES<10) continue;
    if ( _jesFile.Contains("FatJet") && pT/JES<40 ) continue;
    //if (E==30)
    // printf("eta = %.2f, JES: %.2f, E = %.2f, Eem: %.2f\n",eta,JES,E,E/JES);
    //abort();
    g->SetPoint(Np++,eta,1.0/JES);
  }
  return g;
}

Graph *GetJESAsymmGraph(double E) {
  Graph *g = new Graph();
  int Np=0;
  for (int i=0;i<45;++i) {
    double eta=0.05+0.1*i;
    double Rplus=1./GetJES(E,eta);
    double Rminus=1./GetJES(E,-eta);
    double pT=E/cosh(eta);
    if (pT*Rplus<10) continue;
    if ( _jesFile.Contains("FatJet") && pT*Rplus<40 ) continue;
    g->SetPoint(Np++,eta,Rplus-Rminus);  
  }
  return g;
}

Graph *GetJMSGraph(double E) {
  Graph *g = new Graph();
  int Np=0;
  for (int i=-40;i<40;++i) {
    double eta=0.05+0.1*i;
    double JMS=GetJMS(E,eta);
    double pT=E/cosh(eta);
    if (pT<40) continue;
    g->SetPoint(Np++,eta,1.0/JMS);
  }
  return g;
}

Graph *GetInsituGraph(double pT) {
  Graph *g = new Graph(); int Np=0;
  for (int i=-45;i<45;++i) {
    double eta=0.05+0.1*i;
    if (pT*cosh(eta)>3500) continue;
    double corr = myJES->GetInsituCorr(pT*1000,eta);
    g->SetPoint(Np++,eta,corr);
  }
  return g;
}

Graph *GetInsituAsymmGraph(double pT) {
  Graph *g = new Graph(); int Np=0;
  for (int i=0;i<45;++i) {
    double eta=0.05+0.1*i;
    if (pT*cosh(eta)>3500) continue;
    double corrplus = myJES->GetInsituCorr(pT*1000,eta);
    double corrminus = myJES->GetInsituCorr(pT*1000,-eta);
    g->SetPoint(Np++,eta,corrplus-corrminus);
  }
  return g;
}

Graph *GetInsituGraphVsPt(double eta) {
  Graph *g = new Graph(); int Np=0;
  double min=log(15), max=log(2500);
  for (int i=0;i<=100;++i) {
    double pt=exp(min+(max-min)*i/100);
    double corr = myJES->GetInsituCorr(pt*1000,eta);
    g->SetPoint(Np++,pt,corr);
  }
  return g;
}

/*
 *   Drawing of labels and formatting
 */

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

void FormatGraph(Graph *g, int style) {
  g->SetLineColor(colors[style]); g->SetMarkerColor(colors[style]);
  g->SetMarkerStyle(markers[style]); g->SetMarkerSize(msizes[style]);
}

bool DrawPileupCorr(Str jetAlgo) {
  JetCalibrationTool *theJES = new JetCalibrationTool(jetAlgo,_jesFile,false);
  bool residual = theJES->JetAreaJES();

  TH1D *temp = new TH1D("","",100,-5,5);
  temp->SetXTitle("Jet #eta"); temp->SetYTitle("Jet offset at "+GetConstScale(jetAlgo)+"-scale [GeV]");
  if (residual) temp->SetYTitle("Residual jet offset at "+GetConstScale(jetAlgo)+"-scale [GeV]");
  temp->SetMinimum(-20); temp->SetMaximum(15); temp->SetStats(0);
  if ( residual && theJES->ResidualOffsetCorr_Description() == "" ) return false;
  Can->Clear(); temp->Draw();
  for (int npv=0;npv<9;++npv) {
    double NPV=(npv%3)*10, mu=(npv/3)*10;
    if (NPV==0) NPV=1;
    Graph *g = new Graph();
    FormatGraph(g,npv);
    for (int ieta=-50;ieta<50;++ieta) {
      double eta=0.05 + 0.1*ieta;
      double O = residual ? theJES->GetResidualOffset(eta,mu,NPV)/1000 :
	theJES->GetOffset(eta,mu,NPV)/1000; // convert to GeV
      g->SetPoint(g->GetN(),eta,O);
    }
    if (npv<4) DrawLabel(Form("N_{PV} = %.0f, #LT#mu#GT = %.0f",NPV,mu),0.18,0.35-0.04*npv,npv);
    else DrawLabel(Form("N_{PV} = %.0f, #LT#mu#GT = %.0f",NPV,mu),0.48,0.35-0.04*(npv-4),npv);
    
    g->Draw("P");
  }
  tex->SetNDC(); tex->SetTextAlign(12);
  if (residual)
    tex->DrawLatex(0.18,0.975,theJES->ResidualOffsetCorr_Description());
  else 
    tex->DrawLatex(0.18,0.975,theJES->OffsetCorr_Description());
  tex->DrawLatex(0.18,0.9,GetJetDesc(jetAlgo));
  return true;
}

void DrawPileupCorrVsMuNPV(Str jetAlgo) {
  JetCalibrationTool *theJES = new JetCalibrationTool(jetAlgo,_jesFile);
  bool residual = theJES->JetAreaJES();

  TH1D *tempNPV = new TH1D("","",12,0,20);
  TH1D *tempmu  = new TH1D("","",15,0,25);
  tempNPV->SetXTitle("Number of vertices, #it{N}_{PV}");
  tempmu->SetXTitle("Average number of interactions, #LT#it{#mu}#GT"); 
  double etaBins[] = {0, 0.3, 0.8, 1.2, 2.1, 2.8, 3.2, 3.6, 4.5};
  Str tit=residual?"Residual jet offset":"Jet offset";
  tempNPV->SetYTitle(tit+" at "+GetConstScale(jetAlgo)+"-scale [GeV]");
  tempmu->SetYTitle(tit+" at "+GetConstScale(jetAlgo)+"-scale [GeV]");
  tempNPV->SetMinimum(-8); tempNPV->SetMaximum(8); tempNPV->SetStats(0); 
  tempNPV->GetYaxis()->SetTitleOffset(1.0); tempNPV->GetXaxis()->SetTitleOffset(1.0);
  tempmu->SetMinimum(-8); tempmu->SetMaximum(8); tempmu->SetStats(0); 
  tempmu->GetYaxis()->SetTitleOffset(1.0); tempmu->GetXaxis()->SetTitleOffset(1.0);
  if (residual && theJES->ResidualOffsetCorr_Description()=="") return;

  Can->Clear(); tempNPV->Draw();
  for (int ei=0;ei<5;++ei) {
    double eta=0.5;
    if (ei==1) eta=1.0;
    if (ei==2) eta=2.0;
    if (ei==3) eta=3.0;
    if (ei==4) eta=4.0;
    Graph *g = new Graph();
    FormatGraph(g,ei);
    for (int npv=1;npv<=20;++npv)
      if (residual)
	g->SetPoint(g->GetN(),npv,theJES->GetResidualOffset(eta,0.0,npv)/1000);
      else
	g->SetPoint(g->GetN(),npv,theJES->GetOffset(eta,0.0,npv)/1000);
    g->Draw("PL");
  }
  DrawLabel("0.3 #leq |#eta| < 0.8",0.7,0.37,0);
  DrawLabel("0.8 #leq |#eta| < 1.2",0.7,0.37-0.05,1);
  DrawLabel("2.1 #leq |#eta| < 2.8",0.7,0.37-0.1,2);
  DrawLabel("2.8 #leq |#eta| < 3.2",0.7,0.37-0.15,3);
  DrawLabel("3.6 #leq |#eta| < 4.5",0.7,0.37-0.2,4);
  tex->SetNDC(); tex->SetTextAlign(12);
  //tex->DrawLatex(0.18,0.975,myJES->OffsetCorr_Description());
  int jetR=jetAlgo.Contains("Kt4")?4:6;
  tex->DrawLatex(0.18,0.9,Form("Anti-k_{t} #it{R} = 0.%d, %s+JES",jetR,GetConstScale(jetAlgo).Data()));
  Can->Print(ps); 
  
  Can->Clear(); tempmu->Draw();
  for (int ei=0;ei<5;++ei) {
    double eta=0.5;
    if (ei==1) eta=1.0;
    if (ei==2) eta=2.0;
    if (ei==3) eta=3.0;
    if (ei==4) eta=4.0;
    Graph *g = new Graph();
      FormatGraph(g,ei);
      for (int mu=0;mu<=25;++mu) 
	if (residual)
	  g->SetPoint(g->GetN(),mu,theJES->GetResidualOffset(eta,mu,1)/1000);
	else
	  g->SetPoint(g->GetN(),mu,theJES->GetOffset(eta,mu,1)/1000);
      g->Draw("PL");
  }
  DrawLabel("0.3 #leq |#eta| < 0.8",0.2,0.37,0);
  DrawLabel("0.8 #leq |#eta| < 1.2",0.2,0.37-0.05,1);
  DrawLabel("2.1 #leq |#eta| < 2.8",0.2,0.37-0.1,2);
  DrawLabel("2.8 #leq |#eta| < 3.2",0.2,0.37-0.15,3);
  DrawLabel("3.6 #leq |#eta| < 4.5",0.2,0.37-0.2,4);
  tex->SetNDC(); tex->SetTextAlign(12);
  //tex->DrawLatex(0.18,0.975,myJES->OffsetCorr_Description());
  tex->DrawLatex(0.18,0.9,Form("Anti-k_{t} #font[52]{R} = 0.%d, %s+JES",jetR,GetConstScale(jetAlgo).Data()));
  Can->Print(ps);
}
