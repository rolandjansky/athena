////////////////////////////////////////////////////////////
// plotHisto.C
// 
// last modified 2004/06/28
//
// plots histogram
////////////////////////////////////////////////////////////

#include <iostream>

#include "TH1.h"
#include "TObject.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

bool plotHisto(TString histoName, TString axisTitleX="", TString axisTitleY="", TString axisTitleZ="", TString histoTitle="", TString drawOption="") {
  // draw histogram
  // 
  // TString histoName      name of histogram to plot
  // TString axisTitleX     X axis title
  // TString axisTitleY     Y axis title
  // TString axisTitleZ     Z axis title (only relevant for TH2* histos)
  // TString histoTitle     title of histogram.  "" means do not change the existing title
  // TString drawOption     histo Draw() option.  As for ROOT: If "", the default is HIST for 1D, and SCAN for 2D
  //                        to produce a lego plot, use LEGO
  //
  // returns true if histogram got printed

  // try to retrieve object
  TObject* object = gDirectory->Get(histoName);
  if (!object) {
    cout << "WARNING: Object " + histoName + " not found" << endl;
    return false;
  }

  // get TObject class name associated to a name
  TString className = object->ClassName();
  TString classNameBase = TString(className.Data(), 3);
  // cout << "class name " << className << " " << classNameBase << endl;
  if (classNameBase != "TH1" && classNameBase != "TH2") {
    cout << "WARNING: Object " + histoName + " not of class TH1* or TH2*" << endl;
    return false;
  }

  // get the histogram
  TH1* h = (TH1*)object;

  // 1D and 2D common customize
  h->GetXaxis()->SetTitle(axisTitleX);
  h->GetYaxis()->SetTitle(axisTitleY);
  if (histoTitle != "") h->SetTitle(histoTitle);
  
  // here customize for 1D or 2D histos
  if (classNameBase == "TH1") {  // 1D
    gStyle->SetOptStat(111111);
    h->GetXaxis()->SetLabelSize(0.05);
    h->GetXaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetTitleOffset(1.2);
    h->GetYaxis()->SetLabelSize(0.04);
    if (drawOption == "") drawOption = "HIST";  // because "" actually plots with error bars
  } else {                      // 2D
    gStyle->SetOptStat(1111);
    if (drawOption == "LEGO") {
      h->GetXaxis()->SetLabelSize(0.05);
      h->GetXaxis()->CenterTitle();
      h->GetXaxis()->SetTitleSize(0.05);
      h->GetXaxis()->SetTitleOffset(1.5);
      h->GetYaxis()->SetLabelSize(0.05);
      h->GetYaxis()->CenterTitle();
      h->GetYaxis()->SetTitleSize(0.05);
      h->GetYaxis()->SetTitleOffset(1.5);
      h->GetYaxis()->SetLabelOffset(0.);
      h->GetZaxis()->SetTitleOffset(1.1);
      h->GetZaxis()->SetTitle(axisTitleZ);
    } else {
      h->GetXaxis()->SetLabelSize(0.05);
      h->GetXaxis()->SetTitleOffset(1.1);
      h->GetYaxis()->SetTitleOffset(1.2);
      h->GetYaxis()->SetLabelSize(0.04);
    }
  }
  h->Draw(drawOption);

  // date and time
  dateAndTimeLabel()->Draw();
  // run number 
  runNumberLabel()->Draw();

  return true;
}
