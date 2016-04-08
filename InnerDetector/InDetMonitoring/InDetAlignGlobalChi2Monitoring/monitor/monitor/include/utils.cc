/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TH2F.h"
#include "TPaletteAxis.h"
#include "TLatex.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TGraph.h"
#include "TProfile.h"
#include "TF1.h"

// void utils();
// void createSymLinks();
void cleanSymLinks();
void doSquare(Char_t name[MaxPathSize]);

//=====================================================================
//  utils()
//=====================================================================
void utils() {
  cout << " -> utils loaded!" << endl;
}


//=====================================================================
// checkDir()
//=====================================================================
Long_t checkDir(Char_t dirName[MaxPathSize]) {
  
  Long_t id, size, flag, modtime;
  Int_t status = gSystem->GetPathInfo(dirName,&id,&size,&flag,&modtime);
  
  if (PrintLevel<1) {
    cout << "checking dir: " << dirName << endl;
    cout << " * status: " << status << endl;
    cout << " * id: " << id << endl;
    cout << " * size: " << size << " " << endl;
    cout << " * flag: " << flag << endl;
    cout << " * modtime: " << modtime << endl;
  }

  return status;
}


//=====================================================================
// isDir()
//=====================================================================
Long_t isDir(Char_t dirName[MaxPathSize]) {
  
  Long_t id, size, flag, modtime;
  Int_t status = gSystem->GetPathInfo(dirName,&id,&size,&flag,&modtime);
  
  if (PrintLevel<1) {
    cout << "checking isDir: " << dirName << endl;
    cout << " * status: " << status << endl;
    cout << " * id: " << id << endl;
    cout << " * size: " << size << " " << endl;
    cout << " * flag: " << flag << endl;
    cout << " * modtime: " << modtime << endl;
  }

  return flag;
}


//=====================================================================
// checkFile()
//=====================================================================
Long_t checkFile(Char_t fileName[MaxPathSize]) {

  Long_t id, size, flag, modtime;
  Int_t status = gSystem->GetPathInfo(fileName,&id,&size,&flag,&modtime);

  if (PrintLevel<1) {
    cout << "checking file: " << fileName << endl;
    cout << " * status: " << status << endl;
    cout << " * id: " << id << endl;
    cout << " * size: " << size << " " << endl;
    cout << " * flag: " << flag << endl;
    cout << " * modtime: " << modtime << endl;
  }

  return status;
}


//=====================================================================
// createSymLinks()
//=====================================================================
void createSymLinks() {

  cleanSymLinks();

  // preparing plots
  gROOT->ProcessLine(".!ln -s ../trkana/trkana.h .");
  gROOT->ProcessLine(".!ln -s ../trkana/ResFit.C .");
  gROOT->ProcessLine(".!ln -s ../trkana/ResFit.h .");

  return;
}


//=====================================================================
// SetZAxisTitle()
//=====================================================================
void SetZAxisTitle(TH2F *histo2D, Char_t fileName[MaxPathSize],
		   Float_t X1=0.82, Float_t X2=0.89,
		   Float_t Y1=0.1, Float_t Y2=0.87) {

  // moving the color scale
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)histo2D->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(X1);
  palette->SetX2NDC(X2);
  palette->SetY1NDC(Y1);
  palette->SetY2NDC(Y2);
  gPad->Modified();

  TLatex *t = new TLatex();
  t->SetNDC();
  t->SetTextFont(font);
  t->SetTextSize(0.041);
  sprintf(name,"res (#mum)");
  t->DrawLatex(X1-0.01,0.90,name);

  return;
}


//=====================================================================
// cleanSymLinks()
//=====================================================================
void cleanSymLinks() {

  if (checkFile("trkana.h")==0) {

    // cleaning 
    gROOT->ProcessLine(".!rm trkana.h");
    gROOT->ProcessLine(".!rm ResFit.C");
    gROOT->ProcessLine(".!rm ResFit.h");
  }

  return;
}


//=====================================================================
//  setTitleFont()
//=====================================================================
void setTitleFont(Int_t font=62) {
  if (!AtlasStyle) {
    gPad->Update();
    TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
    pt->SetTextFont(font);
    pt->SetTextSize(0.04);
    gPad->Modified();
  }
}


//=====================================================================
//  ScalePlots()
//=====================================================================
void ScalePlots(Int_t position=0) {
  if (normalizePlots) {
    TLatex *t2 = new TLatex();
    t2->SetNDC();
    t2->SetTextFont(font);
    t2->SetTextSize(0.08);
    t2->SetTextAlign(12);
    t2->SetTextSize(0.035);
    t2->SetTextColor(kRed);
    sprintf(name,"Normalized plots");
    if(!AtlasStyle){
      if (position==0) {
	t2->SetTextAngle(270);
	t2->DrawLatex(0.93,0.37,name);
      }
      else
	t2->DrawLatex(0.3,0.91,name);
    }
    else {
      if (position==0) {
	t2->SetTextAngle(270);
	t2->DrawLatex(0.97,0.47,name);
      }
      else
	t2->DrawLatex(0.3,0.91,name);
    }
  }
}


//=====================================================================
//  setTitleSize()
//=====================================================================
void setTitleSize(double x2Size = 0.90) {
  if (!AtlasStyle) {
    gPad->Update();
    TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
    pt->SetX2NDC(x2Size);
    gPad->Modified();
  }
}


//=====================================================================
//  moveTPaveStats()
//=====================================================================
void moveTPaveStats(TH2F* histoIn2D) {
  if (!AtlasStyle) {
    TPaveStats *st1 = (TPaveStats*)histoIn2D->FindObject("stats"); 
    st1->SetX1NDC(0.56); //new x start position
    st1->SetX2NDC(0.89); //new x start position
    st1->SetY1NDC(0.80); //new y end position
    st1->SetOptStat(1110);
    st1->Draw();
  }
}


//=====================================================================
//  redrawTPaveStats()
//=====================================================================
void redrawTPaveStats(TH2F* histoIn2D) {
  if (!AtlasStyle) {   
    gPad->Update();

    Char_t name[MaxPathSize];
    int entries = (int)histoIn2D->GetEntries();
    float mean1 = histoIn2D->GetMean(1);
    float rms1 = histoIn2D->GetRMS(1);
    float mean2 = histoIn2D->GetMean(2);
    float rms2 = histoIn2D->GetRMS(2);

    TPaveStats *ptstats = new TPaveStats(0.68,0.82,0.98,0.995,"brNDC");
    ptstats->SetName("stats");
    ptstats->SetBorderSize(1);
    ptstats->SetTextAlign(12);
    ptstats->SetTextFont(42);
    ptstats->SetFillColor(kWhite);

    sprintf(name,"Entries = %d  ",entries);
    ptstats->AddText(name);

    sprintf(name,"Mean x = %6.4g  ",mean1);
    ptstats->AddText(name);

    sprintf(name,"Mean y = %6.4g  ",mean2);
    ptstats->AddText(name);

    sprintf(name,"RMS x = %6.4g  ",rms1);
    ptstats->AddText(name);

    sprintf(name,"RMS y = %6.4g  ",rms2);
    ptstats->AddText(name);

    ptstats->SetOptStat(1000);
    ptstats->SetOptFit(0);
    ptstats->Draw();
    gPad->Modified();

  }
}


//=====================================================================
//  ForceStyle()
//=====================================================================
void ForceStyle(TH1F* histoIn1D) {
  histoIn1D->GetXaxis()->SetLabelFont(42);
  histoIn1D->GetXaxis()->SetTitleFont(42);
  histoIn1D->GetYaxis()->SetLabelFont(42);
  histoIn1D->GetYaxis()->SetTitleFont(42);
  histoIn1D->GetYaxis()->SetTitleOffset(1.5);
}


//=====================================================================
//  drawVerticalLine()
//=====================================================================
void drawVerticalLine(TH1F* histoIn1D) {
  // Draw vertical Line around x=0.0
  double y1 = histoIn1D->GetYaxis()->GetXmin();
  double y2 = 1.045*histoIn1D->GetMaximum();
  TLine *l0x = new TLine(0.0,y1,0.0,y2);
  l0x->SetLineColor(kRed);
  l0x->Draw();
  return;
}

//=====================================================================
//  drawVerticalLine()
//=====================================================================
void drawVerticalLine(TGraph* hGraph, int xCut) {
  // Draw vertical Line around y=0.0
  double y1 = hGraph->GetYaxis()->GetXmin();
  double y2 = hGraph->GetYaxis()->GetXmax();
  // printf("y1: %2.6f\n",y1);
  // printf("y2: %2.6f\n",y2);
  TLine *l0x = new TLine(xCut,y1,xCut,y2);
  l0x->SetLineColor(kRed);
  l0x->SetLineStyle(2);
  l0x->Draw();
  return;
}

//=====================================================================
//  drawHorizontalLine()
//=====================================================================
void drawHorizontalLine(TProfile* profIn2D) {
  // Draw horizontal Line around y=0.0
  double x1 = profIn2D->GetXaxis()->GetXmin();
  double x2 = profIn2D->GetXaxis()->GetXmax();
  TLine *l0x = new TLine(x1,0.0,x2,0.0);
  l0x->SetLineColor(kRed);
  l0x->SetLineStyle(2);
  l0x->Draw();
  return;
}


//=====================================================================
//  drawHorizontalLine()
//=====================================================================
void drawVerticalLine(TGraph* hGraph, Double_t x2 = 0.0, Double_t x1 = 0.0) {
  // Draw horizontal Line around y=0.0
  if (x1==0.0) x1 = hGraph->GetXaxis()->GetXmin();
  if (x2==0.0) x2 = hGraph->GetXaxis()->GetXmax();
  // printf("y1: %2.6f\n",y1);
  // printf("y2: %2.6f\n",y2);

  TLine *l0x = new TLine(x1,0.0,x2,0.0);
  l0x->SetLineColor(kRed);
  l0x->SetLineStyle(2);
  l0x->Draw();
  return;
}


//=====================================================================
//  drawCross()
//=====================================================================
void drawCross(TH2F* histoIn2D) {
  // Draw Lines
  double x1 = histoIn2D->GetXaxis()->GetXmin();
  double x2 = histoIn2D->GetXaxis()->GetXmax();
  double y1 = histoIn2D->GetYaxis()->GetXmin();
  double y2 = histoIn2D->GetYaxis()->GetXmax();
  TLine *l1 = new TLine(x1,0.0,x2,0.0);
  l1->SetLineColor(kRed);
  l1->Draw();
  TLine *l2 = new TLine(0.0,y1,0.0,y2);
  l2->SetLineColor(kRed);
  l2->Draw();
  return;
}


//=====================================================================
//  doSpace()
//=====================================================================
void doSpace(TH1F* histoIn1D, float scale=1.1) {
  double y2 = scale*histoIn1D->GetMaximum();
  histoIn1D->GetYaxis()->SetRangeUser(0,y2);
  return;
}

//=====================================================================
//  doSquare()
//=====================================================================
void doSquare(TH1F *h1, Char_t name[MaxPathSize]) {
  h1->Delete();
  doSquare(name);
  return;
}


//=====================================================================
//  doSquare()
//=====================================================================
void doSquare(Char_t name[MaxPathSize]) {

  float imin = 0.1;
  float imax = 0.94;

  TLine *TLineBorder;
  TLineBorder = new TLine(imin,imax,imax,imax);
  TLineBorder->Draw();
  TLineBorder = new TLine(imin,imin,imax,imin);
  TLineBorder->Draw();
  TLineBorder = new TLine(imin,imax,imin,imin);
  TLineBorder->Draw();
  TLineBorder = new TLine(imax,imax,imax,imin);
  TLineBorder->Draw();

  TLatex *t = new TLatex();
  t->SetNDC();
  t->SetTextFont(42);
  t->SetTextColor(kRed);
  t->SetTextSize(0.06);
  t->SetTextAngle(25);
  t->DrawLatex(0.35,0.5,name);
  sprintf(name,"no processed");
  t->DrawLatex(0.38,0.44,name);
  return;
}


//=====================================================================
//  forceFont()
//=====================================================================
void forceFont(TH1F *h1) {
  h1->SetLabelFont(font,"X");
  h1->SetLabelFont(font,"Y");
  h1->SetLabelFont(font,"Z");
  h1->SetTitleFont(font,"x");
  h1->SetTitleFont(font,"y");
  h1->SetTitleFont(font,"z");
  h1->GetYaxis()->SetTitleOffset(1.8);
  return;
}


//=====================================================================
//  DrawFitFunc()
//=====================================================================
FitValues DrawFitFunc(TH1F *h1, Color_t setColor, bool ShowFit) {
  // look for fitted fuctions
  TList *FunList;
  FunList = h1->GetListOfFunctions();
  
  FitValues ifit;

  if (FunList->GetEntries() != 0) {
    TF1 *Fun = (TF1*) FunList->First();
    Fun->SetLineColor(setColor);
    Fun->SetLineWidth(1);
    if(ShowFit) Fun->Draw("same");

    // Cas 1: ajust a una gaussiana sols
    if (strcmp(Fun->GetName(),"gaus")==0) {
      ifit.FitMean = Fun->GetParameter(1);
      ifit.FitSigma = Fun->GetParameter(2);
      ifit.FitErrorMean = Fun->GetParError(1);
      ifit.FitErrorSigma = Fun->GetParError(2);
    }
    // Cas 2: ajust a dos gaussianas sumades
    if (strcmp(Fun->GetName(),"GaussPlusGauss")==0) {
      double Heigh1 = Fun->GetParameter(0);
      double Mean1  = Fun->GetParameter(1);
      double Sigma1 = Fun->GetParameter(2);
      double Heigh2 = Fun->GetParameter(3);
      double Mean2  = Fun->GetParameter(4);
      double Sigma2 = Fun->GetParameter(5);

      double Weight1 = Heigh1/Sigma1;
      double Weight2 = Heigh2/Sigma2;
      
      ifit.FitMean =  (Mean1*Weight1 + Mean2*Weight2 ) / (Weight1 + Weight2);

      ifit.FitSigma = sqrt( (Sigma1*Sigma1*Weight1*Weight1 + Sigma2*Sigma2*Weight2*Weight2) / (Weight1*Weight1 + Weight2*Weight2) );

//       double errorHeigh1 = Fun->GetParError(0);
      double errorMean1  = Fun->GetParError(1);
      double errorSigma1 = Fun->GetParError(2);
//       double errorHeigh2 = Fun->GetParError(3);
//       double errorMean2  = Fun->GetParError(4);
//       double errorSigma2 = Fun->GetParError(5);

      //      int N = h1->GetEntries();

      ifit.FitErrorSigma = errorSigma1;
      ifit.FitErrorMean = errorMean1;
      // ifit.FitErrorMean = FitErrorSigma/sqrt(N);

    }

    // printf("Mean: (%2.6f +- %2.6f) um\n",1000*ifit.FitMean,1000*ifit.FitErrorMean);
    // printf("Sigma: (%2.6f +- %2.6f) um\n",1000*ifit.FitSigma,1000*ifit.FitErrorSigma);
    
  }
  
  return ifit;
}
