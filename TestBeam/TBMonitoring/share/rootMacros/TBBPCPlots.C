#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TCollection.h"
#include "TH1F.h"
#include "TKey.h"
#include "TList.h"
#include "TPaveLabel.h"
#include "TString.h"
#include "TROOT.h"

using namespace std;

void TBBPCPlots(TString path, TString canvasTitle, TString canvasNameBase, int nx=2, int ny=2, int minIndex=0, int maxIndex=99) {

  gDirectory->cd(path);

  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  // set the number of pads per canvas
  //int nx = 2;
  //int ny = 2;

  gStyle->SetOptStat(111111);
  TPaveLabel* dateLabel = dateAndTimeLabel();

  int Index = 0;
  vector<TCanvas*> allCanvas;
  int canvasIndex = -1;
  int padIndex = 0;

  // prepare loop over histograms
  // the directory has a list of keys
  TList* listOfKeys = gDirectory->GetListOfKeys();
  TIter myIter(listOfKeys);
  TKey* key;
  // iterate over the keys
  while (key = (TKey*)myIter.Next()) {
    // key->Dump();
    if (Index >= minIndex && Index <= maxIndex) {
      // pad and canvas management
      if (padIndex%(nx*ny) == 0) {
        padIndex = 0;
        ++canvasIndex;
	TString canvasName = canvasNameBase;
	canvasName+=canvasIndex;  //appends canvasIndex to end of canvasNameBase
	TCanvas* oneCanvas = new TCanvas(canvasName, canvasTitle+" - "+canvasName, 350+20*canvasIndex, 10+20*canvasIndex, 600, 462);
        oneCanvas->Divide(nx, ny);
        allCanvas.push_back(oneCanvas);
      }
      allCanvas[canvasIndex]->cd(++padIndex);
      // get the histogram associated to a key
      TH1F* h = (TH1F*)gDirectory->Get(key->GetName());
      if (h) {
        h->GetXaxis()->SetLabelSize(0.05);
	//h->GetXaxis()->SetTitle("ADC counts");
        h->GetYaxis()->SetLabelSize(0.04);
        h->Draw("HIST");
        dateLabel->Draw();
        allCanvas[canvasIndex]->Update();
      }
    }
    ++Index;
  }
  gDirectory->cd();
}
