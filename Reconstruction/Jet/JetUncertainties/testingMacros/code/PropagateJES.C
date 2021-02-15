/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Example code on how to propagate JES uncertainties
 *  
 *  Dag Gillberg, September 27, 2012
 *  
 */

#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TError.h"
#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include <iostream>

using namespace std;

typedef TString Str;
typedef MultijetJESUncertaintyProvider MultijetProvider;
typedef vector<TH1D*> HistV;

// Global variables - being a bit lazy here... bad boy.
HistV pt1, pt2, eta1, eta2, mjj, delta_y, deltaPhi, deltaR, y_star, y_boost;

TH1D *MakeHisto(Str name, int Nbins, double min, double max, Str xtit="", Str ytit="") {
  TH1D *h = new TH1D(name,"",Nbins,min,max); h->SetXTitle(xtit); h->SetYTitle(ytit);
  return h;
}

void FillHistos(TLorentzVector jetA, TLorentzVector jetB, int histIndex);
void DrawComparison(const HistV &histos, int up, int down, Str sysName);

int main(int argc, char **argv) {
  
  Str JESconfig="JES_2012/JESUncertainty2012_Sept2012.config";
  Str MJESconfig="JES_2012/MultijetJES_2012.config";
  if (argc>2) { JESconfig=argv[1]; MJESconfig=argv[2]; }
  gErrorIgnoreLevel=2000; gStyle->SetTitleOffset(1.4,"X"); gStyle->SetTitleOffset(1.4,"Y");
  gStyle->SetPadTopMargin(0.02); gStyle->SetPadBottomMargin(0.12); gStyle->SetPadRightMargin(0.04);
  gStyle->SetOptStat(0);
  
  MultijetProvider *jesProvider = new MultijetProvider(MJESconfig,JESconfig,
						       "AntiKt4TopoEM","MC12a","../share");
  jesProvider->useGeV();

  Str psName="HCP_JES_uncertaity_test.ps";
  TCanvas *c = new TCanvas();
  c->SetMargin(0.12,0.04,0.10,0.02);
  c->Print(psName+"[");

  // make pThat spectrum
  TH1D *pThatPDF = new TH1D("pThat","",100,0,200);
  for (int i=8;i<=200;++i) pThatPDF->SetBinContent(i,pow(pThatPDF->GetBinCenter(i),-2.5));

  // set the seed
  gRandom->SetSeed(1234567);

  // Jet energy resolution
  TF1 *jer = new TF1("JER","sqrt(pow([0]/x,2)+pow([1],2)/x+pow([2],2))",10,1000);
  jer->SetParameters(3.64,0.760,5.16e-02);
  
  TLorentzVector jet1, jet2;

  StrV sysNames = JESUtils::Vectorize("BaselineJES ForwardJES PileupMu PileupNPV Closeby Flavour bJES"," ");

  for (int i=0;i<sysNames.size()*2+1;++i) {
    Str suff="nom";
    if (i>0) {
      suff = sysNames[(i-1)/2];
      suff += ( i%2==0 ? "_up" : "_down" );
    }
    pt1.push_back(MakeHisto("jet1_pt_"+suff,50,0,200,"leading jet p_{T} [GeV]"));
    pt2.push_back(MakeHisto("jet2_pt_"+suff,50,0,200,"second jet p_{T} [GeV]"));
    eta1.push_back(MakeHisto("eta1_pt_"+suff,50,-5,5,"leading jet #eta"));
    eta2.push_back(MakeHisto("eta2_pt_"+suff,50,-5,5,"second jet #eta"));
    mjj.push_back(MakeHisto("mjj_"+suff,50,0,200,"#it{m}(jet1,jet2)"));
    delta_y.push_back(MakeHisto("delta_y_"+suff,50,0,8,"#Delta#it{y}(jet1,jet2)"));
    deltaR.push_back(MakeHisto("deltaR_"+suff,50,0,10,"#Delta#it{R}(jet1,jet2)"));
    deltaPhi.push_back(new TH1D("deltaPhi_"+suff,"",100,0,4));
    y_boost.push_back(MakeHisto("y_boost_"+suff,100,-5,5,"y_{boost} = (y_{jet1}+y_{jet2})/2"));
    y_star.push_back(MakeHisto("y_star_"+suff,100,-5,5,"y^{*} = |y_{jet1}-y_{jet2}|/2"));
  }

  // how many events to generate
  int Nevts=25000;
  for (int ievt=0;ievt<Nevts;++ievt) {

    // Very simplified MC generator

    // sample the pThat
    double pThat = pThatPDF->GetRandom();
    
    // pull yStar and yBoost from Guassians
    double yStar  = gRandom->Gaus(0,1.5);
    double yBoost = gRandom->Gaus(0,2);
    double phi = gRandom->Rndm()*TMath::TwoPi();
    
    double mu = gRandom->Rndm()*20+10;
    double NPV = gRandom->Poisson(0.8*mu); // vtx reco eff = 80%

    // whether this is a b-bbar or (light)-quark/gluon dijet event
    // bq/bg dijet events ignored
    bool is_bb_or_cc = gRandom->Rndm()<0.25;
    double dRmin1 = gRandom->Gaus(0.7,0.3);
    double dRmin2 = gRandom->Gaus(0.7,0.3);

    // create the truth jets
    jet1.SetPtEtaPhiM(pThat,yStar+yBoost,phi,0);
    jet2.SetPtEtaPhiM(pThat,-yStar+yBoost,phi+TMath::Pi(),0);
    
    // cut on eta 
    if ( fabs(jet1.Eta())>4.5|| fabs(jet2.Eta())>4.5 ) continue;

    // Let's go from truth to reco - apply JER
    // note: the JER should be eta dependent not considered here
    jet1 *= gRandom->Gaus(1.0,jer->Eval(jet1.Pt()));
    jet2 *= gRandom->Gaus(1.0,jer->Eval(jet2.Pt()));
    
    // Fill nominal histos
    FillHistos(jet1,jet2,0);
    
    // Baseline JES
    double sys1 = jesProvider->getRelUncertComponent(sysNames[0],jet1.Pt(),jet1.Eta());
    double sys2 = jesProvider->getRelUncertComponent(sysNames[0],jet2.Pt(),jet2.Eta());
    // fill histos after shifting the jets up and down in unison
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 1); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 2); // down

    // Forward JES
    sys1 = jesProvider->getRelUncertComponent(sysNames[1],jet1.Pt(),jet1.Eta());
    sys2 = jesProvider->getRelUncertComponent(sysNames[1],jet2.Pt(),jet2.Eta());
    // fill histos after shifting the jets up and down in unison
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 3); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 4); // down

    // PileupMu
    sys1 = jesProvider->getRelMuOffsetTerm(jet1.Pt(),jet1.Eta(),mu);
    sys2 = jesProvider->getRelMuOffsetTerm(jet2.Pt(),jet2.Eta(),mu);
    // fill histos after shifting the jets up and down in unison
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 5); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 6); // down

    // PileupNPV
    sys1 = jesProvider->getRelNPVOffsetTerm(jet1.Pt(),jet1.Eta(),NPV);
    sys2 = jesProvider->getRelNPVOffsetTerm(jet2.Pt(),jet2.Eta(),NPV);
    // fill histos after shifting the jets up and down in unison
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 7); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 8); // down
    
    // Closeby
    sys1 = jesProvider->getRelClosebyUncert(jet1.Pt(),dRmin1);
    sys2 = jesProvider->getRelClosebyUncert(jet2.Pt(),dRmin2);
    // fill histos after shifting the jets up and down in unison
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 9); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 10); // down
    
    // Flavour
    sys1 = jesProvider->getRelFlavorUncert(jet1.Pt(),jet1.Eta(),1);
    sys2 = jesProvider->getRelFlavorUncert(jet2.Pt(),jet2.Eta(),1);
    // this uncertainty does not apply to b jets!
    if (is_bb_or_cc) { sys1=0; sys2=0; }
    // fill histos after shifting the jets up and down in unison
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 11); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 12); // down

    // b-quark JES uncertainty
    sys1=0; sys2=0; // only apply to b-jets
    if (is_bb_or_cc) { // For 10% of the cases, both jets are b-tagged
      sys1 = jesProvider->getRelBJESUncert(jet1.Pt(),jet1.Eta());
      sys2 = jesProvider->getRelBJESUncert(jet2.Pt(),jet2.Eta());
    }
    FillHistos( jet1*(1.0+sys1), jet2*(1.0+sys2), 13); // up
    FillHistos( jet1*(1.0-sys1), jet2*(1.0-sys2), 14); // down
  }

  for (int si=0;si<sysNames.size();++si) {
    int upIndex = 2*si+1, downIndex = 2*si+2;
    c->Clear();
    c->Divide(2,2);
    c->cd(1); DrawComparison(pt1,upIndex,downIndex,sysNames[si]);
    c->cd(2); DrawComparison(pt2,upIndex,downIndex,sysNames[si]);
    c->cd(3); DrawComparison(eta1,upIndex,downIndex,sysNames[si]);
    c->cd(4); DrawComparison(eta2,upIndex,downIndex,sysNames[si]);
    c->Print(psName);
    
    c->Clear();
    c->Divide(2,2);
    c->cd(1); DrawComparison(delta_y,upIndex,downIndex,sysNames[si]);
    c->cd(2); DrawComparison(mjj,upIndex,downIndex,sysNames[si]);
    //c->cd(2); deltaPhi[si]->Draw();
    c->cd(3); DrawComparison(y_boost,upIndex,downIndex,sysNames[si]);
    //c->cd(4); DrawComparison(y_star,upIndex,downIndex,sysNames[si]);
    c->cd(4); DrawComparison(deltaR,upIndex,downIndex,sysNames[si]);
    c->Print(psName);
  }

  c->Print(psName+"]");
  
  // Convert the ps file to a pdf file
  int stat = gSystem->Exec(Form("ps2pdf %s",psName.Data()));
  if (stat==0) {
    gSystem->Exec(Form("rm %s",psName.Data()));
    printf("\nProduced\n  %s\n",psName.ReplaceAll(".ps",".pdf").Data());
  }
}

void FillHistos(TLorentzVector jetA, TLorentzVector jetB, int histIndex) {
  // apply cuts
  if (jetA.Pt()<20||jetB.Pt()<20||fabs(jetA.Eta())>4.5||jetB.Eta()>4.5) return;
  static TLorentzVector jet1, jet2;
  if (jetA.Pt()>jetB.Pt()) { jet1=jetA; jet2=jetB; } else { jet1=jetB; jet2=jetA; }

  pt1[histIndex]->Fill(jet1.Pt());
  pt2[histIndex]->Fill(jet2.Pt());
  eta1[histIndex]->Fill(jet1.Eta());
  eta2[histIndex]->Fill(jet2.Eta());
  mjj[histIndex]->Fill((jet1+jet2).M());
  delta_y[histIndex]->Fill(fabs(jet1.Eta()-jet2.Eta()));
  deltaR[histIndex]->Fill(jet1.DeltaR(jet2));
  deltaPhi[histIndex]->Fill(fabs(jet1.DeltaPhi(jet2)));
  y_boost[histIndex]->Fill((jet1.Rapidity()+jet2.Rapidity())/2);
  y_star[histIndex]->Fill(fabs(jet1.Rapidity()-jet2.Rapidity())/2);
}

// Divide two histograms, either with uncorrelated events: corrlated = false
// or with (highly) correlated events: correlated = true (very approximate errors)
TH1D *DivideHistos(TH1D *num, TH1D *den, bool correlated, Str ytit="ratio") {

  std::vector<double> bins, ratios, ratio_errs;
  bins.push_back(num->GetXaxis()->GetBinLowEdge(1));
  bool zeroIntegral=true;
  double a=0, b=0, da=0, db=0, r=0, dr=0; // a and b are bin-yields of num and den respectively
  for (int i=1;i<=num->GetNbinsX();++i) {
    if (zeroIntegral&&i>1) { 
      bins.push_back(num->GetXaxis()->GetBinLowEdge(i)); ratios.push_back(1); ratio_errs.push_back(0);
    } 
    a += num->GetBinContent(i); b+= den->GetBinContent(i);
    double dai = num->GetBinError(i); double dbi = den->GetBinError(i);
    da = sqrt(da*da+dai*dai); db = sqrt(db*db+dbi*dbi);
    if (b==0) continue;
    zeroIntegral=false;
    //if (b==0) { ratio->SetBinContent(i,0); ratio->SetBinError(i,0); continue; }
    
    // r = a/b
    // Uncorrelated: num and den are different samples
    //   dr/r = da/a (+) db/b
    // Partly correlated: num and den are same sample, but bin migration cause
    // a and b to be partly decorrelated:
    //   dr/r = da/a (-) db/b
    // might be better to scale this up by 1/sqrt(2) to account both for migrations 
    // in and out (?). To obtain the proper uncertainties require full migration matrix ...
    r=a/b; double err1=r*da/a, err2=r*db/b;
    dr = sqrt( pow(r*err1,2) + pow(r*err2,2) ); //*sqrt(2);
    if (correlated) {
      if (err1>err2)
	dr = sqrt( pow(r*err1,2) - pow(r*err2,2) );
      else 
	dr = sqrt( pow(r*err2,2) - pow(r*err1,2) );
    }
    //if (dr>0.05) continue;
    if (a*a/da/da < 100 || b*b/db/db < 100 ) continue;
    // we have enough stats to create a ratio bin!
    ratios.push_back(r); ratio_errs.push_back(dr);
    //ratio->SetBinContent(i,r); ratio->SetBinError(i,dr);
    bins.push_back(num->GetXaxis()->GetBinLowEdge(i+1)); // upper edge
    int entry=ratios.size()-1;
    /*
    printf("%3d: %5.1f - %5.1f : %.4f +/- %.4f, ab %.2f %.2f\n",
	   entry,bins[entry],bins[entry+1],ratios[entry],ratio_errs[entry],a,b);
    printf("%.2f %.2f\n",bins[entry],bins[entry+1]);
    */
    a=0; b=0; da=0; db=0; r=0; dr=0;// reset vars
  }
  if (num->GetBinLowEdge(num->GetNbinsX()+1)>bins.back()) {
    bins.push_back(num->GetBinLowEdge(num->GetNbinsX()+1));
    ratios.push_back(r); ratio_errs.push_back(dr);
    if (r==0) { ratios.push_back(1); ratio_errs.push_back(0); }
  }

  static int rati=0;
  TH1D *ratio = new TH1D(Form("ratio_%d",++rati),"",bins.size()-1,&bins[0]);
  for (int i=0;i<ratios.size();++i) {
    ratio->SetBinContent(i+1,ratios[i]); ratio->SetBinError(i+1,ratio_errs[i]);
  }
  ratio->SetYTitle(ytit); ratio->SetXTitle(num->GetXaxis()->GetTitle());
  if (ytit=="ratio"&&correlated) ratio->SetYTitle("ratio (approx. errors)");
  ratio->SetLineColor(num->GetLineColor()); 
  ratio->SetMarkerStyle(num->GetMarkerStyle()); ratio->SetMarkerColor(num->GetMarkerColor());
  return ratio;
}

TH1D *DivideCorrelatedHistos(TH1D *num, TH1D *den, Str ytit="ratio") {
  return DivideHistos(num,den,true,ytit);
}

void DrawComparison(const HistV &histos, int up, int down, Str sysName) {
  histos[0]->SetLineColor(kBlack); histos[up]->SetLineColor(kRed); histos[down]->SetLineColor(kBlue);
  histos[0]->GetXaxis()->SetLabelOffset(10); histos[0]->GetXaxis()->SetTitleOffset(10);
  histos[0]->Draw("e"); histos[up]->Draw("same e"); histos[down]->Draw("same e");
  static TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04); tex->SetTextFont(42);
  tex->SetTextColor(kBlack); tex->DrawLatex(0.7,0.92,"Nominal");
  tex->SetTextColor(kRed); tex->DrawLatex(0.7,0.87,sysName+" up");
  tex->SetTextColor(kBlue); tex->DrawLatex(0.7,0.82,sysName+" down");
  
  TH1D *ratioUp   = DivideCorrelatedHistos(histos[up],histos[0]);
  TH1D *ratioDown = DivideCorrelatedHistos(histos[down],histos[0]);

  gPad->SetBottomMargin(0.36);
  // create new pad, fullsize to have equal font-sizes in both plots
  TPad *p = new TPad( "p_test", "", 0, 0, 1, 1.0 - 0.06, 0, 0, 0.0);
  p->SetTopMargin(1.0 - 0.36); p->SetFillStyle(0);
  //p->SetMargin(0.12,0.04,0.14,1.0 - 0.36); 
  p->Draw(); p->cd(); //p->SetGridy(kTRUE);

  ratioUp->SetMinimum(0.75); ratioUp->SetMaximum(1.25); ratioUp->GetYaxis()->SetNdivisions(405);
  ratioUp->Draw("e"); ratioDown->Draw("same e");
  //ratioUp->Draw("same histl"); ratioDown->Draw("same histl");
}

