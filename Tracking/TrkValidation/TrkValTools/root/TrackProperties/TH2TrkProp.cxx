// $:$Id: TH2TrkProp.cxx, v 1.0  04/17/2006 17:07:07 
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
#include "TH2.h"

#include "TH2TrkProp.h"


TH2TrkProp::TH2TrkProp(const char* format, const char* rootFile, const char* dir, const char* ntuple,
		       const char* xvariable, const char* xvariableName, const char* xunit,
		       const char* yvariable, const char* yvariableName, const char* yunit,
		       const char* cut, const char* cutName, const char* title, 
		       int nbinsx, double xlow, double xup,
		       int nbinsy, double ylow, double yup,
		       bool logx, bool logy
		       ) {

  gROOT->Reset();
  
  //new style
  setStyle();

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
  char* strVarYUnit = new char[200];
  char* strHist = new char[200];
  char* strOutput = new char[200];
  char* strHisto = new char[200];

  //concatenate strings
  strcat(strcat(strcat(strcpy(strTitle,title)," ("),cutName),")");
  strcat(strcat(strcpy(strVarXUnit,xvariableName)," "),xunit);
  strcat(strcat(strcpy(strVarYUnit,yvariableName)," "),yunit);
  strcat(strcat(strcat(strcpy(strHist,yvariable),":"),xvariable)," >> histo");
  strcat(strcat(strcat(strcat(strcpy(strOutput,title),"_"),cutName),"."),format);
  strcat(strcat(strcpy(strHisto,title),"_"),cutName);

  //book 2D-histo from ntuple(TTree)
  TH2F* histo = new TH2F("histo",strTitle,nbinsx,xlow,xup,nbinsy,ylow,yup);

  histo->GetXaxis()->SetTitle(strVarXUnit);
  histo->GetYaxis()->SetTitle(strVarYUnit);


  //create new canvas
  TCanvas* canvas = new TCanvas("canvas",title,700,600);
  canvas->cd();

  if (logx) {
    canvas->SetLogx();
  }
  if (logy) {
    canvas->SetLogy();
  }

  //fill 2D-histo
  tree->Draw(strHist,cut,"colz");

  //clone 1D-histo 
  TH2F* hnew = (TH2F*)histo->Clone();
  //hnew->SetName(strHisto);


  //save 2D-histo to file.format
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
TH2TrkProp::~TH2TrkProp() {

  cout<<"deleting TH2TrkProp object"<<endl;

}



//____________________________________________________________________________________

void TH2TrkProp::setStyle() {

  TStyle* style2D = new TStyle("style2D","Style for 2D-Histograms");
  style2D->cd();
  //pad 
  style2D->SetPadBorderMode(0);
  style2D->SetPadColor(0);
  style2D->SetPadTopMargin(0.21);
  style2D->SetPadBottomMargin(0.12);
  style2D->SetPadLeftMargin(0.12);
  style2D->SetPadRightMargin(0.16);
  //canvas
  style2D->SetCanvasColor(0);
  style2D->SetCanvasBorderMode(0);
  //frame
  style2D->SetFrameBorderMode(0);
  //statistics
  style2D->SetOptStat(1001110);
  style2D->SetStatBorderSize(3);
  style2D->SetStatColor(0);
  style2D->SetStatFont(62);
  style2D->SetStatFontSize(0.033);
  style2D->SetStatTextColor(1);
  style2D->SetStatStyle(1001);
  style2D->SetStatX(0.993);
  style2D->SetStatY(0.995);
  style2D->SetStatW(0.27);
  //histograms
  style2D->SetMarkerColor(1);
  style2D->SetMarkerStyle(6);
  //title
  style2D->SetTitleX(0.010);
  style2D->SetTitleY(0.93);
  style2D->SetTitleW(0.7);
  style2D->SetTitleBorderSize(3);
  style2D->SetTitleFont(72,"TITLE");
  //axes
  style2D->SetLabelFont(62,"X");
  style2D->SetLabelFont(62,"Y");
  style2D->SetTitleOffset(1.4,"Y");
  //palette
  style2D->SetPalette(1,0);

}






  
