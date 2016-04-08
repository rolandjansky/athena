/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "atlasstyle/AtlasStyle.C"
#include "atlasstyle/AtlasLabels.C"
#include "TMath.h"

using namespace std;
typedef TString Str;
typedef MultijetJESUncertaintyProvider JESprovider;
typedef std::vector<TH1D*> HistV;

std::vector<double> ptBins, etaBins; 
Str _jetAlgo, _JESconfig;
int _nNPmax=8;
bool _jetAreaJES; // Moriond2013 JES or later

// color and line-styles
int cols[7] = {kRed,kBlue,kGreen+1,kGray+1,kOrange+2,kViolet-5,kCyan-2};
int ls[5] = {1,2,3,4,5};
int getCol(int i) { return cols[i%7]; }
int getLS(int i) { return ls[i%5]; }

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
void DrawJEScomp(HistV nps, StrV compNames, Str msg, double var, bool vsPt);
void DrawJEScompVsPt(HistV nps, StrV compNames, double eta, Str msg) {
  DrawJEScomp(nps,compNames,msg,eta,true);
}
void DrawJEScompVsEta(HistV nps, StrV compNames, double pt, Str msg) {
  DrawJEScomp(nps,compNames,msg,pt,false);
}

int main(int argc, char **argv) {

  if (argc<3) error("Need to specify JES and MJES configs as arguments"); 
  _JESconfig = argv[1];
  Str mJES = argv[2];
 
  std::cout << "String argv[2] is " << mJES << std::endl;
 
  printf("\n================================================\n");
  printf("  Creating JES uncertainty plots for configs\n    %s\n    %s\n",
	 _JESconfig.Data(),mJES.Data());
  
  // create Plots output dir - if it doesn't exist already
  gSystem->Exec("mkdir -p JES_Plots");
  Str ps="JES_Plots/"+Str(gSystem->BaseName(_JESconfig)).ReplaceAll(".config",".ps");

  _jetAreaJES = _JESconfig.Contains("InsituJES2012");
  
  SetAtlasStyle(); gErrorIgnoreLevel=2000;
  
  TCanvas *c = new TCanvas();
  TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04); tex->SetTextFont(42);
  c->SetMargin(0.12,0.04,0.15,0.04); //c->SetGridy();
  c->Print(ps+"["); c->SetLogx();
  TLine *line = new TLine(); line->SetLineStyle(2);
  
  //log binning
  int N = 1000; double min = 15, max = 2500;
  double dx=(log(max)-log(min))/N;
  for (int i=0;i<=N;++i) ptBins.push_back(exp(log(min)+i*dx));
  for (int i=0;i<=N;++i) etaBins.push_back(-4.5+9.0*i/N);
  
  for (int ji=0;ji<4;++ji) {
    int jetR = ji<2?4:6; Str constScale = ji%2==0?"EM":"LC";
    _jetAlgo = Form("AntiKt%dTopo",jetR); _jetAlgo+=constScale;
    Str mc = _JESconfig.Contains("2012") ? "MC12a" : "MC11c";
    
    JESprovider j(mJES,_JESconfig,_jetAlgo,mc,"../share"); j.useGeV();

    double mu=8.5, npv=10, rms=3;
    if (_JESconfig.Contains("2012")) { mu=20; npv=13; rms=5; };
    double closeR = jetR == 4? 0.7 : 0.9;

    c->SetLogx();
    for (int ei=0;ei<2;++ei) {
      double eta=ei==0?0.5:2.0;
      Str suf="_"+_jetAlgo+Form("_eta%.1f",eta);

      StrV extraJES = JESUtils::Vectorize("PileupMu PileupNPV Closeby Flavour bJES"," ");
      StrV compNames = j.getComponentNames();
      
      HistV nps;
      for (int ci=0;ci<compNames.size();++ci) {
	nps.push_back(new TH1D(compNames[ci]+suf,"",N,&ptBins[0]));
	for (int bin=1;bin<=nps[ci]->GetNbinsX();++bin) {
	  double pt=nps[ci]->GetBinCenter(bin);
	  nps[ci]->SetBinContent(bin,j.getRelUncertComponent(compNames[ci],pt,eta));
	}
      }

      if (nps.size()+extraJES.size()>_nNPmax) {
	if (_jetAreaJES)
	  DrawJEScompVsPt(nps,compNames,eta,"#it{In situ} JES uncertainties");
	else
	  DrawJEScompVsPt(nps,compNames,eta,"Baseline JES uncertainties");
	c->Print(ps);
	TH1D *base = AddInQuad(nps);
	nps.clear(); compNames.clear();
	nps.push_back(base); compNames.push_back(_jetAreaJES?"#it{In situ} JES":"Baseline JES");
      }
      
      for (int ci=0;ci<extraJES.size();++ci) {
	nps.push_back(new TH1D(extraJES[ci]+suf,"",N,&ptBins[0]));
	compNames.push_back(extraJES[ci]);
	for (int bin=1;bin<=nps.back()->GetNbinsX();++bin) {
	  double pt = nps.back()->GetBinCenter(bin);
	  nps.back()->SetBinContent(bin,GetMultiUncert(j,extraJES[ci],pt,eta,closeR,mu,npv,rms));
	}
      }
      
      DrawJEScompVsPt(nps,compNames,eta,"All JES uncertainties");
      c->Print(ps);      
    } // for each eta bin
    
    c->SetLogx(0);
    for (int pti=0;pti<2;++pti) {
      double pt=pti==0?25:300;
      Str suf="_"+_jetAlgo+Form("_pt%.0f",pt);
      
      StrV extraJES = JESUtils::Vectorize("PileupMu PileupNPV Closeby Flavour bJES"," ");
      StrV compNames = j.getComponentNames();
      
      HistV nps;
      for (int ci=0;ci<compNames.size();++ci) {
	nps.push_back(new TH1D(compNames[ci]+suf,"",N,&etaBins[0]));
	for (int bin=1;bin<=nps.back()->GetNbinsX();++bin) {
	  double eta=nps.back()->GetBinCenter(bin);
	  nps.back()->SetBinContent(bin,j.getRelUncertComponent(compNames[ci],pt,eta));
	}
      }

      if (nps.size()+extraJES.size()>_nNPmax) {
	//printf("More than %d!!!\n",_nNPmax);
	DrawJEScompVsEta(nps,compNames,pt,_jetAreaJES?"#it{In situ} JES uncertainties":"Baseline JES uncertainties");
	c->Print(ps);
	TH1D *base = AddInQuad(nps);
	nps.clear(); compNames.clear();
	nps.push_back(base); 
	compNames.push_back(_jetAreaJES?"#it{In situ} JES":"Baseline JES");
      }
      
      for (int ci=0;ci<extraJES.size();++ci) {
	nps.push_back(new TH1D(extraJES[ci]+suf,"",N,&etaBins[0]));
	compNames.push_back(extraJES[ci]);
	for (int bin=1;bin<=nps.back()->GetNbinsX();++bin) {
	  double eta = nps.back()->GetBinCenter(bin);
	  nps.back()->SetBinContent(bin,GetMultiUncert(j,extraJES[ci],pt,eta,closeR,mu,npv,rms));
	}
      }
      
      DrawJEScompVsEta(nps,compNames,pt,"All JES uncertainties");
      c->Print(ps);
    } // for each pT bin
  } // for each jet algo
  c->Print(ps+"]");
  Str pdf=ps; pdf.ReplaceAll(".ps",".pdf");
  gSystem->Exec(Form("ps2pdf %s %s",ps.Data(),pdf.Data()));
  gSystem->Exec(Form("rm %s",ps.Data()));
  printf("\nProduced\n  %s\n================================================\n",pdf.Data());
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
  int jetR  = jetAlgo.Contains("Kt4")?4:6;
  return Form("Anti-k_{t} #font[52]{R} = 0.%d, %s",jetR,calib.Data());
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
  double ptMax=3000/cosh(eta); if (ptMax>2500) ptMax=2500;
  h->SetMinimum(0); h->SetMaximum(0.12); h->SetLineColor(col); h->SetLineStyle(style);
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

void DrawJEScomp(HistV nps, StrV compNames, Str msg, double var, bool vsPt) {
  TH1D *tot = AddInQuad(nps);
  if (compNames.back()=="bJES") tot=AddInQuadButOne(nps);
  
  if (vsPt) DrawPtHisto(tot,var,"",kBlack,1,1);
  else DrawVsEtaHisto(tot,var,"",kBlack,1,1);
  tot->SetFillColor(590); tot->Draw("histFsame"); //tot->Draw("sameLaxis");
  for (int i=0;i<nps.size();++i)
    if (vsPt) DrawPtHisto(nps[i],var,"samel",getCol(i),getLS(i));
    else      DrawVsEtaHisto(nps[i],var,"samel",getCol(i),getLS(i));
  
  //double legx=0.5, legy=0.84, dy=0.045; DrawAtlasLabel(true);
  double legx=0.56, legy=0.90, dy=0.045;
  DrawFillLabel("Total uncertainty",legx,legy,tot);
  for (int i=0;i<nps.size();++i) {
    if (compNames[i]=="bJES")
      DrawLineLabel(compNames[i]+" (not incl. in total)",legx,legy-=dy,nps[i]);
    else
      DrawLineLabel(compNames[i],legx,legy-=dy,nps[i]);
  }
  DrawText(GetJetDesc(_jetAlgo),kBlack,0.905);
  DrawText(msg,kBlack,0.855);
  if (vsPt) DrawText(Form("#eta = %.1f",var),kBlack,0.8);
  else      DrawText(Form("#it{p}_{T}^{jet} = %.0f GeV",var),kBlack,0.8);
  
  DrawText("#bf{"+_JESconfig+"}",kViolet-6,0.98,0.14);
}

// Add a new DrawJEScomp to compare new to old.
void DrawJEScompToOld(HistV nps, StrV compNames, Str msg, double var, bool vsPt, HistV compTot, StrV compCompNames) {

  TH1D *tot = AddInQuad(nps);
  TH1D *compTot = AddInQuad(compTot); 
  if (compNames.back()=="bJES") tot=AddInQuadButOne(nps);
  if (compCompNames.back()=="bJES") compTot=AddInQuadButOne(compTot);
 
  // Add cross-hatched version with the new uncertainties
  // and keep violet for old uncertainties.
  // Let black band indicate edge of old uncertainties and green band indicate edge of new.
  if (vsPt) {
    DrawPtHisto(compTot,var,"",kBlack,1,1);
    DrawPtHisto(tot,var,"",kGreen,1,1);
  } else {
    DrawVsEtaHisto(compTot,var,"",kBlack,1,1);
    DrawVsEtaHisto(tot,var,"",kGreen,1,1);
  }
  compTot->SetFillColor(590); compTot->Draw("histFsame"); //tot->Draw("sameLaxis");
  tot->SetFillColor(kGreen); tot->SetFillStyle(3004); tot->Draw("histFsame");

  for (int i=0;i<nps.size();++i)
    if (vsPt) DrawPtHisto(nps[i],var,"samel",getCol(i),getLS(i));
    else      DrawVsEtaHisto(nps[i],var,"samel",getCol(i),getLS(i));
  
  //double legx=0.5, legy=0.84, dy=0.045; DrawAtlasLabel(true);
  double legx=0.56, legy=0.90, dy=0.045;
  DrawFillLabel("Total uncertainty",legx,legy,tot);
  for (int i=0;i<nps.size();++i) {
    if (compNames[i]=="bJES")
      DrawLineLabel(compNames[i]+" (not incl. in total)",legx,legy-=dy,nps[i]);
    else
      DrawLineLabel(compNames[i],legx,legy-=dy,nps[i]);
  }
  DrawText(GetJetDesc(_jetAlgo),kBlack,0.905);
  DrawText(msg,kBlack,0.855);
  if (vsPt) DrawText(Form("#eta = %.1f",var),kBlack,0.8);
  else      DrawText(Form("#it{p}_{T}^{jet} = %.0f GeV",var),kBlack,0.8);
  
  DrawText("#bf{"+_JESconfig+"}",kViolet-6,0.98,0.14);

}

