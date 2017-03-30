#include <iostream>

#include "TCanvas.h"
#include "TExec.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TProfile.h"
#include "TString.h"
#include "TStyle.h"

TCanvas * canSigmad0Reco;
TCanvas * canSigmad0Unfo;
TCanvas * canSigmaz0Reco;
TCanvas * canSigmaz0Unfo;

TCanvas * canSigmad0UnfoKPVup;
TCanvas * canSigmad0UnfoKPVdown;

TCanvas * canSigmad0UnfoKtrkup;
TCanvas * canSigmad0UnfoKtrkdown;


TCanvas * canSigmad0RecoPos;
TCanvas * canSigmad0RecoNeg;
TCanvas * canSigmad0UnfoldPos;
TCanvas * canSigmad0UnfoldNeg;
TCanvas * canSigmaz0RecoPos;
TCanvas * canSigmaz0RecoNeg;
TCanvas * canSigmaz0UnfoldPos;
TCanvas * canSigmaz0UnfoldNeg;

TCanvas * canProjSigmad0;

TCanvas * canSigmad0UnfoOldData;
TCanvas * canSigmad0UnfoOldMC; 
TCanvas * canSigmad0UnfoOldDataz0;
TCanvas * canSigmad0UnfoOldMCz0; 

TCanvas * canSigmad0RecoMean; 
TCanvas * canSigmad0UnfoMean; 
TCanvas * canSigmaz0RecoMean; 
TCanvas * canSigmaz0UnfoMean; 

TCanvas * cand0Ktrk;
TCanvas * canSigmaz0Ktrk;

TCanvas * canChi2z0Reco;
TCanvas * canChi2d0Reco;
TCanvas * canChi2z0Unfold;
TCanvas * canChi2d0Unfold;

TCanvas * canProjSigmad0NvsW;

TH2F* theHistR;
TH2F* theHistU;

TH2F* theHistP;
TH2F* theHistN;

TH2F* theHistKup;
TH2F* theHistKdown;

TH1 * h_projR;
TH1 * h_projU;

TH2F* theOldMCHist;
TH2F* theOldDataHist;
TH2F* theOldMCHistz0;
TH2F* theOldDataHistz0;

TH2F* thed0KtrkHist;
TH2F* thez0KtrkHist;

TH2F* thed0Chi2HistR;
TH2F* thez0Chi2HistR;
TH2F* thed0Chi2HistU;
TH2F* thez0Chi2HistU;

void   BuildPalettes (Int_t colorstyle=0);

/////////////////////////////////////
void theIPplots()
{

  TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test.root");
  bool do2015plots = false;
  TFile *inputHistosFilefromSteffen = NULL;
  if (do2015plots) inputHistosFilefromSteffen = TFile::Open("trackIPAlignTight.root");
  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.18);

  double theMax = 1000.;
  double theMin = 0.;
  if (do2015plots) {
    theOldMCHist = (TH2F*)inputHistosFilefromSteffen->Get("mc_zmumu/d0tru_Pt_Eta_Tight_zmumu"); 
    theOldDataHist = (TH2F*)inputHistosFilefromSteffen->Get("data_zmumu/d0unf_Pt_Eta_Tight_zmumu"); 
    theOldMCHistz0 = (TH2F*)inputHistosFilefromSteffen->Get("mc_zmumu/z0tru_Pt_Eta_Tight_zmumu"); 
    theOldDataHistz0 = (TH2F*)inputHistosFilefromSteffen->Get("data_zmumu/z0unf_Pt_Eta_Tight_zmumu"); 
  }

  bool doKtrkplots = true;
  if (doKtrkplots) {
    thed0KtrkHist = (TH2F*)inputHistosFile->Get("histktrk_d0_pt_eta"); 
    thez0KtrkHist = (TH2F*)inputHistosFile->Get("histktrk_z0_pt_eta");
  }

  bool dochi2plots = true;
  if (dochi2plots) {
    thed0Chi2HistR = (TH2F*)inputHistosFile->Get("histd0_chi2");
    thez0Chi2HistR = (TH2F*)inputHistosFile->Get("histz0_chi2");
    thed0Chi2HistU = (TH2F*)inputHistosFile->Get("histd0_chi2_unfold");
    thez0Chi2HistU = (TH2F*)inputHistosFile->Get("histz0_chi2_unfold"); 
  }


  // Palettes
  TExec *PaletteForSigma = new TExec("PaletteForSigma","BuildPalettes(0);");
  TExec *PaletteForK = new TExec("PaletteForK","BuildPalettes(1);");
  TExec *PaletteForChi2 = new TExec("PaletteForChi2","BuildPalettes(0);");

  PaletteForK->Exec(); // set palette
  /////////////////////////////////////////////////////////////////
  if (thed0KtrkHist != NULL && thez0KtrkHist != NULL) {
    //theHistR->GetZaxis()->SetTitle("Reco #sigma_{z0} [mm]");
    //theHistU->GetZaxis()->SetTitle("Unfolded #sigma_{z0} [mm]");
    thed0KtrkHist->GetZaxis()->SetTitleOffset(1.4);
    thez0KtrkHist->GetZaxis()->SetTitleOffset(1.4);
    
    thed0KtrkHist->SetStats(kFALSE);
    thez0KtrkHist->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 1.5;
    theMin = 0.5;

    //if (theMax > 0.24) theMax = 0.3;
    
    thed0KtrkHist->SetMaximum(theMax);
    thed0KtrkHist->SetMinimum(theMin);
    thez0KtrkHist->SetMaximum(theMax);
    thez0KtrkHist->SetMinimum(theMin);

    
    canSigmaz0Ktrk = new TCanvas ("canSigmaz0Ktrk","canSigmaz0Ktrk", 900,600);
    canSigmaz0Ktrk->cd();
    PaletteForK->Exec(); // set palette
    thez0KtrkHist->Draw("colz");
    canSigmaz0Ktrk->Print("Sigmaz0Ktrk.png");

    cand0Ktrk = new TCanvas ("cand0Ktrk","cand0Ktrk", 900,600);
    cand0Ktrk->cd();
    thed0KtrkHist->Draw("colz");
    cand0Ktrk->Print("Sigmad0Ktrk.png");
  }

  PaletteForChi2->Exec(); // set palette
  /////////////////////////////////////////////////////////////////
  if (thed0Chi2HistR != NULL && thez0Chi2HistR != NULL) {
    thed0Chi2HistR->GetZaxis()->SetTitle("");
    thez0Chi2HistR->GetZaxis()->SetTitle("");
    thed0Chi2HistR->GetZaxis()->SetTitleOffset(1.4);
    thez0Chi2HistR->GetZaxis()->SetTitleOffset(1.4);
    
    thed0Chi2HistR->SetStats(kFALSE);
    thez0Chi2HistR->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 10.;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    thed0Chi2HistR->SetMaximum(theMax);
    thed0Chi2HistR->SetMinimum(theMin);
    thez0Chi2HistR->SetMaximum(theMax);
    thez0Chi2HistR->SetMinimum(theMin);

    
    canChi2z0Reco = new TCanvas ("canChi2z0Reco","canChi2z0Reco", 900,600);
    canChi2z0Reco->cd();
    PaletteForChi2->Exec(); // set palette
    thez0Chi2HistR->Draw("colz");
    canChi2z0Reco->Print("Chi2z0Chi2R.png");

    canChi2d0Reco = new TCanvas ("canChi2d0Reco","canChi2d0Reco", 900,600);
    canChi2d0Reco->cd();
    PaletteForChi2->Exec(); // set palette
    thed0Chi2HistR->Draw("colz");
    canChi2d0Reco->Print("Chi2d0Chi2R.png");

  }

  PaletteForChi2->Exec(); // set palette
  /////////////////////////////////////////////////////////////////
  if (thed0Chi2HistU != NULL && thez0Chi2HistU != NULL) {
    //theHistR->GetZaxis()->SetTitle("Reco #sigma_{z0} [mm]");
    //theHistU->GetZaxis()->SetTitle("Unfolded #sigma_{z0} [mm]");
    thed0Chi2HistU->GetZaxis()->SetTitle("");
    thez0Chi2HistU->GetZaxis()->SetTitle("");
    thed0Chi2HistU->GetZaxis()->SetTitleOffset(1.4);
    thez0Chi2HistU->GetZaxis()->SetTitleOffset(1.4);
    
    thed0Chi2HistU->SetStats(kFALSE);
    thez0Chi2HistU->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 10.;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    thed0Chi2HistU->SetMaximum(theMax);
    thed0Chi2HistU->SetMinimum(theMin);
    thez0Chi2HistU->SetMaximum(theMax);
    thez0Chi2HistU->SetMinimum(theMin);

    
    canChi2z0Unfold = new TCanvas ("canChi2z0Unfold","canChi2z0Unfold", 900,600);
    canChi2z0Unfold->cd();
    PaletteForChi2->Exec(); // set palette
    thez0Chi2HistU->Draw("colz");
    canChi2z0Unfold->Print("Chi2z0Chi2U.png");

    canChi2d0Unfold = new TCanvas ("canChi2d0Unfold","canChi2d0Unfold", 900,600);
    canChi2d0Unfold->cd();
    PaletteForChi2->Exec(); // set palette
    thed0Chi2HistU->Draw("colz");
    canChi2d0Unfold->Print("Chi2d0Chi2U.png");

  }

  PaletteForSigma->Exec();
  theHistR = (TH2F*)inputHistosFile->Get("histz0_sigma_reco"); 
  theHistU = (TH2F*)inputHistosFile->Get("histz0_sigma_unfold"); 
  if (theHistR != NULL && theHistU != NULL) {
    theHistR->GetZaxis()->SetTitle("Reco #sigma_{z0} [mm]");
    theHistU->GetZaxis()->SetTitle("Unfolded #sigma_{z0} [mm]");
    theHistR->GetZaxis()->SetTitleOffset(1.4);
    theHistU->GetZaxis()->SetTitleOffset(1.4);

    theHistR->SetStats(kFALSE);
    theHistU->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.3;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    theHistR->SetMaximum(theMax);
    theHistR->SetMinimum(theMin);
    theHistU->SetMaximum(theMax);
    theHistU->SetMinimum(theMin);

    canSigmaz0Reco = new TCanvas ("canSigmaz0Reco","canSigmaz0Reco", 900,600);
    canSigmaz0Reco->cd();
    theHistR->Draw("colz");
    canSigmaz0Reco->Print("Sigmaz0Reco.png");

    canSigmaz0Unfo = new TCanvas ("canSigmaz0Unfo","canSigmaz0Unfo", 900,600);
    canSigmaz0Unfo->cd();
    theHistU->Draw("colz");
    canSigmaz0Unfo->Print("Sigmaz0Unfo.png");
  }
  PaletteForSigma->Exec();

  //return;

  if (theOldMCHist != NULL && theOldDataHist != NULL) {
    theOldMCHist->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theOldDataHist->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theOldDataHist->GetZaxis()->SetTitleOffset(1.4);
    theOldMCHist->GetZaxis()->SetTitleOffset(1.4);
    theOldDataHist->GetXaxis()->SetTitleOffset(1.0);
    theOldMCHist->GetXaxis()->SetTitleOffset(1.0);
    theOldDataHist->GetYaxis()->SetTitleOffset(1.0);
    theOldMCHist->GetYaxis()->SetTitleOffset(1.0);

    theOldMCHist->GetYaxis()->SetTitle("#eta");
    theOldDataHist->GetYaxis()->SetTitle("#eta");
    theOldMCHist->GetXaxis()->SetTitle("p_{T} [GeV]");
    theOldDataHist->GetXaxis()->SetTitle("p_{T} [GeV]");
    theOldMCHist->SetTitle("Unfolded d_{0}");
    theOldDataHist->SetTitle("Unfolded d_{0}");

    theOldDataHist->SetStats(kFALSE);
    theOldMCHist->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.03;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    theOldMCHist->SetMaximum(theMax);
    theOldMCHist->SetMinimum(theMin);
    theOldMCHist->GetXaxis()->SetRangeUser(15,250);
    theOldMCHist->SetMinimum(theMin);
    theOldDataHist->SetMaximum(theMax);
    theOldDataHist->SetMinimum(theMin);
    theOldDataHist->GetXaxis()->SetRangeUser(15,250);

    canSigmad0UnfoOldMC = new TCanvas ("canSigmad0UnfoOldMC","canSigmad0UnfoOldMC", 900,600);
    canSigmad0UnfoOldMC->cd();
    theOldMCHist->Draw("colz");
    canSigmad0UnfoOldMC->Print("Sigmad0UnfoOldMC.png");

    canSigmad0UnfoOldData = new TCanvas ("canSigmad0UnfoOldData","canSigmad0UnfoOldData", 900,600);
    canSigmad0UnfoOldData->cd();
    theOldDataHist->Draw("colz");
    canSigmad0UnfoOldData->Print("Sigmad0UnfoOldData.png");
  }
  if (theOldMCHistz0 != NULL && theOldDataHistz0 != NULL) {
    theOldMCHistz0->GetZaxis()->SetTitle("Unfolded #sigma_{z0} [mm]");
    theOldDataHistz0->GetZaxis()->SetTitle("Unfolded #sigma_{z0} [mm]");
    theOldDataHistz0->GetZaxis()->SetTitleOffset(1.4);
    theOldMCHistz0->GetZaxis()->SetTitleOffset(1.4);
    theOldDataHistz0->GetXaxis()->SetTitleOffset(1.0);
    theOldMCHistz0->GetXaxis()->SetTitleOffset(1.0);
    theOldDataHistz0->GetYaxis()->SetTitleOffset(1.0);
    theOldMCHistz0->GetYaxis()->SetTitleOffset(1.0);

    theOldMCHistz0->GetYaxis()->SetTitle("#eta");
    theOldDataHistz0->GetYaxis()->SetTitle("#eta");
    theOldMCHistz0->GetXaxis()->SetTitle("p_{T} [GeV]");
    theOldDataHistz0->GetXaxis()->SetTitle("p_{T} [GeV]");
    theOldMCHistz0->SetTitle("Unfolded z_{0}");
    theOldDataHistz0->SetTitle("Unfolded z_{0}");

    theOldDataHistz0->SetStats(kFALSE);
    theOldMCHistz0->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.3;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    theOldMCHistz0->SetMaximum(theMax);
    theOldMCHistz0->SetMinimum(theMin);
    theOldMCHistz0->GetXaxis()->SetRangeUser(15,250);
    theOldMCHistz0->SetMinimum(theMin);
    theOldDataHistz0->SetMaximum(theMax);
    theOldDataHistz0->SetMinimum(theMin);
    theOldDataHistz0->GetXaxis()->SetRangeUser(15,250);

    canSigmad0UnfoOldMCz0 = new TCanvas ("canSigmad0UnfoOldMCz0","canSigmad0UnfoOldMCz0", 900,600);
    canSigmad0UnfoOldMCz0->cd();
    theOldMCHistz0->Draw("colz");
    canSigmad0UnfoOldMCz0->Print("Sigmad0UnfoOldMCz0.png");

    canSigmad0UnfoOldDataz0 = new TCanvas ("canSigmad0UnfoOldDataz0","canSigmad0UnfoOldDataz0", 900,600);
    canSigmad0UnfoOldDataz0->cd();
    theOldDataHistz0->Draw("colz");
    canSigmad0UnfoOldDataz0->Print("Sigmad0UnfoOldDataz0.png");
  }

  theHistP = (TH2F*)inputHistosFile->Get("histz0_sigma_pos_unfold"); //reco_pos
  theHistN = (TH2F*)inputHistosFile->Get("histz0_sigma_neg_unfold"); //reco_neg
  if (theHistR != NULL && theHistU != NULL) {
    //theHistP->GetZaxis()->SetTitle("#mu^{+}: Reco #sigma_{d0} [mm]");
    //theHistN->GetZaxis()->SetTitle("#mu^{-}: Reco #sigma_{d0} [mm]");

    theHistP->GetZaxis()->SetTitle("#mu^{+}: Unfold #sigma_{z0} [mm]");
    theHistN->GetZaxis()->SetTitle("#mu^{-}: Unfold #sigma_{z0} [mm]");
    theHistP->GetZaxis()->SetTitleOffset(1.4);
    theHistN->GetZaxis()->SetTitleOffset(1.4);

    theHistP->SetStats(kFALSE);
    theHistN->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.3;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    theHistP->SetMaximum(theMax);
    theHistP->SetMinimum(theMin);
    theHistN->SetMaximum(theMax);
    theHistN->SetMinimum(theMin);

    canSigmaz0UnfoldPos = new TCanvas ("canSigmaz0UnfoldPos","canSigmaz0UnfoldPos", 900,600);
    canSigmaz0UnfoldPos->cd();
    theHistP->Draw("colz");
    canSigmaz0UnfoldPos->Print("Sigmaz0UnfoldPos.png");//RecoPos

    canSigmaz0UnfoldNeg = new TCanvas ("canSigmaz0UnfoldNeg","canSigmaz0UnfoldNeg", 900,600);
    canSigmaz0UnfoldNeg->cd();
    theHistN->Draw("colz");
    canSigmaz0UnfoldNeg->Print("Sigmaz0UnfoldNeg.png");//RecoNeg
  }
 
  theHistP = (TH2F*)inputHistosFile->Get("histz0_sigma_reco_pos"); //reco_pos
  theHistN = (TH2F*)inputHistosFile->Get("histz0_sigma_reco_neg"); //reco_neg
  if (theHistR != NULL && theHistU != NULL) {
    theHistP->GetZaxis()->SetTitle("#mu^{+}: Reco #sigma_{z0} [mm]");
    theHistN->GetZaxis()->SetTitle("#mu^{-}: Reco #sigma_{z0} [mm]");

    //theHistP->GetZaxis()->SetTitle("#mu^{+}: Unfold #sigma_{z0} [mm]");
    //theHistN->GetZaxis()->SetTitle("#mu^{-}: Unfold #sigma_{z0} [mm]");

    theHistP->SetStats(kFALSE);
    theHistN->SetStats(kFALSE);
    theHistP->GetZaxis()->SetTitleOffset(1.4);
    theHistN->GetZaxis()->SetTitleOffset(1.4);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.3;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    theHistP->SetMaximum(theMax);
    theHistP->SetMinimum(theMin);
    theHistN->SetMaximum(theMax);
    theHistN->SetMinimum(theMin);

    canSigmaz0RecoPos = new TCanvas ("canSigmaz0RecoPos","canSigmaz0RecoPos", 900,600);
    canSigmaz0RecoPos->cd();
    theHistP->Draw("colz");
    canSigmaz0RecoPos->Print("Sigmaz0RecoPos.png");//RecoPos

    canSigmaz0RecoNeg = new TCanvas ("canSigmaz0RecoNeg","canSigmaz0RecoNeg", 900,600);
    canSigmaz0RecoNeg->cd();
    theHistN->Draw("colz");
    canSigmaz0RecoNeg->Print("Sigmaz0RecoNeg.png");//RecoNeg
  }


  theHistR = (TH2F*)inputHistosFile->Get("histd0_sigma_reco"); 
  theHistU = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold"); 
  if (theHistR != NULL && theHistU != NULL) {

    theHistR->GetZaxis()->SetTitle("Reco #sigma_{d0} [mm]");
    theHistU->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theHistR->GetZaxis()->SetTitleOffset(1.4);
    theHistU->GetZaxis()->SetTitleOffset(1.4);
     
    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.03;
    theMin = 0.;

    //if (theMax > 0.03) theMax = 0.03;

    theHistR->SetStats(kFALSE);
    theHistU->SetStats(kFALSE);

    theHistR->SetMaximum(theMax);
    theHistR->SetMinimum(theMin);
    theHistU->SetMaximum(theMax);
    theHistU->SetMinimum(theMin);

    canSigmad0Reco = new TCanvas ("canSigmad0Reco","canSigmad0Reco", 900,600);
    canSigmad0Reco->cd();
    theHistR->Draw("colz");
    canSigmad0Reco->Print("Sigmad0Reco.png");

    canSigmad0Unfo = new TCanvas ("canSigmad0Unfo","canSigmad0Unfo", 900,600);
    canSigmad0Unfo->cd();
    theHistU->Draw("colz");
    canSigmad0Unfo->Print("Sigmad0Unfo.png");

  }


  theHistR = (TH2F*)inputHistosFile->Get("histd0_mean"); 
  theHistU = (TH2F*)inputHistosFile->Get("histd0_mean_unfold"); 
  if (theHistR != NULL && theHistU != NULL) {
    gStyle->SetPalette(1);

    theHistR->SetTitle("Reco mean d_{0} map");
    theHistU->SetTitle("Unfolded mean d_{0} map");

    theHistR->GetZaxis()->SetTitle("Reco mean d_{0} [mm]");
    theHistU->GetZaxis()->SetTitle("Unfolded mean d_{0} [mm]");
    theHistR->GetZaxis()->SetTitleOffset(1.4);
    theHistU->GetZaxis()->SetTitleOffset(1.4);

    double value_mean_R = 0.;
    double value_mean_U = 0.;

    /*for (int etabin = 1; etabin <= theHistR->GetYaxis()->GetNbins(); etabin++) {
       for (int ptbin = 1; ptbin <= theHistR->GetYaxis()->GetNbins(); ptbin++) {
          value_mean_R = theHistR->GetBinContent(ptbin,etabin);
          theHistR->SetBinContent(ptbin,etabin, fabs(value_mean_R));
          value_mean_U = theHistU->GetBinContent(ptbin,etabin);
          theHistU->SetBinContent(ptbin,etabin, fabs(value_mean_U));
       }
    }*/

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.015;
    theMin = -theMax;

    //if (theMax > 0.03) theMax = 0.03;

    theHistR->SetStats(kFALSE);
    theHistU->SetStats(kFALSE);

    theHistR->SetMaximum(theMax);
    theHistR->SetMinimum(theMin);
    theHistU->SetMaximum(theMax);
    theHistU->SetMinimum(theMin);

    canSigmad0RecoMean = new TCanvas ("canSigmad0RecoMean","canSigmad0RecoMean", 900,600);
    canSigmad0RecoMean->cd();
    theHistR->Draw("colz");
    canSigmad0RecoMean->Print("Sigmad0RecoMean.png");

    canSigmad0UnfoMean = new TCanvas ("canSigmad0UnfoMean","canSigmad0UnfoMean", 900,600);
    canSigmad0UnfoMean->cd();
    theHistU->Draw("colz");
    canSigmad0UnfoMean->Print("Sigmad0UnfoMean.png");

  }


  theHistR = (TH2F*)inputHistosFile->Get("histz0_mean"); 
  theHistU = (TH2F*)inputHistosFile->Get("histz0_mean_unfold"); 
  if (theHistR != NULL && theHistU != NULL) {
    gStyle->SetPalette(1);

    theHistR->SetTitle("Reco mean z_{0} map");
    theHistU->SetTitle("Unfolded mean z_{0} map");

    theHistR->GetZaxis()->SetTitle("Reco mean z_{0} [mm]");
    theHistU->GetZaxis()->SetTitle("Unfolded mean z_{0} [mm]");
    theHistR->GetZaxis()->SetTitleOffset(1.4);
    theHistU->GetZaxis()->SetTitleOffset(1.4);

    double value_mean_R = 0.;
    double value_mean_U = 0.;

   /* for (int etabin = 1; etabin <= theHistR->GetYaxis()->GetNbins(); etabin++) {
       for (int ptbin = 1; ptbin <= theHistR->GetYaxis()->GetNbins(); ptbin++) {
          value_mean_R = theHistR->GetBinContent(ptbin,etabin);
          theHistR->SetBinContent(ptbin,etabin, fabs(value_mean_R));
          value_mean_U = theHistU->GetBinContent(ptbin,etabin);
          theHistU->SetBinContent(ptbin,etabin, fabs(value_mean_U));
       }
    }*/

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.3;
    theMin = -theMax;

    //if (theMax > 0.03) theMax = 0.03;

    theHistR->SetStats(kFALSE);
    theHistU->SetStats(kFALSE);

    theHistR->SetMaximum(theMax);
    theHistR->SetMinimum(theMin);
    theHistU->SetMaximum(theMax);
    theHistU->SetMinimum(theMin);

    canSigmaz0RecoMean = new TCanvas ("canSigmaz0RecoMean","canSigmaz0RecoMean", 900,600);
    canSigmaz0RecoMean->cd();
    theHistR->Draw("colz");
    canSigmaz0RecoMean->Print("Sigmaz0RecoMean.png");

    canSigmaz0UnfoMean = new TCanvas ("canSigmaz0UnfoMean","canSigmaz0UnfoMean", 900,600);
    canSigmaz0UnfoMean->cd();
    theHistU->Draw("colz");
    canSigmaz0UnfoMean->Print("Sigmaz0UnfoMean.png");

  }
 
  ////
  PaletteForSigma->Exec();
  

  theHistP = (TH2F*)inputHistosFile->Get("histd0_sigma_pos_unfold"); //reco_pos
  theHistN = (TH2F*)inputHistosFile->Get("histd0_sigma_neg_unfold"); //reco_neg
  if (theHistR != NULL && theHistU != NULL) {
    //theHistP->GetZaxis()->SetTitle("#mu^{+}: Reco #sigma_{d0} [mm]");
    //theHistN->GetZaxis()->SetTitle("#mu^{-}: Reco #sigma_{d0} [mm]");

    theHistP->GetZaxis()->SetTitle("#mu^{+}: Unfold #sigma_{d0} [mm]");
    theHistN->GetZaxis()->SetTitle("#mu^{-}: Unfold #sigma_{d0} [mm]");
    theHistP->GetZaxis()->SetTitleOffset(1.4);
    theHistN->GetZaxis()->SetTitleOffset(1.4);

    theHistP->SetStats(kFALSE);
    theHistN->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.03;
    theMin = 0.;

    //if (theMax > 0.03) theMax = 0.03;//0.025
    
    theHistP->SetMaximum(theMax);
    theHistP->SetMinimum(theMin);
    theHistN->SetMaximum(theMax);
    theHistN->SetMinimum(theMin);

    canSigmad0UnfoldPos = new TCanvas ("canSigmad0UnfoldPos","canSigmad0UnfoldPos", 900,600);
    canSigmad0UnfoldPos->cd();
    theHistP->Draw("colz");
    canSigmad0UnfoldPos->Print("Sigmad0UnfoldPos.png");//RecoPos

    canSigmad0UnfoldNeg = new TCanvas ("canSigmad0UnfoldNeg","canSigmad0UnfoldNeg", 900,600);
    canSigmad0UnfoldNeg->cd();
    theHistN->Draw("colz");
    canSigmad0UnfoldNeg->Print("Sigmad0UnfoldNeg.png");//RecoNeg
  }

  theHistP = (TH2F*)inputHistosFile->Get("histd0_sigma_reco_pos"); //reco_pos
  theHistN = (TH2F*)inputHistosFile->Get("histd0_sigma_reco_neg"); //reco_neg
  if (theHistR != NULL && theHistU != NULL) {
    theHistP->GetZaxis()->SetTitle("#mu^{+}: Reco #sigma_{d0} [mm]");
    theHistN->GetZaxis()->SetTitle("#mu^{-}: Reco #sigma_{d0} [mm]");
    theHistP->GetZaxis()->SetTitleOffset(1.4);
    theHistN->GetZaxis()->SetTitleOffset(1.4);

    //theHistP->GetZaxis()->SetTitle("#mu^{+}: Unfold #sigma_{z0} [mm]");
    //theHistN->GetZaxis()->SetTitle("#mu^{-}: Unfold #sigma_{z0} [mm]");

    theHistP->SetStats(kFALSE);
    theHistN->SetStats(kFALSE);

    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.03;
    theMin = 0.;

    //if (theMax > 0.24) theMax = 0.3;
    
    theHistP->SetMaximum(theMax);
    theHistP->SetMinimum(theMin);
    theHistN->SetMaximum(theMax);
    theHistN->SetMinimum(theMin);

    canSigmad0RecoPos = new TCanvas ("canSigmad0RecoPos","canSigmad0RecoPos", 900,600);
    canSigmad0RecoPos->cd();
    theHistP->Draw("colz");
    canSigmad0RecoPos->Print("Sigmad0RecoPos.png");//RecoPos

    canSigmad0RecoNeg = new TCanvas ("canSigmad0RecoNeg","canSigmad0RecoNeg", 900,600);
    canSigmad0RecoNeg->cd();
    theHistN->Draw("colz");
    canSigmad0RecoNeg->Print("Sigmad0RecoNeg.png");//RecoNeg
  }


  if (theHistP != NULL && theHistN != NULL) {
    double etaValue = 0.5;
    int binEta = theHistR->GetYaxis()->FindBin(etaValue);

    h_projR = theHistP->ProjectionX("_px_pos", binEta, binEta); 
    h_projU = theHistN->ProjectionX("_px_neg", binEta, binEta); 

    h_projR->SetStats(kFALSE);
    h_projU->SetStats(kFALSE);

    h_projR->SetLineColor(kGray+3);
    h_projU->SetLineColor(kRed+1);

    h_projR->SetLineWidth(2);
    h_projU->SetLineWidth(2);

    //theMax = max(h_projR->GetMaximum(), h_projU->GetMaximum())*1.1;
    theMax = 0.015;
    theMin = 0.;

    //if (theMax > 0.03) theMax = 0.03; //0.025
    
    h_projR->SetMaximum(theMax);
    h_projR->SetMinimum(theMin);
    h_projU->SetMaximum(theMax);
    h_projU->SetMinimum(theMin);

    h_projR->SetYTitle(theHistP->GetZaxis()->GetTitle());

    canProjSigmad0 = new TCanvas ("canProjSigmad0","canProjSigmad0", 900,600);
    canProjSigmad0->cd();
    h_projR->Draw();
    h_projU->Draw("same");
    TLegend * theLegend = new TLegend(0.6, 0.7, 0.8, 0.85);
    theLegend->AddEntry(h_projR,"#mu^{+}");
    theLegend->AddEntry(h_projU,"#mu^{-}");
    theLegend->Draw();
    TString theComment;
    theComment.Form("#eta= %3.2f",etaValue);
    TLatex theCommentText;
    theCommentText.DrawLatexNDC(0.70, 0.91, theComment.Data());
    canProjSigmad0->Print("ProjSigmad0.png");
  }


  theHistKup = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold_kpvup"); 
  theHistKdown = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold_kpvdown"); 
  if (theHistR != NULL && theHistU != NULL) {

    theHistKup->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theHistKdown->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theHistKup->GetZaxis()->SetTitleOffset(1.4);
    theHistKdown->GetZaxis()->SetTitleOffset(1.4);
     
    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.03;
    theMin = 0.;

    //if (theMax > 0.03) theMax = 0.03;

    theHistKup->SetStats(kFALSE);
    theHistKdown->SetStats(kFALSE);

    theHistKup->SetMaximum(theMax);
    theHistKup->SetMinimum(theMin);
    theHistKdown->SetMaximum(theMax);
    theHistKdown->SetMinimum(theMin);

    canSigmad0UnfoKPVup = new TCanvas ("canSigmad0UnfoKPVup","canSigmad0KPVup", 900,600);
    canSigmad0UnfoKPVup->cd();
    theHistKup->Draw("colz");
    canSigmad0UnfoKPVup->Print("Sigmad0UnfoKPVup.png");

    canSigmad0UnfoKPVdown = new TCanvas ("canSigmad0UnfoKPVdown","canSigmad0UnfoKPVdown", 900,600);
    canSigmad0UnfoKPVdown->cd();
    theHistKdown->Draw("colz");
    canSigmad0UnfoKPVdown->Print("Sigmad0UnfoKPVdown.png");

  }

  theHistKup = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold_ktrkup"); 
  theHistKdown = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold_ktrkdown"); 
  if (theHistKup != NULL && theHistKdown != NULL) {

    theHistKup->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theHistKdown->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theHistKup->GetZaxis()->SetTitleOffset(1.4);
    theHistKdown->GetZaxis()->SetTitleOffset(1.4);
     
    //theMax = max(theHistR->GetMaximum(), theHistU->GetMaximum());
    theMax = 0.03;
    theMin = 0.;

    //if (theMax > 0.03) theMax = 0.03;

    theHistKup->SetStats(kFALSE);
    theHistKdown->SetStats(kFALSE);

    theHistKup->SetMaximum(theMax);
    theHistKup->SetMinimum(theMin);
    theHistKdown->SetMaximum(theMax);
    theHistKdown->SetMinimum(theMin);

    canSigmad0UnfoKtrkup = new TCanvas ("canSigmad0UnfoKtrkup","canSigmad0Ktrkup", 900,600);
    canSigmad0UnfoKtrkup->cd();
    theHistKup->Draw("colz");
    canSigmad0UnfoKtrkup->Print("Sigmad0UnfoKtrkup.png");

    canSigmad0UnfoKtrkdown = new TCanvas ("canSigmad0UnfoKtrkdown","canSigmad0UnfoKtrkdown", 900,600);
    canSigmad0UnfoKtrkdown->cd();
    theHistKdown->Draw("colz");
    canSigmad0UnfoKtrkdown->Print("Sigmad0UnfoKtrkdown.png");

  }
 
  // compare narrow and wide Gaussian fit
  theHistU = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold"); 
  theHistR = (TH2F*)inputHistosFile->Get("histd0_sigma_unfold_widerfit"); 

  if (theHistR != NULL && theHistU != NULL) {
    theHistR->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    theHistU->GetZaxis()->SetTitle("Unfolded #sigma_{d0} [mm]");
    
    double etaValue = 0.5;
    int binEta = theHistR->GetYaxis()->FindBin(etaValue); std::cout << " projection for eta= " << etaValue << " bin = " << binEta << std::endl; 

    h_projR = theHistR->ProjectionX("_Rpx_nvsw", binEta, binEta); 
    h_projU = theHistU->ProjectionX("_Upx_nvsw", binEta, binEta); 

    h_projR->SetStats(kFALSE);
    h_projU->SetStats(kFALSE);

    h_projR->SetLineColor(kRed+1); 
    h_projU->SetLineColor(kGray+3); 

    h_projR->SetLineWidth(2);
    h_projU->SetLineWidth(2);

    //theMax = max(h_projR->GetMaximum(), h_projU->GetMaximum())*1.1;
    theMax = 0.015;
    theMin = 0.;

    //if (theMax > 0.03) theMax = 0.03; //0.025
    
    h_projR->SetMaximum(theMax);
    h_projR->SetMinimum(theMin);
    h_projU->SetMaximum(theMax);
    h_projU->SetMinimum(theMin);

    h_projR->GetYaxis()->SetTitle(theHistR->GetZaxis()->GetTitle());
    h_projU->GetYaxis()->SetTitle(theHistU->GetZaxis()->GetTitle());

    canProjSigmad0NvsW = new TCanvas ("canProjSigmad0NvsW","canProjSigmad0NvsW", 900,600);
    canProjSigmad0NvsW->cd();
    h_projR->Draw();
    h_projU->Draw("same");
    TLegend * theLegend = new TLegend(0.6, 0.7, 0.8, 0.85);
    theLegend->AddEntry(h_projU,"nominal fit");
    theLegend->AddEntry(h_projR,"wider #sigma fit");
    theLegend->Draw();
    canProjSigmad0NvsW->Print("ProjSigmad0NvsW.png");
  }
  else{
    if (theHistR == NULL) std::cout << " theHistR failed " << std::endl; 
    if (theHistU == NULL) std::cout << " theHistU failed " << std::endl;
  }


 return;
}

////////////////////////////////////////
void BuildPalettes(Int_t colorstyle)
{
  if (colorstyle == 0) {
    const Int_t NCont = 100;
    const Int_t NRGBs = 6;
    Double_t stops[NRGBs] = {0.09, 0.24, 0.35, 0.60, 0.80, 1.00};
    Double_t red[NRGBs] =   {0.00, 0.00, 1.00, 0.99, 0.55, 0.20};
    Double_t green[NRGBs] = {0.10, 1.00, 1.00, 0.10, 0.15, 0.04};
    Double_t blue[NRGBs] =  {0.00, 0.00, 0.10, 0.10, 0.99, 0.40};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont); 
    gStyle->SetNumberContours(99);
    std::cout << " -- Palette 0 built -- " << std::endl;
  }
  if (colorstyle == 1) {
    const Int_t NCont = 100;
    const Int_t NRGBs = 6;
    Double_t stops[NRGBs] = {0.10, 0.45, 0.55, 0.65, 0.90, 1.00};
    Double_t red[NRGBs] =   {0.00, 0.00, 0.00, 1.00, 1.00, 0.10};
    Double_t green[NRGBs] = {0.00, 1.00, 1.00, 1.00, 0.00, 0.00};
    Double_t blue[NRGBs] =  {0.10, 1.00, 0.00, 0.00, 0.00, 0.00};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont); 
    gStyle->SetNumberContours(99);
    std::cout << " -- Palette 1 built -- " << std::endl;
  }
  return;
}
