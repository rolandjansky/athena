// $:$Id: TH1TrkProp.cxx, v 1.0  04/17/2006 12:07:07 
// Author: Miguel Angel Olivo Gomez   04/17/06

/*************************************************************************
 *                                                                       *
 * Copyright (C) 2006                                    .               *
 * All rights reserved.                                                  *
 *                                                                       *
 *************************************************************************/

#include <iostream>
#include <string.h>
#include "TStyle.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"

#include "TH1TrkProp.h"


TH1TrkProp::TH1TrkProp(const char* format, const char* rootFile, const char* dir, const char* ntuple, 
		       const char* xvariable, const char* xvariableName, const char* xunit, 
		       const char* cut, const char* cutName, const char* title, int nbinsx, 
		       double xlow, double xup, bool logy, bool fitGauss
		       ) {

  gROOT->Reset();
  
  //new style
  setStyle(fitGauss);

  //read rootFile
  TFile* f = new TFile(rootFile,"read");
  if (f->IsZombie()) {
    cout<<"Error opening file"<<endl;
    exit(-1);
  } else {


  //get directory
  TDirectory* directory = (TDirectory*)f->Get(dir);

  //get ntuple (TTree)
  TTree* tree = (TTree*)directory->Get(ntuple);

  //allocate the necessary memory for string concatenation
  char* strTitle = new char[200];
  char* strVarXUnit = new char[200];
  char* strHist = new char[200];
  char* strOutput = new char[200];
  char* strHisto = new char[200];

  //concatenate strings
  strcat(strcat(strcat(strcpy(strTitle,title)," ("),cutName),")");
  strcat(strcat(strcpy(strVarXUnit,xvariableName)," "),xunit);
  strcat(strcpy(strHist,xvariable)," >> histo");
  strcat(strcat(strcat(strcat(strcpy(strOutput,title),"_"),cutName),"."),format);
  strcat(strcat(strcpy(strHisto,title),"_"),cutName);

  //book 1D histo from ntuple(TTree)
  TH1F* histo = new TH1F("histo",strTitle,nbinsx,xlow,xup);

  histo->GetXaxis()->SetTitle(strVarXUnit);
  histo->GetYaxis()->SetTitle("Entries");


  //create new canvas
  TCanvas* canvas = new TCanvas("canvas",title,700,600);
  canvas->cd();

  if (logy) {
    canvas->SetLogy();
  }

  //fill 1D histo
  tree->Draw(strHist,cut,"E");

  //clone 1D histo 
  TH1F* hnew = (TH1F*)histo->Clone();
  //hnew->SetName(strHisto);
  //hnew->Sumw2();

  if (fitGauss){
    histo->Fit("gaus");
  }

  //save 1D-histo to file.format
  canvas->Print(strOutput);

  //store histograms in root file
  TFile *histTrkProp = new TFile("histTrkProp.root","update");
  histTrkProp->cd();
  hnew->Write(strHisto);
  histTrkProp->Close();

  //close rootFile
  f->Close();
  
  }

}



//___________________________________________________________________________________
TH1TrkProp::~TH1TrkProp() {

  cout<<"deleting TH1TrkProp object"<<endl;

}



//____________________________________________________________________________________

void TH1TrkProp::setStyle(bool fitGauss) {
  TStyle* style1D = new TStyle("style1D","Style for 1D-Histograms");
  style1D->cd();
  //pad 
  style1D->SetPadBorderMode(0);
  style1D->SetPadColor(0);
  style1D->SetPadTopMargin(0.21);
  style1D->SetPadBottomMargin(0.12);
  style1D->SetPadLeftMargin(0.12);
  style1D->SetPadRightMargin(0.07);
  //canvas
  style1D->SetCanvasColor(0);
  style1D->SetCanvasBorderMode(0);
  //frame
  style1D->SetFrameBorderMode(0);
  //statistics
  style1D->SetOptStat(1111110);
  style1D->SetStatBorderSize(3);
  style1D->SetStatColor(0);
  style1D->SetStatFont(62);
  style1D->SetStatFontSize(0.033);
  style1D->SetStatTextColor(1);
  style1D->SetStatStyle(1001);
  style1D->SetStatX(0.993);
  style1D->SetStatY(0.995);
  style1D->SetStatW(0.27);
  //histograms
  style1D->SetMarkerColor(1);
  style1D->SetMarkerStyle(20);
  style1D->SetMarkerSize(0.8);
  //title
  style1D->SetTitleX(0.010);
  style1D->SetTitleY(0.93);
  style1D->SetTitleW(0.7);
  style1D->SetTitleBorderSize(3);
  style1D->SetTitleFont(72,"TITLE");
  //axes
  style1D->SetLabelFont(62,"X");
  style1D->SetLabelFont(62,"Y");
  style1D->SetTitleOffset(1.3,"Y");
  //fitGauss
  if (fitGauss){
    style1D->SetStatFontSize(0.03);
    style1D->SetStatW(0.2);
    style1D->SetTitleW(0.6);
    style1D->SetOptFit(1111);
  }

}




  
