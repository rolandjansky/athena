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

TCanvas* dimuonCanvas;
TCanvas* posmuonCanvas;
TCanvas* negmuonCanvas;
TCanvas* deltaSagittaCanvas;
TCanvas* sigmaCanvas;
TCanvas* sigmaProfCanvas;
TCanvas* corrDimuonDeltaSagittaPos;
TCanvas* corrDimuonDeltaSagittaNeg;
TCanvas* canDeltaSagittaComp;
TCanvas* canPtLeadVsMass;
TCanvas* canRadialDistortion;
TCanvas* canRadialDistortionInput;
TCanvas* canRadialDistVsPt;
TCanvas* canDeltaMVsEta;
TCanvas* canDeltaMVsPtlead;
TCanvas* deltaSagittaCanvas2;

TH2F* theHist;
TH2F* theHist2;
TH1F* theHist1D;

TProfile* theProf;
TH1D* theProj;

float theMax = -9999., theMin = -9999.; // default values
/////////////////////////////////////
void thePlots()
{
  TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test.root");
  //TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_Jpsi_MCbiasCorr_WithDeltaR_5Iter_Yes.root");
  //TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_JPsi_WithMCbiasCorr_1iter.root");

  bool ZmumuPlots = false;
  bool JPsiPlots = true;

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
  
  //init text
  TText theText;
  theText.SetTextColor(kGray+2);
  TGraph * thePoints = new TGraph ();

  /////////
  theHist = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphiPos_fitresultVsPDG"); 
  if (theHist != NULL) {
    theHist->GetZaxis()->SetTitle("#Delta M [MeV]");
    posmuonCanvas = new TCanvas ("posmuonCanvas","posmuonCanvas", 900,600);
    posmuonCanvas->cd();
    //theMax =  347.;
    //theMin = -theMax;
    //theHist->SetMaximum(theMax);
    //theHist->SetMinimum(theMin);
    theHist->Draw("colz text");
    posmuonCanvas->Print("posmuon_deltaM.png");
    theMax = theHist->GetMaximum();
    theMin = theHist->GetMinimum();
  }

  std::cout << " ** thePlots ** dimuon Pos ** range: " << theMax << " --> " << theMin << std::endl;
  theHist = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG"); 
  if (theHist != NULL) {
    theHist->GetZaxis()->SetTitle("#Delta M [MeV]");
    negmuonCanvas = new TCanvas ("negmuonCanvas","negmuonCanvas", 900,600);
    negmuonCanvas->cd();
    //theMax =  347.;
    //theMin = -theMax;
    theHist->SetMaximum(theMax);
    theHist->SetMinimum(theMin);
    theHist->Draw("colz text");
    negmuonCanvas->Print("negmuon_deltaM.png");
  }

  theHist = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphi_fitresultVsPDG"); 
  //theHist = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphi_fitresult"); 
  if (theHist != NULL) {
    theHist->GetZaxis()->SetTitle("#Delta M [MeV]");
    dimuonCanvas = new TCanvas ("dimuonCanvas","dimuonCanvas", 900,600);
    dimuonCanvas->cd();
    std::cout << " the max = " << theMax << std::endl;
    if (theMax < 10.) theMax = 10.; //
    theMin = -theMax;
    theHist->SetMaximum(theMax);
    theHist->SetMinimum(theMin);
    gStyle->SetPaintTextFormat("5.3f");
    theHist->Draw("colz text");
    dimuonCanvas->Print("dimuon_deltaM.png");

    // perform an eta projection
    theProj = theHist->ProjectionX();
    canDeltaMVsEta = new TCanvas ("canDeltaMVsEta","Fitted mass vs #eta", 900,600);
    theProj->SetYTitle(theHist->GetZaxis()->GetTitle());
    theProj->SetStats(kFALSE);
    theProj->SetMaximum(theHist->GetMaximum());
    theProj->SetMinimum(theHist->GetMinimum());
    theProj->Draw(); 
    canDeltaMVsEta->Print("dimuon_deltaM_vs_eta.png");
  }



  /////////////////////////////////////////////////////////////////
  theHist = (TH2F*)inputHistosFile->Get("LambdaCorrectionVsEtaPhi"); 
  if (theHist != NULL) {
    deltaSagittaCanvas = new TCanvas ("deltaSagittaCanvas","deltaSagittaCanvas", 900,600);
    deltaSagittaCanvas->cd();
    theHist->SetStats(kFALSE);
    theMax = theHist->GetMaximum();
    theMin = theHist->GetMinimum();
    if (fabs(theMin) > theMax) {
      theMax = fabs(theMin);
    }
    else {
      theMin = -theMax;
    }
    std::cout << " ** thePlots ** deltaSagitta ** range: " << theMax << " --> " << theMin << std::endl;
    theMax = 0.86884;
    theMin = -theMax;
    theHist->SetMaximum(1.02*theMax);
    theHist->SetMinimum(1.02*theMin);
    theHist->Draw("colz text");
    deltaSagittaCanvas->Print("deltaSagitta.png");
  }

  theHist = (TH2F*)inputHistosFile->Get("h_deltaSagittaMap"); 
  if (theHist != NULL) {
    deltaSagittaCanvas2 = new TCanvas ("deltaSagittaCanvas2","deltaSagittaCanvas2", 900,600);
    deltaSagittaCanvas2->cd();
    theHist->SetStats(kFALSE);
    theMax = theHist->GetMaximum();
    theMin = theHist->GetMinimum();
    if (fabs(theMin) > theMax) {
      theMax = fabs(theMin);
    }
    else {
      theMin = -theMax;
    }
    std::cout << " ** thePlots ** deltaSagitta ** range: " << theMax << " --> " << theMin << std::endl;
    //theMax = 0.86884;
    //theMin = -theMax;
    theHist->SetMaximum(1.02*theMax);
    theHist->SetMinimum(1.02*theMin);
    theHist->Draw("colz ");
    deltaSagittaCanvas2->Print("deltaSagitta_PtBased.png");
  }

  theHist = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphi_sigma"); 
  if (theHist != NULL) {
    sigmaCanvas = new TCanvas ("sigmaCanvas","sigmaCanvas", 900,600);
    sigmaCanvas->cd();
    theHist->SetTitle("dimuon invariant mass fit: #sigma of resolution model");
    theHist->SetZTitle("#sigma_{R} [MeV]");

    const Int_t NRGBs = 3;
    const Int_t NCont = 50;
    Double_t stops[NRGBs] = {0.33, 0.66, 1.00 };
    Double_t red[NRGBs] = { 0.10, 1.00, 1.00 };
    Double_t green[NRGBs] = {0.90, 1.00, 0.05 };
    Double_t blue[NRGBs] = {0.10, 0.00, 0.00 };
    //Double_t red[NRGBs] = { 1.00, 1.00, 0.10 };
    //Double_t green[NRGBs] = {0.05, 1.00, 0.90 };
    //Double_t blue[NRGBs] = {0.00, 0.00, 0.10 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue,
				     NCont);
    gStyle->SetNumberContours(NCont);

    // Zmumu
    if (ZmumuPlots) {
      theHist->SetMaximum(3050.);
      theHist->SetMinimum(2675.);
    }
    // Jpsi
    if (JPsiPlots) {
      theHist->SetMaximum(125.);
      theHist->SetMinimum(25.);
    }
    std::cout << " ** thePlots ** deltaSagitta ** range: " << theHist->GetMinimum() << " --> " << theHist->GetMaximum() << std::endl;
    gStyle->SetPaintTextFormat("4.3f");
    theHist->Draw("colz text");
    sigmaCanvas->Print("dimuon_sigma.png");

    
    // make an eta profile
    theProf = new TProfile("theProf", theHist->GetTitle(), theHist->GetNbinsX(), theHist->GetXaxis()->GetXmin(), theHist->GetXaxis()->GetXmax());    
    theProf->SetStats(kFALSE);
    theProf->SetXTitle(theHist->GetXaxis()->GetTitle());
    theProf->SetYTitle(theHist->GetZaxis()->GetTitle());
    theProf->SetMarkerStyle(20);
    theProf->SetMarkerColor(kBlue+1);
    theProf->SetLineWidth(2);
    theProf->SetLineColor(kBlue+1);
    theProf->SetMinimum(theHist->GetMinimum());
    theProf->SetMaximum(theHist->GetMaximum());
    for (int etabin=1; etabin <= theHist->GetNbinsX(); etabin++) {
      double theSum = 0;
      for (int phibin=1; phibin <= theHist->GetNbinsY(); phibin++) {
	theSum += theHist->GetBinContent(etabin, phibin);
	theProf->Fill(theHist->GetXaxis()->GetBinCenter(etabin), theHist->GetBinContent(etabin, phibin));
      }
    }
    sigmaProfCanvas = new TCanvas ("sigmaProfCanvas","sigmaProfCanvas", 900,600);
    sigmaProfCanvas->cd();
    gPad->SetGridy(1);
    theProf->Draw();
    sigmaProfCanvas->Print("dimuon_sigmaProf.png");
  }

  // correlation dimuon mass vs sagita distortion
  // Positive muon
  theHist = (TH2F*)inputHistosFile->Get("LambdaCorrectionVsEtaPhi"); 
  theHist2 = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphiPos_fitresultVsPDG"); 
  if (theHist != NULL && theHist2 != NULL ) {
    int pointIndex=0;
    TGraph * thePoints = new TGraph ();
    thePoints->SetMarkerColor(kBlue+1);
    thePoints->SetMarkerStyle(kOpenCircle);
    thePoints->SetLineWidth(2);
    for (int etabin=1; etabin <= theHist->GetNbinsX(); etabin++) {
      for (int phibin=1; phibin <= theHist->GetNbinsY(); phibin++) {
	thePoints->SetPoint(pointIndex, theHist->GetBinContent(etabin,phibin),  theHist2->GetBinContent(etabin,phibin)); 
	pointIndex++;
      }
    }
    corrDimuonDeltaSagittaPos = new TCanvas("corrDimuonDeltaSagittaPos", "corrDimuonDeltaSagittaPos", 900, 600);
    corrDimuonDeltaSagittaPos->cd();
    thePoints->GetXaxis()->SetTitle(theHist->GetZaxis()->GetTitle());
    thePoints->GetYaxis()->SetTitle("#Delta M [MeV]");
    thePoints->Draw("AP");
    gPad->SetGridx();
    gPad->SetGridy();
    TString correlationText; correlationText.Clear();
    correlationText.Form("Correlation factor = %1.3f",thePoints->GetCorrelationFactor());
    theText.DrawTextNDC(0.15,0.8,correlationText.Data());
    
    corrDimuonDeltaSagittaPos->Print("corrDimuonDeltaSagittaPos.png");
  }

  // correlation dimuon mass vs sagita distortion
  // Negative muon
  //theHist = (TH2F*)inputHistosFile->Get("LambdaCorrectionVsEtaPhi"); 
  theHist2 = (TH2F*)inputHistosFile->Get("h_dimuon_mass_vs_etaphiNeg_fitresultVsPDG"); 
  if (theHist != NULL && theHist2 != NULL ) {
    int pointIndex=0;
    thePoints->SetMarkerColor(kBlue+1);
    thePoints->SetMarkerStyle(kOpenCircle);
    thePoints->SetLineWidth(2);
    for (int etabin=1; etabin <= theHist->GetNbinsX(); etabin++) {
      for (int phibin=1; phibin <= theHist->GetNbinsY(); phibin++) {
	thePoints->SetPoint(pointIndex, theHist->GetBinContent(etabin,phibin),  theHist2->GetBinContent(etabin,phibin)); 
	pointIndex++;
      }
    }
    corrDimuonDeltaSagittaNeg = new TCanvas("corrDimuonDeltaSagittaNeg", "corrDimuonDeltaSagittaNeg", 900, 600);
    corrDimuonDeltaSagittaNeg->cd();
    thePoints->GetXaxis()->SetTitle(theHist->GetZaxis()->GetTitle());
    thePoints->GetYaxis()->SetTitle("#Delta M [MeV]");
    thePoints->Draw("AP");
    gPad->SetGridx();
    gPad->SetGridy();
    TString correlationText; correlationText.Clear();
    correlationText.Form("Correlation factor = %1.3f",thePoints->GetCorrelationFactor());
    theText.DrawTextNDC(0.40,0.8,correlationText.Data());
    
    corrDimuonDeltaSagittaNeg->Print("corrDimuonDeltaSagittaNeg.png");
  }


  // correlation between delta sagitta results
  // theHist -> original method as implemented in ATLAS-CONF-2012-141
  // theHist2 -> new method
  theHist = (TH2F*)inputHistosFile->Get("LambdaCorrectionVsEtaPhi"); 
  theHist2 = (TH2F*)inputHistosFile->Get("h_deltaSagittaMap"); 
  if (theHist != NULL && theHist2 != NULL ) {
    // first set the same scale for both:
    theMax = max(theHist->GetBinContent(theHist->GetMaximumBin()), theHist2->GetBinContent(theHist2->GetMaximumBin())); 
    theMin = min(theHist->GetMinimum(), theHist2->GetMinimum());
    theMin = min(theHist->GetBinContent(theHist->GetMinimumBin()), theHist2->GetBinContent(theHist2->GetMinimumBin())); 
    theMax = max(fabs(theMax), fabs(theMin));
    if (theMax>1) theMax = 1.;
    theMin = -theMax;
    theHist->SetMaximum(theMax);
    theHist->SetMinimum(theMin);
    theHist2->SetMaximum(theMax);
    theHist2->SetMinimum(theMin);

    // set titles
    theHist->SetTitle("#delta_{sagitta} map: ATLAS-CONF-2012-141");
    theHist2->SetTitle("#delta_{sagitta} map: p_{T} distribution");
    
    // now create a canvas to store the maps
    canDeltaSagittaComp = new TCanvas("canDeltaSagittaComp", "canDeltaSagittaComp",1200,1200);
    canDeltaSagittaComp->Divide(2,2);

    // draw options (with or without text)
    TString drawOptions("colz");
    if ((theHist->GetXaxis()->GetNbins() * theHist->GetYaxis()->GetNbins()) < 64) drawOptions.Append(" text");
  
    // draw histograms
    canDeltaSagittaComp->cd(1);
    theHist->Draw(drawOptions.Data());
    canDeltaSagittaComp->cd(3);
    theHist2->Draw(drawOptions.Data());

    // correlation between both delta_sagitta
    canDeltaSagittaComp->cd(2);
    int pointIndex=0;
    thePoints->SetMarkerColor(kBlue+1);
    thePoints->SetMarkerStyle(kOpenCircle);
    thePoints->SetLineWidth(2);
    for (int etabin=1; etabin <= theHist->GetNbinsX(); etabin++) {
      for (int phibin=1; phibin <= theHist->GetNbinsY(); phibin++) {
	thePoints->SetPoint(pointIndex, theHist->GetBinContent(etabin,phibin),  theHist2->GetBinContent(etabin,phibin)); 
	pointIndex++;
      }
    }
    TString theAxisTitle(theHist->GetZaxis()->GetTitle());
    theAxisTitle.Append(" [ATLAS-CONF-2012-141]");
    thePoints->GetXaxis()->SetTitle(theAxisTitle.Data());

    theAxisTitle.Clear();
    theAxisTitle.Append(theHist2->GetZaxis()->GetTitle());
    theAxisTitle.Append(" [p_{T} distribution]");
    thePoints->GetYaxis()->SetTitle(theAxisTitle.Data());

    thePoints->Draw("AP");

    // now make x and y axis to have the same range:
    theMin = std::min(thePoints->GetXaxis()->GetXmin(), thePoints->GetYaxis()->GetXmin());
    theMax = std::max(thePoints->GetXaxis()->GetXmax(), thePoints->GetYaxis()->GetXmax());
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

    if (false) {
      // the difference
      canDeltaSagittaComp->cd(4);
      //TH2F* histDiff = (TH2F*) theHist->Clone();
      //histDiff->SetTitle("#delta_{sagitta} difference");
      //histDiff->Add(theHist2,-1); // -> this way has some problem with the Z axis range -> do it by hand
      TH2F* histDiff = new TH2F("histDiff","#delta_{sagitta} difference",
				theHist->GetXaxis()->GetNbins(), theHist->GetXaxis()->GetXmin(), theHist->GetXaxis()->GetXmax(),  
				theHist->GetYaxis()->GetNbins(), theHist->GetYaxis()->GetXmin(), theHist->GetYaxis()->GetXmax());  
      histDiff->SetXTitle(theHist2->GetXaxis()->GetTitle());
      histDiff->SetYTitle(theHist2->GetYaxis()->GetTitle());
      theAxisTitle.Clear();
      theAxisTitle.Append(theHist2->GetZaxis()->GetTitle());
      theAxisTitle.Prepend("#Delta");
      histDiff->SetZTitle(theAxisTitle.Data());
      for (int etabin=1; etabin <= theHist->GetNbinsX(); etabin++) {
	for (int phibin=1; phibin <= theHist->GetNbinsY(); phibin++) {
	  histDiff->SetBinContent(etabin, phibin, theHist->GetBinContent(etabin, phibin) - theHist2->GetBinContent(etabin, phibin)); 
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
				  std::min(theHist->GetXaxis()->GetNbins(), 10), theMin, theMax);
      histDelta1->SetXTitle(thePoints->GetXaxis()->GetTitle());

      TH1F* histDelta2 = new TH1F("histDelta1","#delta_{sagitta}",
				  std::min(theHist->GetXaxis()->GetNbins(), 10), theMin, theMax);
      histDelta2->SetXTitle(thePoints->GetYaxis()->GetTitle());


      histDelta1->SetStats(false);
      histDelta2->SetStats(false);
      for (int etabin=1; etabin <= theHist->GetNbinsX(); etabin++) {
	for (int phibin=1; phibin <= theHist->GetNbinsY(); phibin++) {
	  histDelta1->Fill(theHist->GetBinContent(etabin, phibin)); 
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

  // reconstructed mass vs pt of leading muon
  theHist = (TH2F*)inputHistosFile->Get("h_dimuon_ptlead_vs_mass");
  if (theHist != NULL) {
    canPtLeadVsMass = new TCanvas ("canPtLeadVsMass","canvas: pt leading vs mass", 900,600);
    theHist->SetStats(kFALSE);
    theHist->GetYaxis()->SetRangeUser(0., 30.);
    gStyle->SetPalette(1);
    theHist->Draw("colz");
    theProf = theHist->ProfileX("ptleadvsmass_profx");
    theProf->SetLineColor(kBlack);
    theProf->SetLineWidth(2);
    theProf->Draw("same");
  }

  // radial distortion map
  theHist = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_etaphi_Integrated");
  if (theHist != NULL) {
    canRadialDistortion = new TCanvas ("canRadialDistortion","radial distortion map", 900,600);
    theHist->SetStats(kFALSE);
    theMax = 1.01*max(fabs(theHist->GetMaximum()), fabs(theHist->GetMinimum()));
    if(theMax < 1.1E-3) theMax = 1.1E-3;
    theMin = -theMax;
    theHist->SetMaximum( theMax);
    theHist->SetMinimum( theMin);
    theHist->GetZaxis()->SetTitle("#epsilon = #delta R/R");
    theHist->GetZaxis()->SetTitleOffset(1.35);
    gStyle->SetPalette(1);
    gStyle->SetPaintTextFormat("5.4f");
    theHist->Draw("colz text");
    canRadialDistortion->Print("RadialDistortion_map.png");

    theHist1D = (TH1F*) inputHistosFile->Get("h_radialdistortion_vs_pt_prof");
    if (theHist1D != NULL) {
      theHist1D->SetStats(kFALSE);
      theHist1D->SetTitle("Radial distortion (#epsilon = #delta R/R) vs leading p_{T}");
      theMax = 1.01*max(fabs(theHist1D->GetMaximum()), fabs(theHist1D->GetMinimum()));
      if (theMax < 0.001) theMax = 0.001;
      theMin = -theMax;
      theHist1D->SetMaximum( theMax);
      theHist1D->SetMinimum( theMin);
      theHist1D->GetYaxis()->SetTitleOffset(1.30);
      canRadialDistVsPt = new TCanvas ("canRadialDistVsPt","radial distortion vs pt", 900, 600);
      canRadialDistVsPt->cd();
      theHist1D->Draw();
      gPad->SetGridx();
      gPad->SetGridy();
      canRadialDistVsPt->Print("RadialDistortion_vs_pt.png");
    }

    theHist1D = (TH1F*) inputHistosFile->Get("p_dimuon_DeltaM_vs_ptlead");
    if (theHist1D != NULL) {
      theHist1D->SetStats(kFALSE);
      //theHist1D->SetTitle("Radial distortion (#epsilon = #delta R/R) vs leading p_{T}");
      theMax = 1.01*max(fabs(theHist1D->GetMaximum()), fabs(theHist1D->GetMinimum()));
      if (theMax < 0.001) theMax = 0.001;
      theMin = -theMax;
      theHist1D->SetMaximum( theMax);
      theHist1D->SetMinimum( theMin);
      theHist1D->GetYaxis()->SetTitleOffset(1.30);
      canDeltaMVsPtlead = new TCanvas ("canDeltaMVsPtlead","(M_{reco} - M_{MC})/MPDG vs leading p_{T}", 900, 600);
      canDeltaMVsPtlead->cd();
      theHist1D->Draw();
      gPad->SetGridx();
      gPad->SetGridy();
      canDeltaMVsPtlead->Print("DeltaM_vs_ptlead.png");
    }

    theHist = (TH2F*)inputHistosFile->Get("h_RadialDistortion_input");
    if (theHist != NULL) {
      canRadialDistortionInput = new TCanvas ("canRadialDistortionInput","input radial distortion map", 900,600);
      theHist->SetStats(kFALSE);
      theMax = 1.01*max(fabs(theHist->GetMaximum()), fabs(theHist->GetMinimum()));
      if(theMax < 1.1E-3) theMax = 1.1E-3;
      theMin = -theMax;
      theHist->SetMaximum( theMax);
      theHist->SetMinimum( theMin);
      theHist->GetZaxis()->SetTitle("#epsilon = #delta R/R");
      theHist->GetZaxis()->SetTitleOffset(1.35);
      gStyle->SetPalette(1);
      gStyle->SetPaintTextFormat("5.4f");
      theHist->Draw("colz text");
      canRadialDistortionInput->Print("RadialDistortion_input_map.png");
    }
  }



  std::cout << endl << "    ** thePlots ** Game Over. Insert coin ** " << std::endl << std::endl;
  return;
}
