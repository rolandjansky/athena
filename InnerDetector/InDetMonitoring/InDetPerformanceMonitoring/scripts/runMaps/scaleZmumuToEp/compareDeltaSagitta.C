//
#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TLine.h"
#include "TProfile.h"
#include "TString.h"
#include "TStyle.h"
#include "TText.h"

TCanvas* canDeltaSagittaComp;

TH2F* theHist1;
TH2F* theHist2;


float theMax = -9999., theMin = -9999.; // default values
/////////////////////////////////////
void compareDeltaSagitta()
{
  TFile *inputHistosFile1 = TFile::Open("correctionmaps_Recommendations2016data_PreTSI_30x30.root");
  TFile *inputHistosFile2 = TFile::Open("correctionmaps_HighGran_preTSI_30x30_INDET_2016_datareproAll25ns_correctedEp.root");
  TString legend1("Original");
  TString legend2("Reweighted");
  TString htitle1("#delta_{sagitta} map  ");
  TString htitle2("#delta_{sagitta} map  ");
  TString histoName1("LambdaCorrectionVsEtaPhi");
  TString histoName2("LambdaCorrectionVsEtaPhi_reweightedToEP");

  

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
  
  //init text
  TText theText;
  theText.SetTextColor(kGray+2);
  TGraph * thePoints = new TGraph ();

  // correlation between delta sagitta results
  // theHist -> original method as implemented in ATLAS-CONF-2012-141
  // theHist2 -> new method
  theHist1 = (TH2F*)inputHistosFile1->Get(histoName1.Data()); 
  theHist2 = (TH2F*)inputHistosFile2->Get(histoName2.Data()); 
  //theHist1 = (TH2F*)inputHistosFile1->Get("h_deltaSagittaMap"); 
  //theHist2 = (TH2F*)inputHistosFile2->Get("h_deltaSagittaMap"); 
  if (theHist1 != NULL && theHist2 != NULL ) {
    // 
    theHist1->SetStats(kFALSE);
    theHist2->SetStats(kFALSE);
    // first set the same scale for both:
    theMax = max(theHist1->GetBinContent(theHist1->GetMaximumBin()), theHist2->GetBinContent(theHist2->GetMaximumBin())); 
    theMin = min(theHist1->GetMinimum(), theHist2->GetMinimum());
    theMin = min(theHist1->GetBinContent(theHist1->GetMinimumBin()), theHist2->GetBinContent(theHist2->GetMinimumBin())); 
    theMax = max(fabs(theMax), fabs(theMin));
    if (theMax>1) theMax = 1.;
    theMin = -theMax;
    theHist1->SetMaximum(theMax);
    theHist1->SetMinimum(theMin);
    theHist2->SetMaximum(theMax);
    theHist2->SetMinimum(theMin);

    // set titles
    htitle1.Append(legend1.Data());
    theHist1->SetTitle(htitle1.Data());

    htitle2.Append(legend2.Data());
    theHist2->SetTitle(htitle2.Data());
    
    // now create a canvas to store the maps
    canDeltaSagittaComp = new TCanvas("canDeltaSagittaComp", "canDeltaSagittaComp",1200,1200);
    canDeltaSagittaComp->Divide(2,2);

    // draw options (with or without text)
    TString drawOptions("colz");
    if ((theHist1->GetXaxis()->GetNbins() * theHist1->GetYaxis()->GetNbins()) < 64) drawOptions.Append(" text");
  
    // draw histograms
    canDeltaSagittaComp->cd(1);
    theHist1->Draw(drawOptions.Data());
    canDeltaSagittaComp->cd(3);
    theHist2->Draw(drawOptions.Data());

    // correlation between both delta_sagitta
    canDeltaSagittaComp->cd(2);
    int pointIndex=0;
    thePoints->SetMarkerColor(kBlue+1);
    thePoints->SetMarkerStyle(kOpenCircle);
    thePoints->SetLineWidth(2);
    for (int etabin=1; etabin <= theHist1->GetNbinsX(); etabin++) {
      for (int phibin=1; phibin <= theHist1->GetNbinsY(); phibin++) {
	thePoints->SetPoint(pointIndex, theHist1->GetBinContent(etabin,phibin),  theHist2->GetBinContent(etabin,phibin)); 
	pointIndex++;
      }
    }
    TString theAxisTitle(theHist1->GetZaxis()->GetTitle());
    theAxisTitle.Append(" [");
    theAxisTitle.Append(legend1.Data());
    theAxisTitle.Append("]");
    thePoints->GetXaxis()->SetTitle(theAxisTitle.Data());

    theAxisTitle.Clear();
    theAxisTitle.Append(theHist2->GetZaxis()->GetTitle());
    theAxisTitle.Append(" [");
    theAxisTitle.Append(legend2.Data());
    theAxisTitle.Append("]");
    thePoints->GetYaxis()->SetTitle(theAxisTitle.Data());

    thePoints->Draw("AP");

    // now make x and y axis to have the same range:
    theMin = std::max(thePoints->GetXaxis()->GetXmin(), thePoints->GetYaxis()->GetXmin());
    theMax = std::min(thePoints->GetXaxis()->GetXmax(), thePoints->GetYaxis()->GetXmax());
    thePoints->GetXaxis()->SetLimits(theMin, theMax);
    thePoints->GetYaxis()->SetLimits(theMin, theMax);
    gPad->Update();
    gPad->SetGridx();
    gPad->SetGridy();
    // diagonal
    TLine theDiagonal;
    theDiagonal.SetLineColor(kGray+1);
    theDiagonal.SetLineStyle(kDashed);
    theDiagonal.DrawLine(theMin, theMin, theMax, theMax);

    TString correlationText; correlationText.Clear();
    correlationText.Form("Correlation factor = %1.3f",thePoints->GetCorrelationFactor());
    theText.DrawTextNDC(0.15,0.8,correlationText.Data());

    if (true) {
      // the difference
      canDeltaSagittaComp->cd(4);
      //TH2F* histDiff = (TH2F*) theHist1->Clone();
      //histDiff->SetTitle("#delta_{sagitta} difference");
      //histDiff->Add(theHist2,-1); // -> this way has some problem with the Z axis range -> do it by hand
      TH2F* histDiff = new TH2F("histDiff","#delta_{sagitta} difference",
				theHist1->GetXaxis()->GetNbins(), theHist1->GetXaxis()->GetXmin(), theHist1->GetXaxis()->GetXmax(),  
				theHist1->GetYaxis()->GetNbins(), theHist1->GetYaxis()->GetXmin(), theHist1->GetYaxis()->GetXmax());  
      histDiff->SetXTitle(theHist2->GetXaxis()->GetTitle());
      histDiff->SetYTitle(theHist2->GetYaxis()->GetTitle());
      theAxisTitle.Clear();
      theAxisTitle.Append(theHist2->GetZaxis()->GetTitle());
      theAxisTitle.Prepend("#Delta");
      histDiff->SetZTitle(theAxisTitle.Data());
      for (int etabin=1; etabin <= theHist1->GetNbinsX(); etabin++) {
	for (int phibin=1; phibin <= theHist1->GetNbinsY(); phibin++) {
	  histDiff->SetBinContent(etabin, phibin, theHist1->GetBinContent(etabin, phibin) - theHist2->GetBinContent(etabin, phibin)); 
	}
      }
      // find range
      theMax = histDiff->GetBinContent(histDiff->GetMaximumBin());
      theMin = histDiff->GetBinContent(histDiff->GetMinimumBin());
      theMax = max(fabs(theMax), fabs(theMin));
      if (theMax>1) theMax = 1.;
      theMin = -theMax;
      histDiff->SetMaximum(theMax);
      histDiff->SetMinimum(theMin);
      histDiff->SetStats(kFALSE);
      histDiff->Draw(drawOptions.Data());
    }
    else {
      theMax = max(fabs(theMax), fabs(theMin));
      theMin = -theMax;
      // two pads to see the distributions
      TH1F* histDelta1 = new TH1F("histDelta1","#delta_{sagitta}",
				  std::min(theHist1->GetXaxis()->GetNbins(), 10), theMin, theMax);
      histDelta1->SetXTitle(thePoints->GetXaxis()->GetTitle());

      TH1F* histDelta2 = new TH1F("histDelta1","#delta_{sagitta}",
				  std::min(theHist1->GetXaxis()->GetNbins(), 10), theMin, theMax);
      histDelta2->SetXTitle(thePoints->GetYaxis()->GetTitle());


      histDelta1->SetStats(false);
      histDelta2->SetStats(false);
      for (int etabin=1; etabin <= theHist1->GetNbinsX(); etabin++) {
	for (int phibin=1; phibin <= theHist1->GetNbinsY(); phibin++) {
	  histDelta1->Fill(theHist1->GetBinContent(etabin, phibin)); 
	  histDelta2->Fill(theHist2->GetBinContent(etabin, phibin)); 
	}
      }
      TString theStatsText; theText.Clear();


      canDeltaSagittaComp->cd(4);
      TPad* pad1 = new TPad ("pad1","pad1", 0.0,0.0,0.5,1.0);
      pad1->Draw();
      pad1->cd();
      histDelta1->Draw();
      theStatsText.Form("mean = %1.3f", histDelta1->GetMean());
      theText.DrawTextNDC(0.55,0.8,theStatsText.Data());
      theStatsText.Form("rms = %1.3f", histDelta1->GetRMS());
      theText.DrawTextNDC(0.55,0.75,theStatsText.Data());

      canDeltaSagittaComp->cd(4);
      TPad* pad2 = new TPad ("pad2","pad2", 0.5,0.0,1.0,1.0);
      pad2->Draw();
      pad2->cd();
      histDelta2->Draw();
      theStatsText.Form("mean = %1.3f", histDelta2->GetMean());
      theText.DrawTextNDC(0.55,0.8,theStatsText.Data());
      theStatsText.Form("rms = %1.3f", histDelta2->GetRMS());
      theText.DrawTextNDC(0.55,0.75,theStatsText.Data());
    }
    
    canDeltaSagittaComp->Print("deltaSagitta_Comparison.png");
  }



  std::cout << endl << "    ** thePlots ** Game Over. Insert coin ** " << std::endl << std::endl;
  return;
}
