#########################################################
#
#  Just an example macro for analysis plots.
#  Tested on MC samples with protons generated in I.P.
#  
#  run with:
#  > root -l GloRecAnalysis.root glorecplots.C+
#  
#  In general: histograms, ALFA_GloRec::TruthInfo and 
#  ALFA_GloRec::FillTrackCandHistos need to be edited
#  by the user to produce sensible results.
#
#########################################################

#include <iostream>
#include <stdlib.h>
#include <cstdlib> 
#include <stdio.h>
#include <string>
#include <fstream>
#include <ctype.h>


#include "TFile.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TApplication.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include "TClass.h"
#include "TH2.h"
#include "TDirectory.h" 
#include "TChain.h"
#include "TText.h"
#include "TLatex.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "TMath.h"
#include "TStyle.h"
#include "TLegend.h"

using namespace std;

#include "../../../../MyCode/myclasses.h"
#define SIZE(a) (sizeof(a)/sizeof(a[0]))

void glorecplots(){


  ////////////////////
  //  set style
  ////////////////////
  gROOT  -> SetStyle("Plain");
  gStyle -> SetMarkerStyle(20);
  gStyle -> SetMarkerSize(0.7);
  gStyle -> SetTextSize(1);
  gStyle -> SetOptStat("emr");
  gStyle -> SetStatW(0.4);
  gStyle -> SetStatH(0.2);
  gStyle -> SetStatStyle(0);
  gStyle -> SetStatBorderSize(0);
  gStyle -> SetStatTextColor(kOrange+9);
  gStyle -> SetLineWidth(1.); // 2
  gStyle -> SetHistLineWidth(2.);// 2
  gStyle -> SetPadLeftMargin(0.25);
  gStyle -> SetPadRightMargin(0.019);
  gStyle -> SetPadBottomMargin(0.25);
  gStyle -> SetPadTopMargin(0.005);
  gStyle -> SetEndErrorSize(5);
  gStyle -> SetCanvasBorderSize(2);
  //  gStyle -> SetOptFit(kTRUE);
  gROOT  -> ForceStyle();



  /////////////////////////////////
  // Some tmp variables
  /////////////////////////////////
  TFile * frtmp = 0;
  TString strtmp("");


  /////////////////
  //  Open files 
  /////////////////
  TSeqCollection * filelist = gROOT->GetListOfFiles();
  Int_t nfiles = filelist->GetEntries();
  if(nfiles==0){
    cout << "No files were given" << nfiles << endl;
    exit(0);
  }else if(nfiles >1){
    cout << "Only one file is allowed: n = " << nfiles << endl;
    exit(0);
  }
  
  TFile ** fr;
  fr = new TFile*[nfiles];  
  for(Int_t fi = 0; fi < nfiles; fi++){
    fr[fi] = (TFile*) filelist -> At(fi);
    cout << "File " << fr[fi] -> GetName() << " connected as number " << fi << endl;
  }

  if(nfiles > 1) {
    cout << "Only one file is allowed" <<  endl;
    exit(0);
  }
  
  

  ////////////////////////////////////////////
  //  
  //  Open oputput file
  //
  ////////////////////////////////////////////
  //TFile * frout = new TFile("AccPurStab.root","RECREATE");


  //////////////////////////////////////////
  //   Specify histogram names to plot etc.
  //////////////////////////////////////////
  const Int_t maxnh = 100;
  HistoAtt * hatt1[maxnh];
  HistoAtt * hatt2[maxnh];
  HistoAtt * hatt3[maxnh];
  for(Int_t i = 0; i < maxnh; i++) hatt1[i] = 0;
  for(Int_t i = 0; i < maxnh; i++) hatt2[i] = 0;
  for(Int_t i = 0; i < maxnh; i++) hatt3[i] = 0;
  Int_t ind = 0;  
  Int_t can = 0;
  Int_t pad = 1;

  can = 0;
  pad = 1;

  hatt1[ind] = new HistoAtt(fr[0],"","truexvsrecx","x_{gen vertex} [mm]","x_{rec}^{glob track} [mm]", can, 1,"lin",2);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","trueyvsrecy","y_{gen vertex} [mm]","y_{rec}^{glob track} [mm]", can, 4,"lin",2);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","truexslopevsrecxslope","x'_{gen vertex}","x'_{rec}^{glob track}", can, 7,"lin",2);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","trueyslopevsrecyslope","y'_{gen vertex}","y'_{rec}^{glob track}", can, 10,"lin",2);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recxovertruex","x_{rec}^{glob track}/x_{gen vertex}","N", can, 2,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recyovertruey","y_{rec}^{glob track}/y_{gen vertex}","N", can, 5,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recxslopeovertruexslope","x'_{rec}^{ glob track}/x'_{gen vertex}","N", can, 8,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recyslopeovertrueyslope","y'_{rec}^{ glob track}/y'_{gen vertex}","N", can, 11,"lin",1);
  ind++;
  pad++;


  hatt1[ind] = new HistoAtt(fr[0],"","recxminustruex","x_{rec}^{glob track}-x_{gen vertex} [mm]","N", can, 3,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recyminustruey","y_{rec}^{glob track}-y_{gen vertex} [mm]","N", can, 6,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recxslopeminustruexslope","x'_{rec}^{ glob track}-x'_{gen vertex} [mm]","N", can, 9,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","recyslopeminustrueyslope","y'_{rec}^{ glob track}-y'_{gen vertex} [mm]","N", can, 12,"lin",1);
  ind++;
  pad++;



  can = 1;
  hatt1[ind] = new HistoAtt(fr[0],"","xnearuppotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 1,"lin",1);
  hatt2[ind] = new HistoAtt(fr[0],"","ynearuppotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 1,"lin",1);
  ind++;
  pad++;
  
  hatt1[ind] = new HistoAtt(fr[0],"","xfaruppotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 2,"lin",1);
  hatt2[ind] = new HistoAtt(fr[0],"","yfaruppotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 2,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","xnearlwpotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 3,"lin",1);
  hatt2[ind] = new HistoAtt(fr[0],"","ynearlwpotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 3,"lin",1);
  ind++;
  pad++;

  hatt1[ind] = new HistoAtt(fr[0],"","xfarlwpotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 4,"lin",1);
  hatt2[ind] = new HistoAtt(fr[0],"","yfarlwpotresiduals","coord_{extrapol} - coord_{LocRec} [mm]","N", can, 4,"lin",1);
  ind++;
  pad++;


  can = 2;
  pad = 1; 
  hatt1[ind] = new HistoAtt(fr[0],"","extrapxvsrecxnearpot","x_{gen}^{expect} [mm]","x_{loc rec}^{near pot} [mm]", can, pad++,"lin",2);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","recxminusextrapxnearpot","x_{loc rec}^{near pot} - x_{gen}^{expect} [mm]","N", can, pad++,"lin",1);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","extrapyvsrecynearpot","y_{gen}^{expect} [mm]","y_{loc rec}^{near pot} [mm]", can, pad++,"lin",2);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","recyminusextrapynearpot","y_{loc rec}^{near pot} - y_{gen}^{expect} [mm]","N", can, pad++, "lin",1);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","extrapxvsrecxfarpot","x_{gen}^{expect} [mm]","x_{loc rec}^{far pot} [mm]", can, pad++,"lin",2);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","recxminusextrapxfarpot","x_{loc rec}^{far pot} - x_{gen}^{expect} [mm]","N", can, pad++,"lin",1);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","extrapyvsrecyfarpot","y_{gen}^{expect} [mm]","y_{loc rec}^{far pot} [mm]", can, pad++,"lin",2);
  ind++;

  hatt1[ind] = new HistoAtt(fr[0],"","recyminusextrapyfarpot","y_{loc rec}^{far pot} - y_{gen}^{expect} [mm]","N", can, pad++, "lin",1);
  ind++;




  Int_t nh = 0;
  for(Int_t i = 0; i < maxnh; i++) if(hatt1[i]) nh++;
  cout << "There are " << nh << " histograms " << endl;


  ///////////////////////////////////////////
  //   Create canvases
  ///////////////////////////////////////////
  Int_t ncols [] = {3,2,4};
  Int_t nrows [] = {4,2,2};
  Int_t ncanvases = SIZE(ncols);
  TCanvas * canvas[ncanvases];
  Double_t canunit = 250;
  for(Int_t ci = 0; ci < ncanvases; ci++){
    strtmp  = "c"; 
    strtmp +=  ci;
    canvas[ci] = new TCanvas(strtmp.Data(), strtmp.Data(), canunit * ncols[ci], canunit * nrows[ci]);
    canvas[ci] -> Divide(ncols[ci], nrows[ci]);
    canvas[ci] -> Draw();
  }
  TVirtualPad * currpad;

  /////////////////////////////
  //  Loop over histos
  /////////////////////////////
  Double_t totalstability = 0.;
  for(Int_t i = 0; i< nh; i++){
    /////////////////////////////////////
    //     load histograms
    /////////////////////////////////////
    TH1F * h1 = 0;
    TH2F * h2 = 0;

    TH1F * h1_2 = 0;
    TH2F * h2_2 = 0;

    frtmp =  hatt1[i]->GetFile();
    frtmp -> cd(hatt1[i]->GetDir());
    strtmp = hatt1[i]->GetName();
    cout << "reading histo: " << strtmp.Data() << " from " << frtmp->GetName() << "/" << hatt1[i]->GetDir() << endl; 
    if(hatt1[i]->GetToggle() == 1){
      h1 = (TH1F*) gDirectory->Get(strtmp.Data())->Clone();
    }else if(hatt1[i]->GetToggle() == 2) {
      h2 = (TH2F*) gDirectory->Get(strtmp.Data())->Clone();
    }

    if(hatt2[i]){
      frtmp =  hatt2[i]->GetFile();
      frtmp -> cd(hatt2[i]->GetDir());
      strtmp = hatt2[i]->GetName();
      cout << "reading histo: " << strtmp.Data() << " from " << frtmp->GetName() << "/" << hatt2[i]->GetDir() << endl; 
      if(hatt2[i]->GetToggle() == 1){
        h1_2 = (TH1F*) gDirectory->Get(strtmp.Data())->Clone();
      }else if(hatt1[i]->GetToggle() == 2) {
        h2_2 = (TH2F*) gDirectory->Get(strtmp.Data())->Clone();
      }
    }


    //////////////////////////////////////
    //  draw histos
    //////////////////////////////////////
    Double_t hismax = 0.;
    currpad = canvas[hatt1[i]->GetCanNum()] -> cd(hatt1[i]->GetPadNum());
    if(h1){
      hismax = h1->GetBinContent(h1->GetMaximumBin());
    }
    if(hatt1[i]->GetYscale().Contains("logy") && !hatt1[i]->GetYscale().Contains("logxy")) {
      currpad -> SetLogy();
    }else if(hatt1[i]->GetYscale().Contains("logx") && !hatt1[i]->GetYscale().Contains("logxy")) {
      currpad -> SetLogx(1);
    }else if(hatt1[i]->GetYscale().Contains("logxy") || hatt1[i]->GetYscale().Contains("logyx")) {
      currpad -> SetLogy();
      currpad -> SetLogx(1);
    }

    if(h1){
      h1 -> SetMaximum(hismax * 1.6);
      h1 -> SetMinimum(0.01);
      h1 -> SetTitle("");
      h1 -> GetXaxis() -> SetTitleSize(0.06);
      h1 -> GetYaxis() -> SetTitleSize(0.06);
      h1 -> GetXaxis() -> SetTitleOffset(1.1);
      h1 -> GetYaxis() -> SetTitleOffset(1.3);
      h1 -> GetXaxis() -> SetLabelSize(0.05);
      h1 -> GetYaxis() -> SetLabelSize(0.05);
      h1 -> GetXaxis() -> SetLabelOffset(0.01);
      h1 -> GetYaxis() -> SetLabelOffset(0.01);
      h1 -> GetXaxis() -> SetTitle(hatt1[i]->GetXaxisName());
      h1 -> GetYaxis() -> SetTitle(hatt1[i]->GetYaxisName());
      h1 -> GetXaxis() -> SetNdivisions(505);
      h1 -> SetLineWidth(1.);
      h1 -> SetLineStyle(kSolid);
      h1 -> SetLineColor(kBlack);
      h1 -> SetFillColor(kYellow);
      h1 -> SetFillStyle(1001);
      h1 -> SetMarkerSize(0.);
      h1 -> DrawCopy("");
      if(h1_2){
        h1_2 -> SetLineWidth(1.4);
        h1_2 -> SetLineStyle(kDashed);
        h1_2 -> SetLineColor(kRed);
        h1_2 -> SetMarkerSize(0.);
        h1_2 -> DrawCopy("same");
      }
      h1-> DrawCopy("same axis");
    } else if (h2){
      //h2 -> SetMaximum(1.49);//hismax * 1.2);
      //h2 -> SetMinimum(0.01);
      h2 -> SetTitle("");
      h2 -> GetXaxis() -> SetTitleSize(0.06);
      h2 -> GetYaxis() -> SetTitleSize(0.06);
      h2 -> GetXaxis() -> SetTitleOffset(1.1);
      h2 -> GetYaxis() -> SetTitleOffset(1.3);
      h2 -> GetXaxis() -> SetLabelSize(0.05);
      h2 -> GetYaxis() -> SetLabelSize(0.05);
      h2 -> GetXaxis() -> SetLabelOffset(0.01);
      h2 -> GetYaxis() -> SetLabelOffset(0.01);
      h2 -> GetXaxis() -> SetTitle(hatt1[i]->GetXaxisName());
      h2 -> GetYaxis() -> SetTitle(hatt1[i]->GetYaxisName());
      h2 -> GetXaxis() -> SetNdivisions(505);
      h2 -> SetLineWidth(1.2);
      h2 -> SetLineColor(kBlack);
      h2 -> SetMarkerColor(kBlack);
      h2 -> SetMarkerSize(0.15);
      h2 -> DrawCopy("");
      if(h2_2){
        h2_2 -> SetLineColor(kRed);
        h2_2 -> SetMarkerColor(kRed);
        h2_2 -> SetMarkerSize(0.15);
        h2_2 -> DrawCopy("same");
      }

    }


    /////////////////////////////////////////////////////
    //
    //  Put a legend and fit stats. to sigbgd chi2 fit
    //
    /////////////////////////////////////////////////////
    // if(h1){    
    // TLegend leg(0.7, 0.75, 0.95, 0.95);
    //   leg.SetBorderSize(0);
    //   leg.SetFillStyle(0);
    //   leg.SetFillColor(kWhite);
    //   leg.AddEntry(h1, "x coordinate", "L");
    //   if(h1_2) leg.AddEntry(h1_2,"y_coordinate", "L");      
    //   leg.DrawClone("same");    
    // }

  }

  for(Int_t ci = 0; ci < ncanvases; ci++){
    strtmp = canvas[ci]->GetName();
    strtmp += ".pdf";
    canvas[ci] -> SaveAs(strtmp.Data());
  }
}
