/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// ROOT macro to draw efficiency plots for pions
// from tau deacys using the histograms from TauTrackTools/TauTrackEfficiencyAlg
// Plots are shown for the different settings of the track selector
//
// Efficiencies are calculated from the histograms containing the number
// of true and reconstructed pions by projecting them and using TGraphAsymmErrors
// By default efficiencies are plotted vs pT(pion)
//
// Sebastian Fleischmann <Sebastian.Fleischmann -at- cern.ch>
///////////////////////////////////////////////////////////////////////

#include <vector>
#include "Style_Plain.C"
#include "TH3I.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"

// parameters:
// plot vs pT(pion) or vs pTVis(tau)
static const bool   useTauPtVis = false;
// eta range to project on
// (default settings in TauTrackEfficiencyAlg: 5 bins from eta=0 to eta=2.5, i.e. delta eta=0.5 per bin)
static const int    minEtaBin   = 1;    // min eta = 0.
static const int    maxEtaBin   = 3;    // max eta = 1.5
// Y-axis range
static const double minEff   = 0.5;
static const double maxEff   = 1.0;
// X-axis range
static const double minPt   = 0.;
static const double maxPt   = 150.;

void plotEff(char* directory, TFile *file0, char* text, bool addLegend = false);

void TrackEffHistos() {

    //================ Trying out the style ===================/

    TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
    // Canvas
    plain->SetCanvasBorderMode(0);
    plain->SetCanvasColor(0);
    // Pad
    plain->SetPadBorderMode(0);
    plain->SetPadColor(0);
    // Title
    plain->SetTitleColor(0);
    plain->SetTitleBorderSize(0);
    plain->SetTitleX(0.25);
    plain->SetTitleY(0.98);
    // Stat
    plain->SetStatColor(0);
    //plain->SetOptStat(11111);
    //plain->SetOptStat("neMR");
    plain->SetOptStat("eMR");
    //plain->SetOptStat(kFALSE);
    plain->SetOptFit(111);
    // Labelling
    plain->SetLabelFont(132, "XYZ");
    plain->SetLabelSize(0.05, "XYZ");
    plain->SetHistLineWidth(2);
    Style_Plain();
    gROOT->SetStyle("Plain");

    TCanvas* histCanv = new TCanvas("TauTrackEff", "Tau track efficiencies", 100, 100, 600, 800);
    histCanv->cd();
    histCanv->Divide(2, 3);
    // open file
    TFile *file0 = TFile::Open("TauTrackEff.root");
    file0->ls();

    histCanv->cd(1);
    plotEff("/TauTrackEffStandardCuts", file0, "standard cuts", true);
    histCanv->cd(2);
    plotEff("/TauTrackEffLeading", file0, "leading track cuts");
    histCanv->cd(3);
    plotEff("/TauTrackEffSecond", file0, "second track cuts");
    histCanv->cd(4);
    plotEff("/TauTrackEffThird", file0, "third track cuts");
    histCanv->cd(5);
    plotEff("/TauTrackEffTauCutsCalo", file0, "tau cuts");
    histCanv->cd(6);
    plotEff("/TauTrackEffTauCutsCaloNoChi2", file0, "tau cuts without chi2 cut");

}

void plotEff(char* directory, TFile *file0, char* text, bool addLegend) {
    file0->cd(directory);
    // get histos
    TH3I *truthPionEtaPt    = 0;
    TH3I *matchedPionEtaPt  = 0;
    if (useTauPtVis) {
        truthPionEtaPt      = (TH3I*) gDirectory->Get("truthPionEtaPtVis");
        matchedPionEtaPt    = (TH3I*) gDirectory->Get("matchedPionEtaPtVis");
    } else {
        truthPionEtaPt      = (TH3I*) gDirectory->Get("truthPionEtaPt");
        matchedPionEtaPt    = (TH3I*) gDirectory->Get("matchedPionEtaPt");
    }
    TLegend *leg = 0;
    if (addLegend) {
        leg = new TLegend(0.4,0.6,0.89,0.89);
        leg->SetFillColor(0);
    }
    //////////////////////////////
    // 1prong without conversions:

    // project on given eta range
    TH1 *histProjTruth = truthPionEtaPt->ProjectionZ("truePions3p", 1, 1, minEtaBin, maxEtaBin, "e");
    TH1 *histProjMatched = matchedPionEtaPt->ProjectionZ("matchedPions3p", 1, 1, minEtaBin, maxEtaBin, "e");
    // create efficiency plot
    TGraphAsymmErrors* effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
    // set axis labels, etc.
    effPlot->Draw("AP");
    effPlot->SetMarkerStyle(20);
    effPlot->SetMaximum(maxEff);
    effPlot->SetMinimum(minEff);
    effPlot->GetXaxis()->SetLimits(minPt, maxPt);
    if (useTauPtVis) {
        effPlot->GetXaxis()->SetTitle("p_{T}^{vis}(#tau) [GeV]");
    } else {
        effPlot->GetXaxis()->SetTitle("p_{T}(#pi^{#pm}) [GeV]");
    }
    effPlot->GetXaxis()->SetTitleFont(132);
    effPlot->GetXaxis()->SetTitleSize(0.045);
    effPlot->GetXaxis()->SetTitleColor(1);
    effPlot->GetYaxis()->SetDecimals();
    effPlot->GetYaxis()->SetNdivisions(50205);
    gPad->SetGridx();
    gPad->SetGridy();
    if (leg) leg->AddEntry(effPlot,"1 Prong","p");
    delete histProjTruth;
    delete histProjMatched;
    //////////////////////////////
    // 1prong with conversions:
    histProjTruth = truthPionEtaPt->ProjectionZ("truePions3pConv", 2, 2, minEtaBin, maxEtaBin, "e");
    histProjMatched = matchedPionEtaPt->ProjectionZ("matchedPions3pConv", 2, 2, minEtaBin, maxEtaBin, "e");
    effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
    effPlot->Draw("P");
    effPlot->SetMarkerStyle(24);
    effPlot->SetMaximum(maxEff);
    effPlot->SetMinimum(minEff);
    if (leg) leg->AddEntry(effPlot,"1 Prong + Conv","p");
    delete histProjTruth;
    delete histProjMatched;
    //////////////////////////////
    // 3prong without conversions:
    TH1 *histProjTruth = truthPionEtaPt->ProjectionZ("truePions3p", 3, 3, minEtaBin, maxEtaBin, "e");
    TH1 *histProjMatched = matchedPionEtaPt->ProjectionZ("matchedPions3p", 3, 3, minEtaBin, maxEtaBin, "e");
    TGraphAsymmErrors* effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
    effPlot->Draw("P");
    effPlot->SetMarkerStyle(22);
    effPlot->SetMaximum(maxEff);
    effPlot->SetMinimum(minEff);
    if (leg) leg->AddEntry(effPlot,"3 Prong","p");
    delete histProjTruth;
    delete histProjMatched;
    //////////////////////////////
    // 3prong with conversions:
    histProjTruth = truthPionEtaPt->ProjectionZ("truePions3pConv", 4, 4, minEtaBin, maxEtaBin, "e");
    histProjMatched = matchedPionEtaPt->ProjectionZ("matchedPions3pConv", 4, 4, minEtaBin, maxEtaBin, "e");
    effPlot = new TGraphAsymmErrors(histProjMatched, histProjTruth, "");
    effPlot->Draw("P");
    effPlot->SetMarkerStyle(26);
    effPlot->SetMaximum(maxEff);
    effPlot->SetMinimum(minEff);
    if (leg) leg->AddEntry(effPlot,"3 Prong + Conv","p");
    delete histProjTruth;
    delete histProjMatched;
    // draw label and legend
    TPaveLabel *pl = new TPaveLabel(0.15,0.2,0.7,0.3,text,"brNDC");
    pl->SetLineColor(0);
    //pl->SetLineStyle(1);
    pl->SetLineWidth(0);
    pl->SetFillColor(0);
    pl->SetTextAlign(12);
    pl->SetTextSize(0.99);
    pl->Draw();
    if (leg) leg->Draw();
}
