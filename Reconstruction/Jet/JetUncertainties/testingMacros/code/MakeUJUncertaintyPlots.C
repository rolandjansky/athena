/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "JetUncertainties/UJUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "atlasstyle/AtlasStyle.C"
#include "atlasstyle/AtlasLabels.C"
#include "TMath.h"

using namespace std;
typedef TString Str;
typedef MultijetJESUncertaintyProvider JESprovider;
typedef UJUncertaintyProvider UJUprovider;
typedef std::vector<TH1D*> HistV;

std::vector<double> ptBins, etaBins; 
Str _jetAlgo, _JESconfig;
int _nNPmax=8;
bool _jetAreaJES; // Moriond2013 JES or later

bool logy=true;
bool display_leg=false;

// color and line-styles
int cols[8] = {kRed,kBlue,kGreen+1,kGray+1,kOrange+2,kViolet-5,kTeal-7,kMagenta};
int ls[6] = {1,1,5,4,3,3};
int getCol(int i) { return cols[i%8]; }
int getLS(int i) { return ls[i%6]; }

void DrawText(Str txt, int col=kBlack, double y=0.88, double x=0.15, int align=12, double ts=0.038);
void DrawLineLabel(Str txt, double x, double y, int col, int ls, int lw);
void DrawLineLabel(Str txt, double x, double y, TH1 *h);
void DrawFillLabel(Str txt, double x, double y, TH1 *h);
Str GetJetDesc(Str jetAlgo);
void DrawAtlasLabel(bool right=false);
double GetPileupUncertainty(JESprovider &jes, double pt, double eta, 
			    double npv, double mu, double rms, int muNPV=-1);

double DrawPtHisto(TH1 *h, double eta, Str opt, int col=kRed, int style=1, int width=4);
double DrawVsEtaHisto(TH1 *h, double pt, Str opt, int col=kRed, int style=1, int width=4, double max=-1);
TH1D *AddInQuad(std::vector<TH1D*> hists, bool avoidLast=false);
TH1D *AddInQuadButOne(std::vector<TH1D*> hists) { return AddInQuad(hists,true); }
void error(Str msg) { printf("ERROR:\n  %s\n",msg.Data()); abort(); }
double GetMultiUncert(JESprovider &j, Str name, double pt, double eta, 
		      double DRmin, double mu, double NPV, double rms);
void DrawJEScomp(HistV nps, StrV compNames, Str msg, double var, double var2, bool vsPt);
void DrawJEScompVsPt(HistV nps, StrV compNames, double eta, double mop, Str msg) {
  DrawJEScomp(nps,compNames,msg,eta,mop,true);
}
void DrawJEScompVsEta(HistV nps, StrV compNames, double pt, double mop, Str msg) {
  DrawJEScomp(nps,compNames,msg,pt,mop,false);
}

int main(int argc, char **argv) {

  if (argc<3) error("Need to specify the rootfile and the jetalgo as arguments"); 
  Str rootfile = argv[1];
  _jetAlgo = argv[2];//"AntiKt10LCTopo";
  

  printf("\n================================================\n");
  printf("  Creating JES uncertainty plots for rootfile\n    %s\n",
	 rootfile.Data());
  
  // create Plots output dir - if it doesn't exist already
  gSystem->Exec("mkdir -p JES_Plots");
  Str ps="JES_Plots/"+Str(gSystem->BaseName(rootfile)).ReplaceAll(".root",Form("_%s.ps",_jetAlgo.Data()));

  //_jetAreaJES = _JESconfig.Contains("InsituJES2012");
  
  SetAtlasStyle(); gErrorIgnoreLevel=2000;
  
  TCanvas *c = new TCanvas();
  TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04); tex->SetTextFont(42);
  c->SetMargin(0.12,0.04,0.15,0.04); //c->SetGridy();
  c->Print(ps+"["); //c->SetLogx();
  TLine *line = new TLine(); line->SetLineStyle(2);
  
  //log binning
  //int N = 1000; double min = 15, max = 2500;
  //for gamma+jet
  //int N = 1000; double min = 45, max = 2500;
  int N = 1000; double min = 45, max = 1500;
  c->SetLogx();
  if (logy) c->SetLogy();
  max = 2500;

  double dx=(log(max)-log(min))/N;
  for (int i=0;i<=N;++i) ptBins.push_back(exp(log(min)+i*dx));
  for (int i=0;i<=N;++i) etaBins.push_back(-4.5+9.0*i/N);

  Str mc2 = rootfile.Contains("2012") ? "MC12a" : "MC11c";

  UJUprovider j2(rootfile,_jetAlgo,mc2);

  j2.useGeV();

  //const unsigned int BIN_mop = 6;
  //const unsigned int BIN_eta = 2;
  //double mop_range[BIN_mop+1] = {0,0.2,0.3,0.4,0.5,0.6,1.0};
  //double eta_range[BIN_eta+1] = {0,0.8,1.2};
  //for gamma+jet
  //const unsigned int BIN_mop = 1;
  //const unsigned int BIN_eta = 5;
  //double mop_range[BIN_mop+1] = {0,1.0};
  //double eta_range[BIN_eta+1] = {0,0.8,1.2,1.5,1.8,2.2};

  const unsigned int BIN_mop = 6;
  const unsigned int BIN_eta = 4;
  double mop_range[BIN_mop+1] = {0,0.2,0.3,0.4,0.5,0.6,1.0};
  double eta_range[BIN_eta+1] = {-0.001,0.001,0.8,1.195,2.205};

  //j2.usePermissive(true);
  /*
  j3.usePermissive(true);
  j4.usePermissive(true);
  */

  StrV compNames = JESUtils::Vectorize("MoverPt generator stats dPhiCut kterm photonPurity dataMC pt2Cut PES more1smallJetInsideLargeR JER akt4insideOutsideLargeR topology DoubleRatioInterpolation700to900 DoubleRatioInterpolation800to1000 DoubleRatioInterpolation900to1100"," ");
  int component = 16;//placeholder if we have more than one component

  bool plot_total = false;

  TH1D *h_default = new TH1D("h_default","",N,&ptBins[0]);

  for (int it_eta=0;it_eta<BIN_eta;++it_eta){
    for (int it_mop=0;it_mop<BIN_mop;++it_mop){

      Str suf="JPTS_"+_jetAlgo;
      suf += Form("_mop%.1f-%.1f",mop_range[it_mop],mop_range[it_mop+1]);
      suf += Form("_eta%.1f-%.1f",eta_range[it_eta],eta_range[it_eta+1]);
      //printf(suf.Data());
      //printf("\n");

      double eta = 0.5*(eta_range[it_eta]+eta_range[it_eta+1]);
      double mop = 0.5*(mop_range[it_mop]+mop_range[it_mop+1]);

      HistV nps;
      if (plot_total){
	  nps.push_back(new TH1D("12 components"+suf,"",N,&ptBins[0]));
	  nps.push_back(new TH1D("12 comp. + interpolation"+suf,"",N,&ptBins[0]));
	  nps.push_back(new TH1D("12 comp. + interpolation900to1100"+suf,"",N,&ptBins[0]));
	  nps.push_back(new TH1D("12 comp. + interpolation + topology"+suf,"",N,&ptBins[0]));
      }

      for (int bin=1;bin<=h_default->GetNbinsX();++bin) {

	double total1_square = 0;
	double total2_square = 0;
	double total3_square = 0;
	double total4_square = 0;

	double pt=h_default->GetBinCenter(bin);
	for (int it_component=0;it_component<component;++it_component) {//placeholder if we have more than one component

	  if ((plot_total == false) and (bin==1)){
	    nps.push_back(new TH1D(compNames[it_component]+suf,"",N,&ptBins[0]));
	  }

	  double value = 0;
	  if ((it_component != 12) or (pt > 200))
	    value = j2.getRelUncertComponent("Component_"+compNames[it_component], pt, mop*pt, eta, UJUprovider::JPTS);
	  //double value = j2.getRelUncertComponent(it_component, pt, mop*pt, eta, UJUprovider::JPTS);

	  if (it_component < 12) {
	    total1_square += value*value;
	    total2_square += value*value;
	    total3_square += value*value;
	    total4_square += value*value;
	  }
	  if (it_component == 12) total4_square += value*value;
	  if (it_component == 13) {
	    total2_square += value*value;
	    total4_square += value*value;
	  }
	  if (it_component == 15) total3_square += value*value;

	  if (plot_total == false){
	    nps[it_component]->SetBinContent(bin,value);
	  }
	}

	if (plot_total){
	  nps[0]->SetBinContent(bin,sqrt(total1_square));
	  nps[1]->SetBinContent(bin,sqrt(total2_square));
	  nps[2]->SetBinContent(bin,sqrt(total3_square));
	  nps[3]->SetBinContent(bin,sqrt(total4_square));
	}
      }

      if (plot_total){
	StrV tcn = JESUtils::Vectorize("12 componentsY12 comp. + interpolationY12 comp. + interpolation900to1100Y12 comp. + interpolation + topology","Y");
	DrawJEScompVsPt(nps,tcn,eta,mop,"JPTS uncertainties");
      } else {
	DrawJEScompVsPt(nps,compNames,eta,mop,"JPTS uncertainties");
      }
      c->Print(ps);

      /*
      if (component > 1){//placeholder if we have more than one component
	TH1D *base = AddInQuad(nps);
	nps.clear(); compNames.clear();
	nps.push_back(base); compNames.push_back(variableS[it_var]);
	DrawJEScompVsPt(nps,compNames,eta,mop,"All "+variableS[it_var]+" uncertainties");
	c->Print(ps);      
      }
      */
    }
  }

  c->Print(ps+"]");
  Str pdf=ps; pdf.ReplaceAll(".ps",".pdf");
  gSystem->Exec(Form("ps2pdf %s %s",ps.Data(),pdf.Data()));
  gSystem->Exec(Form("rm %s",ps.Data()));
  printf("\nProduced\n  %s\n================================================\n",pdf.Data());

  //double value = j2.getRelUncertComponent(0, 250, 120, 0.5, UJUprovider::JPTS);
  //printf("\npt=250, mop=0.48, eta=0.5, -> %.5f\n",value);
  
  return 0;
}

// Draw text to the left
void DrawText(Str txt, int col, double y, double x, int align, double ts) { 
  static TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(ts);
  tex->SetTextAlign(align); tex->SetTextColor(col); tex->DrawLatex(x,y,txt); 
}

void DrawLineLabel(Str txt, double x, double y, int col, int ls, int lw) {
  static TLine *l = new TLine(); l->SetLineWidth(lw); l->SetLineColor(col); l->SetLineStyle(ls);
  l->DrawLineNDC(x-0.02,y,x+0.02,y); DrawText(txt,kBlack,y,x+0.032,12,0.036);
}

void DrawLineLabel(Str txt, double x, double y, TH1 *h) {
  DrawLineLabel(txt,x,y,h->GetLineColor(),h->GetLineStyle(),h->GetLineWidth());
}

void DrawFillLabel(Str txt, double x, double y, TH1 *h) {
  static TPave *box = new TPave(); 
  box->SetFillColor(590); 
  box->SetLineColor(h->GetLineColor()); box->SetLineWidth(1);
  box->DrawPave(x-0.02,y-0.015,x+0.02,y+0.015,h->GetLineWidth(),"ndc");
  double x1 = gPad->GetX1(), xden=(gPad->GetX2()-gPad->GetX1());
  double y1 = gPad->GetY1(), yden=(gPad->GetY2()-gPad->GetY1());
  DrawText(txt,kBlack,y,x+0.032,12,0.036);
}

Str GetJetDesc(Str jetAlgo) {
  Str calib = jetAlgo.Contains("TopoEM")?"EM+JES":"LCW+JES";
  int jetR  = 4;
  if (jetAlgo.Contains("Kt6")) jetR = 6;
  if (jetAlgo.Contains("Kt8")) jetR = 8;
  if (jetAlgo.Contains("Kt10")) jetR = 10;
  if (jetAlgo.Contains("Kt12")) jetR = 12;
  
  Str algo = "Anti-k_{t}";
  if (jetAlgo.Contains("CamKt")) algo = "C/A";
  return Form("%s #font[52]{R} = 0.%d, %s",algo.Data(),jetR,calib.Data());
}

void DrawAtlasLabel(bool right) {
  ATLASLabel(right?0.70:0.15,0.89,(char*)"Internal",kBlack);
}

double GetPileupUncertainty(JESprovider &jes, double pt, double eta, 
			    double npv, double mu, double rms, int muNPV) {
  // returns average shift: NPV and mu terms added in quadrature
  //return jes.JESUncertaintyProvider::getRelOffsetUncert(pt,eta,npv,mu);

  // sample mu and NPV from a gaussian to get the offset uncertainty
  // wiidth and average of gaussian inspired by the in situ samples used for calibration
  int N=20; double sumw=0, sumwu=0;
  for (int i=-N;i<=N;++i) {
    double x = 3.0*rms/N*i, w = TMath::Gaus(x,0,rms);
    sumw+=w; sumwu += w*jes.getRelNPVOffsetTerm(pt,eta,npv+x);
    //sumw+=w; sumwu += w*jes.getRelMuOffsetTerm(pt,eta,mu);
  }
  double sumwMu=0, sumwuMu=0;
  for (int i=-N;i<=N;++i) {
    double x = 3.0*rms/N*i, w = TMath::Gaus(x,0,rms);
    sumwMu+=w; sumwuMu += w*jes.getRelMuOffsetTerm(pt,eta,mu+x);
  }
  
  if (muNPV==0) return sumwuMu/sumwMu;
  if (muNPV==1) return sumwu/sumw;
  return sqrt(pow(sumwu/sumw,2)+pow(sumwuMu/sumwMu,2));
  
  //double MU=gRandom->Gaus(mu,rms), NPV=gRandom->Gaus(npv,rms);
  //uncSum += jes.getRelNPVOffsetTerm(pt,eta,NPV);
  //uncSum += jes.getRelMuOffsetTerm(pt,eta,MU);
  //uncSum += jes.JESUncertaintyProvider::getRelOffsetUncert(pt,eta,NPV,MU);
  //uncSum += jes.JESUncertaintyProvider::getRelOffsetUncert(pt,eta,NPV,MU);
  //return uncSum/N;
}

TH1D *AddInQuad(std::vector<TH1D*> hists, bool avoidLast) {
  TH1D *tot = (TH1D*)hists[0]->Clone(Str(hists[0]->GetName())+"tot");
  for (int xi=1;xi<=tot->GetNbinsX();++xi) {
    double sum_u2=0;
    for (int i=0;i<hists.size()-int(avoidLast);++i)
      sum_u2 += pow(hists[i]->GetBinContent(xi),2);
    tot->SetBinContent(xi,sqrt(sum_u2));
  }
  tot->SetLineColor(kBlack); tot->SetLineStyle(2); tot->SetLineWidth(3);
  return tot;
}

double DrawPtHisto(TH1 *h, double eta, Str opt, int col, int style, int width) {
  //double ptMax=3000/cosh(eta); 
  double ptMax=3000; 
  if (ptMax>2500) ptMax=2500;
  //h->SetMinimum(0);
  double max = h->GetMaximum()*1.25;
  //if (max < 0.12) max=0.12;
  //h->SetMaximum(max);
  h->SetLineColor(col); h->SetLineStyle(style);
  h->SetLineWidth(width); h->SetXTitle("#it{p}_{T}^{jet} [GeV]"); h->SetYTitle("Fractional JES uncertainty");
  h->GetXaxis()->SetMoreLogLabels();
  h->GetXaxis()->SetTitleOffset(1.4); h->GetYaxis()->SetTitleOffset(1.25);
  h->GetXaxis()->SetRangeUser(20,ptMax);
  h->Draw(opt); return ptMax;
}

double DrawVsEtaHisto(TH1 *h, double pt, Str opt, int col, int style, int width, double max) {
  double etaMax=TMath::ACosH(1500/pt); if (etaMax>4.2) etaMax=4.2;
  h->SetMinimum(0); h->SetMaximum(0.16); h->SetLineColor(col); h->SetLineStyle(style);
  if (etaMax<3) h->SetMaximum(0.06);
  if (max>0) h->SetMaximum(max);
  h->SetLineWidth(width); h->SetXTitle("#eta"); h->SetYTitle("Fractional JES uncertainty");
  h->GetXaxis()->SetTitleOffset(1.4); h->GetYaxis()->SetTitleOffset(1.25);
  h->GetXaxis()->SetRangeUser(-etaMax,etaMax);
  h->Draw(opt); return etaMax;
}


double GetMultiUncert(JESprovider &j, Str name, double pt, double eta, 
		      double DRmin, double mu, double NPV, double rms) {
  // for closeby: assume a 20 GeV jet at DR=0.8 distance
  if (name=="Closeby")
    return _jetAreaJES?j.getRelClosebyUncert(pt,20.0*cos(0.8)/pt):j.getRelClosebyUncert(pt,DRmin);
  else if (name=="PileupMu") 
    return GetPileupUncertainty(j,pt,eta,NPV,mu,rms,0); 
  else if (name=="PileupNPV") 
    return GetPileupUncertainty(j,pt,eta,NPV,mu,rms,1); 
  else if (name=="Flavour") 
    return j.getRelFlavorUncert(pt,eta,true);
  else if (name=="FlavourResponse") 
    return j.getRelFlavorResponseUncert(pt,eta,true);
  else if (name=="FlavourComposition") 
    return j.getRelFlavorCompUncert(pt,eta,true);
  else if (name=="bJES") 
    return j.getRelBJESUncert(pt,eta);
  else
    error("Don't know about mulitjet JES component "+name);
  
  return 0;
}

void DrawJEScomp(HistV nps, StrV compNames, Str msg, double var, double var2, bool vsPt) {
  /*
  TH1D *tot = AddInQuad(nps);
  if (compNames.back()=="bJES") tot=AddInQuadButOne(nps);
  
  if (vsPt) DrawPtHisto(tot,var,"",kBlack,1,1);
  else DrawVsEtaHisto(tot,var,"",kBlack,1,1);
  tot->SetFillColor(590); tot->Draw("histFsame"); //tot->Draw("sameLaxis");
  */
  if (logy==false){
    nps[0]->SetMaximum(0.06);
    nps[0]->SetMinimum(0.00);
  } else {
    nps[0]->SetMaximum(0.08);
    nps[0]->SetMinimum(0.0001);
  }
  if (vsPt) DrawPtHisto(nps[0],var,"",kBlack,1,1);
  else DrawVsEtaHisto(nps[0],var,"",kBlack,1,1);
  nps[0]->Draw("histFsame");
  for (int i=0;i<nps.size();++i)
    if (vsPt) DrawPtHisto(nps[i],var,"samel",getCol(i),getLS(i));
    else      DrawVsEtaHisto(nps[i],var,"samel",getCol(i),getLS(i));
  
  //double legx=0.5, legy=0.84, dy=0.045; DrawAtlasLabel(true);
  double legx=0.5, legy=0.90, dy=0.04;
  //DrawFillLabel("Total uncertainty",legx,legy,tot);
  if (display_leg){
    for (int i=0;i<nps.size();++i) {
      if (compNames[i]=="bJES")
	DrawLineLabel(compNames[i]+" (not incl. in total)",legx,legy-=dy,nps[i]);
      else
	DrawLineLabel(compNames[i],legx,legy-=dy,nps[i]);
    }
  }

  /*
  static TLine *l = new TLine();
  l->SetLineWidth(2);
  l->SetLineColor(kGray);
  l->SetLineStyle(1);
  l->DrawLine(800,nps[0]->GetMinimum(),800,nps[0]->GetMaximum());
  */

  //DrawText(GetJetDesc(_jetAlgo),kBlack,0.905);
  DrawText(_jetAlgo.Data(),kBlack,0.905);
  DrawText(msg,kBlack,0.855);
  if (vsPt) DrawText(Form("#eta = %.2f, M/p_{T} = %.1f",var,var2),kBlack,0.805);
  else      DrawText(Form("#it{p}_{T}^{jet} = %.0f GeV",var),kBlack,0.805);
  
  //DrawText("#bf{"+_JESconfig+"}",kViolet-6,0.98,0.14);
}

