//
#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TLine.h"
#include "TProfile.h"
#include "TString.h"
#include "TStyle.h"
#include "TText.h"
#include "TExec.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TPad.h"

TCanvas* radialCanvas;
TCanvas* radialetaCanvas;
TCanvas* epsilonCanvas;
//TCanvas* radialetaallCanvas;

TH1F* theHist;
TH1F* theHist2;
TH1F* theHist3;
TH1F* theMCHist;

TH2F* theRadialHist;

TH3F* theTH3Hist;
//TH2F* theECAHist_2;
TH2F* thePtEtaHist;
TH2F* thePtEtaHistECC;
TH2F* thePtEtaHistBar;
TH2F* thePtEtaHistECA;
TH1F* thePtEtaProfileAll;
TH1F* thePtEtaProfileECC;
TH1F* thePtEtaProfileBar;
TH1F* thePtEtaProfileECA;

TH1F* theEtaProfileAllPt;
TH1F* theEtaProfileAllPtRun1; TH1F *theEtaProfileAllPtRun2; TH1F *theEtaProfileAllPtRun3;

TProfile* theProf;
void   BuildPalettes (Int_t colorstyle=0);

float theMax = -9999., theMin = -9999.; // default values
/////////////////////////////////////
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


void IntegratedRadialDistortion()
{
  TString theFileName("ZmumuValidationExampleOutput_oneEpsilon.root");
  TFile *inputHistosFile = TFile::Open(theFileName.Data());
  theRadialHist = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_etaphi_Integrated"); 
  if (theRadialHist != NULL) {
    theRadialHist->GetZaxis()->SetTitle("#epsilon");
    gStyle->SetPadRightMargin(0.15);
    gStyle->SetPadTopMargin(0.12);
    epsilonCanvas = new TCanvas ("epsilonCanvas","epsilonCanvas", 900,600);
    epsilonCanvas->cd();
    gStyle->SetPalette(1);
    theRadialHist->SetStats(kFALSE);
    theRadialHist->GetZaxis()->SetTitleOffset(1.4);
    theRadialHist->GetZaxis()->SetTitle("#epsilon = #delta R / R");
    theRadialHist->SetMaximum(0.002); theRadialHist->SetMinimum(-0.002);
    theRadialHist->Draw("colz text");
    TLatex theText;
    theText.SetTextColor(kGray+1);
    theText.SetTextAlign(31);
    theText.SetTextSize(0.03);
    theText.DrawLatexNDC(0.85, 0.90, theFileName.Data()); 
    epsilonCanvas->Print("dimuon_epsilon.png");
  }
  return;
}

void theRadialCorrectionPlots()
{
  //TFile *inputHistosFile = TFile::Open("Zmumu_2015_10x9.root");
  //TFile *inputHistosFile = TFile::Open("Zmumu_2016_5x6_normal.root");
  TFile *inputHistosFile  = TFile::Open("ZmumuValidationExampleOutput_oneEpsilon_001.root");
  TFile *inputHistosFile2 = TFile::Open("ZmumuValidationExampleOutput_twoEpsilon_001.root");
  TFile *inputHistosFile3 = TFile::Open("ZmumuValidationExampleOutput_oneEpsilon.root");
  //TFile *inputHistosFile3 = TFile::Open("ZmumuValidationExampleOutput_test_MCv2.root");
  TFile *inputHistosFileMC = TFile::Open("ZmumuValidationExampleOutput_twoEpsilon.root");

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadTopMargin(0.10);
  
  //init text
  TText theText;
  theText.SetTextColor(kGray+2);
  TGraph * thePoints = new TGraph ();

  //
  //TExec *PaletteForChi2 = new TExec("PaletteForChi2","BuildPalettes(0);");

  //TFile *inputHistosFile3 = TFile::Open("ZmumuValidationExampleOutput_test_306310_20_7.root");
  theHist = (TH1F*)inputHistosFile->Get("h_radialdistortion_vs_pt_prof"); 
  if (inputHistosFile2 != NULL) theHist2 = (TH1F*)inputHistosFile2->Get("h_radialdistortion_vs_pt_prof"); 
  if (inputHistosFile3 != NULL) theHist3 = (TH1F*)inputHistosFile3->Get("h_radialdistortion_vs_pt_prof"); 
  if (inputHistosFileMC != NULL) theMCHist = (TH1F*)inputHistosFileMC->Get("h_radialdistortion_vs_pt_prof");

  if (theHist != NULL) {
    theHist->GetYaxis()->SetTitle("#epsilon");
    theHist->GetYaxis()->SetTitleOffset(1.4);
    theHist->GetZaxis()->SetTitle("#delta_{r} [mm]");
    radialCanvas = new TCanvas ("radialCanvas","radialCanvas", 900,600);
    radialCanvas->cd();
    radialCanvas->SetGridx();
    radialCanvas->SetGridy();

    theMax =  0.0015;
    theMin = -0.0015;
    theHist->SetStats(kFALSE);
    theHist->SetMaximum(theMax);
    theHist->SetMinimum(theMin);
    theHist->SetLineColor(kBlue);
    theHist2->SetLineColor(kGreen);
    theHist3->SetLineColor(kRed);
    theMCHist->SetLineColor(kBlack);
    theHist->SetMarkerSize(0.7);
    theHist2->SetMarkerSize(0.7);
    theHist3->SetMarkerSize(0.7);
    theMCHist->SetMarkerSize(0.7);
    theHist->SetLineWidth(1);
    theHist2->SetLineWidth(1);
    theHist3->SetLineWidth(1);
    theMCHist->SetLineWidth(1);
    theHist->Draw("");
    theHist2->Draw("same");
    theHist3->Draw("same");
    theMCHist->Draw("same");
    TLine *l=new TLine(0,0.0,50,0.0);
    l->SetLineColor(kBlack);
    l->SetLineWidth(2);
    l->Draw("same"); 
    leg = new TLegend(0.12,0.70,0.24,0.88);
    //leg->SetHeader("The Legend Title","C"); // option "C" allows to center the header
    leg->SetHeader("J/#psi MC");
    leg->AddEntry(theHist,"single #epsilon 0.001 ");
    leg->AddEntry(theHist2," ");
    leg->AddEntry(theHist3,"single #epsilon");
    leg->AddEntry(theMCHist,"#epsilon^{+} and #epsilon^{-} ");
    leg->Draw();
    radialCanvas->Print("radialplot.png");
    //theMax = theHist->GetMaximum();
    //theMin = theHist->GetMinimum();
  }


  std::cout << endl << "    ** thePlots ** Game Over. Insert coin ** " << std::endl << std::endl;
  theHist->Reset(); theHist2->Reset(); theHist3->Reset(); theMCHist->Reset();
  inputHistosFile->Close(); inputHistosFile2->Close(); inputHistosFile3->Close(); inputHistosFileMC->Close();
  return;
}

void theRadialCorrectionEtaPlots()
{

  TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_oneEpsilon.root");//MC

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
   
  thePtEtaProfileAll = (TH1F*)inputHistosFile->Get("h_radialdistortion_vs_pt_prof");
  thePtEtaHistECC = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta0");
  thePtEtaHistBar = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta6");
  thePtEtaHistECA = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta13");

std::cout << endl << "    ** thePlots ** M1 ** " << std::endl << std::endl;


  thePtEtaProfileECC =  (TH1F*)thePtEtaHistECC->ProfileX();
  thePtEtaProfileBar =  (TH1F*)thePtEtaHistBar->ProfileX();
  thePtEtaProfileECA =  (TH1F*)thePtEtaHistECA->ProfileX();

std::cout << endl << "    ** thePlots ** M2 ** " << std::endl << std::endl;

  radialetaCanvas = new TCanvas ("radialetaCanvas","radialetaCanvas", 900,600);
  radialetaCanvas->cd();
  radialetaCanvas->SetGridx();
  radialetaCanvas->SetGridy();

  theMax =  0.003;
  theMin = -0.003;
  thePtEtaProfileAll->SetStats(kFALSE);
  thePtEtaProfileAll->SetMaximum(theMax);
  thePtEtaProfileAll->SetMinimum(theMin);
  thePtEtaProfileAll->GetXaxis()->SetTitle("p_{T} [GeV]");
  thePtEtaProfileECC->SetLineColor(kBlue);
  thePtEtaProfileBar->SetLineColor(kGreen);
  thePtEtaProfileECA->SetLineColor(kRed);
  thePtEtaProfileAll->SetLineColor(kBlack);
  thePtEtaProfileAll->SetMarkerSize(0.7);
  thePtEtaProfileAll->SetLineWidth(1);
  thePtEtaProfileAll->Draw();
  thePtEtaProfileECC->Draw("same");
  thePtEtaProfileBar->Draw("same");
  thePtEtaProfileECA->Draw("same");
  leg = new TLegend(0.48,0.7,0.68,0.9);
  //leg->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  leg->AddEntry(thePtEtaProfileECC,"ECC");
  leg->AddEntry(thePtEtaProfileBar,"BAR");
  leg->AddEntry(thePtEtaProfileECA,"ECA");
  leg->AddEntry(thePtEtaProfileAll,"ID");
  leg->Draw();
  radialetaCanvas->Print("radialetaplotMC.png");

  inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test__284285_20_7_v2.root");

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
   
  thePtEtaProfileAll = (TH1F*)inputHistosFile->Get("h_radialdistortion_vs_pt_prof");
  thePtEtaHistECC = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta0");
  thePtEtaHistBar = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta6");
  thePtEtaHistECA = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta13");

std::cout << endl << "    ** thePlots ** M1 ** " << std::endl << std::endl;


  thePtEtaProfileECC =  (TH1F*)thePtEtaHistECC->ProfileX();
  thePtEtaProfileBar =  (TH1F*)thePtEtaHistBar->ProfileX();
  thePtEtaProfileECA =  (TH1F*)thePtEtaHistECA->ProfileX();

std::cout << endl << "    ** thePlots ** M2 ** " << std::endl << std::endl;

  radialetaCanvas = new TCanvas ("radialetaCanvas","radialetaCanvas", 900,600);
  radialetaCanvas->cd();
  radialetaCanvas->SetGridx();
  radialetaCanvas->SetGridy();

  theMax =  0.003;
  theMin = -0.003;
  thePtEtaProfileAll->SetStats(kFALSE);
  thePtEtaProfileAll->SetMaximum(theMax);
  thePtEtaProfileAll->SetMinimum(theMin);
  thePtEtaProfileAll->GetXaxis()->SetTitle("p_{T} [GeV]");
  thePtEtaProfileECC->SetLineColor(kBlue);
  thePtEtaProfileBar->SetLineColor(kGreen);
  thePtEtaProfileECA->SetLineColor(kRed);
  thePtEtaProfileAll->SetLineColor(kBlack);
  thePtEtaProfileAll->SetMarkerSize(0.7);
  thePtEtaProfileAll->SetLineWidth(1);
  thePtEtaProfileAll->Draw();
  thePtEtaProfileECC->Draw("same");
  thePtEtaProfileBar->Draw("same");
  thePtEtaProfileECA->Draw("same");
  leg = new TLegend(0.48,0.7,0.68,0.9);
  //leg->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  leg->AddEntry(thePtEtaProfileECC,"ECC");
  leg->AddEntry(thePtEtaProfileBar,"BAR");
  leg->AddEntry(thePtEtaProfileECA,"ECA");
  leg->AddEntry(thePtEtaProfileAll,"ID");
  leg->Draw();
  radialetaCanvas->Print("radialetaplotRun1.png");

  inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test__304008_20_7_v2.root");

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
   
  thePtEtaProfileAll = (TH1F*)inputHistosFile->Get("h_radialdistortion_vs_pt_prof");
  thePtEtaHistECC = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta0");
  thePtEtaHistBar = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta6");
  thePtEtaHistECA = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta13");

std::cout << endl << "    ** thePlots ** M1 ** " << std::endl << std::endl;


  thePtEtaProfileECC =  (TH1F*)thePtEtaHistECC->ProfileX();
  thePtEtaProfileBar =  (TH1F*)thePtEtaHistBar->ProfileX();
  thePtEtaProfileECA =  (TH1F*)thePtEtaHistECA->ProfileX();

std::cout << endl << "    ** thePlots ** M2 ** " << std::endl << std::endl;

  radialetaCanvas = new TCanvas ("radialetaCanvas","radialetaCanvas", 900,600);
  radialetaCanvas->cd();
  theMax =  0.003;
  theMin = -0.003;
  thePtEtaProfileAll->SetStats(kFALSE);
  thePtEtaProfileAll->SetMaximum(theMax);
  thePtEtaProfileAll->SetMinimum(theMin);
  thePtEtaProfileAll->GetXaxis()->SetTitle("p_{T} [GeV]");
  thePtEtaProfileECC->SetLineColor(kBlue);
  thePtEtaProfileBar->SetLineColor(kGreen);
  thePtEtaProfileECA->SetLineColor(kRed);
  thePtEtaProfileAll->SetLineColor(kBlack);
  thePtEtaProfileAll->SetMarkerSize(0.7);
  thePtEtaProfileAll->SetLineWidth(1);
  thePtEtaProfileAll->Draw();
  thePtEtaProfileECC->Draw("same");
  thePtEtaProfileBar->Draw("same");
  thePtEtaProfileECA->Draw("same");
  leg = new TLegend(0.48,0.7,0.68,0.9);
  //leg->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  leg->AddEntry(thePtEtaProfileECC,"ECC");
  leg->AddEntry(thePtEtaProfileBar,"BAR");
  leg->AddEntry(thePtEtaProfileECA,"ECA");
  leg->AddEntry(thePtEtaProfileAll,"ID");
  leg->Draw();
  radialetaCanvas->Print("radialetaplotRun2.png");

  inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test__306310_20_7_v2.root");

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
   
  thePtEtaProfileAll = (TH1F*)inputHistosFile->Get("h_radialdistortion_vs_pt_prof");
  thePtEtaHistECC = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta0");
  thePtEtaHistBar = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta6");
  thePtEtaHistECA = (TH2F*)inputHistosFile->Get("h_radialdistortion_vs_ptEta13");

std::cout << endl << "    ** thePlots ** M1 ** " << std::endl << std::endl;


  thePtEtaProfileECC =  (TH1F*)thePtEtaHistECC->ProfileX();
  thePtEtaProfileBar =  (TH1F*)thePtEtaHistBar->ProfileX();
  thePtEtaProfileECA =  (TH1F*)thePtEtaHistECA->ProfileX();

std::cout << endl << "    ** thePlots ** M2 ** " << std::endl << std::endl;

  radialetaCanvas = new TCanvas ("radialetaCanvas","radialetaCanvas", 900,600);
  radialetaCanvas->cd();
  radialetaCanvas->SetGridx();
  radialetaCanvas->SetGridy();

  theMax =  0.003;
  theMin = -0.003;
  thePtEtaProfileAll->SetStats(kFALSE);
  thePtEtaProfileAll->SetMaximum(theMax);
  thePtEtaProfileAll->SetMinimum(theMin);
  thePtEtaProfileAll->GetXaxis()->SetTitle("p_{T} [GeV]");
  thePtEtaProfileECC->SetLineColor(kBlue);
  thePtEtaProfileBar->SetLineColor(kGreen);
  thePtEtaProfileECA->SetLineColor(kRed);
  thePtEtaProfileAll->SetLineColor(kBlack);
  thePtEtaProfileAll->SetMarkerSize(0.7);
  thePtEtaProfileAll->SetLineWidth(1);
  thePtEtaProfileAll->Draw();
  thePtEtaProfileECC->Draw("same");
  thePtEtaProfileBar->Draw("same");
  thePtEtaProfileECA->Draw("same");
  leg = new TLegend(0.48,0.7,0.68,0.9);
  //leg->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  leg->AddEntry(thePtEtaProfileECC,"ECC");
  leg->AddEntry(thePtEtaProfileBar,"BAR");
  leg->AddEntry(thePtEtaProfileECA,"ECA");
  leg->AddEntry(thePtEtaProfileAll,"ID");
  leg->Draw();
  radialetaCanvas->Print("radialetaplotRun3.png");

  std::cout << endl << "    ** theRadialCorrectionEtaPlots ** Game Over. Insert coin ** " << std::endl << std::endl;

  //radialetaCanvas->SetGridx();
  //radialetaCanvas->SetGridy();

  //theMax =  0.0015;
  //theMin = -0.0015;
  //theECAHist_2->SetStats(kFALSE);
  //theECAHist_2->SetMaximum(theMax);
  //theECAHist_2->SetMinimum(theMin);
  //theECAHist_2->SetMarkerSize(0.7);
  //theECAHist_2->SetLineWidth(1);
  //theECAHist_2->Draw("colz");

  //theECAHist_2 = new TH2F("", "", theTH3Hist->GetXaxis()->GetNbins(), theTH3Hist->GetXaxis()->GetXmin(), theTH3Hist->GetXaxis()->GetXmax(),
  //                                theTH3Hist->GetZaxis()->GetNbins(), theTH3Hist->GetZaxis()->GetXmin(), theTH3Hist->GetZaxis()->GetXmax());
  //TH2F* theECAHist_2 = (TH2F*)theTH3Hist->Project3D("xz");
  /*if (theTH3Hist != NULL) {
   for( int epsilon_bin = 0; epsilon_bin < theTH3Hist->GetZaxis()->GetNbins(); epsilon_bin++){
     for( int pt_bin = 0; pt_bin < theTH3Hist->GetXaxis()->GetNbins(); pt_bin++){
       for( int eta_bin = 0; eta_bin <= theTH3Hist->GetYaxis()->GetNbins(); eta_bin++){
         value = theTH3Hist->GetBinContent(pt_bin, eta_bin, epsilon_bin);
         value_epsilon_bin = theTH3Hist->GetZaxis()->GetBinCenter(value);
         value_eta_bin = theTH3Hist->GetYaxis()->GetBinCenter(value);
         value_pt_bin = theTH3Hist->GetXaxis()->GetBinCenter(value);
         cout << " ** theRadialCorrectionEtaPlots ** Epsilon = " << value_epsilon_bin << "   eta value = " << value_eta_bin << endl; 
         if (value_eta_bin < 2.5 && value_eta_bin > -2.5) {
         theECAHist_2->SetBinContent(pt_bin, eta_bin, value_epsilon_bin);
         }
       }
     }
   }
  }*/

  thePtEtaProfileAll->Reset(); thePtEtaProfileECC->Reset(); thePtEtaProfileBar->Reset(); thePtEtaProfileECA->Reset();
  inputHistosFile->Close(); 
  return;
}

void theRadialCorrectionEtaProfile()
{

  //gROOT->Reset();

  TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test__284285_20_7_v2.root");
  TFile *inputHistosFile2 = TFile::Open("ZmumuValidationExampleOutput_test__304008_20_7_v2.root");
  TFile *inputHistosFile3 = TFile::Open("ZmumuValidationExampleOutput_test__306310_20_7_v2.root");
  TFile *inputHistosFileMC = TFile::Open("ZmumuValidationExampleOutput_test_MC.root");
  //TFile *inputHistosFileMC = TFile::Open("ZmumuValidationExampleOutput_test_MCv2.root");

  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.15);
   
  theEtaProfileAllPt = (TH1F*)inputHistosFileMC->Get("h_radialdistortion_vs_eta_prof");
  theEtaProfileAllPtRun1 = (TH1F*)inputHistosFile->Get("h_radialdistortion_vs_eta_prof");
  theEtaProfileAllPtRun2 = (TH1F*)inputHistosFile2->Get("h_radialdistortion_vs_eta_prof");
  theEtaProfileAllPtRun3 = (TH1F*)inputHistosFile3->Get("h_radialdistortion_vs_eta_prof");
  
  //TGaxis* deltaM_axis = new TGaxis(0.1,-0.8,0.9,0.8, 0 , 1);

  TCanvas* radialetaallCanvas = new TCanvas ("radialetaallCanvas","radialetaallCanvas", 900,600);
  radialetaallCanvas->cd();
  radialetaallCanvas->SetGridx();
  radialetaallCanvas->SetGridy();
  
  /*TGaxis* deltaM_axis = new TGaxis(gPad->GetUxmin(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax(), -5., 5., 510);
  deltaM_axis->SetName("deltaM_axis");
  //theMax =  theEtaProfileAllPt->GetMaximum();
  //theMin =  theEtaProfileAllPt->GetMinimum();
  cout << "WELL DEFINED" << deltaM_axis <<   " ,,," << gPad->GetUxmin() << " ,,,, " << gPad->GetUxmax() << endl;
  deltaM_axis->Draw();
  deltaM_axis->PaintAxis(gPad->GetUxmin(), gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax(), -5., 5., 510)
  */
  theEtaProfileAllPt->SetStats(kFALSE);
  theEtaProfileAllPt->SetMaximum(0.001);
  theEtaProfileAllPt->SetMinimum(-0.001);
  theEtaProfileAllPt->SetMarkerColor(kBlack);
  theEtaProfileAllPtRun1->SetMarkerColor(kBlue);
  theEtaProfileAllPtRun2->SetMarkerColor(kRed);
  theEtaProfileAllPtRun3->SetMarkerColor(kGreen);
  theEtaProfileAllPt->SetLineColor(kBlack);
  theEtaProfileAllPtRun1->SetLineColor(kBlue);
  theEtaProfileAllPtRun2->SetLineColor(kRed);
  theEtaProfileAllPtRun3->SetLineColor(kGreen);
  theEtaProfileAllPt->SetMarkerStyle(kFullCircle);
  theEtaProfileAllPtRun1->SetMarkerStyle(kFullCircle);
  theEtaProfileAllPtRun2->SetMarkerStyle(kFullCircle);
  theEtaProfileAllPtRun3->SetMarkerStyle(kFullCircle);
  theEtaProfileAllPt->SetMarkerSize(0.7);
  theEtaProfileAllPtRun1->SetMarkerSize(0.7);
  theEtaProfileAllPtRun2->SetMarkerSize(0.7);
  theEtaProfileAllPtRun3->SetMarkerSize(0.7);
  theEtaProfileAllPt->SetLineWidth(1);
  theEtaProfileAllPt->Draw("");
  theEtaProfileAllPtRun1->Draw("same");
  theEtaProfileAllPtRun2->Draw("same");
  theEtaProfileAllPtRun3->Draw("same");
 

  leg = new TLegend(0.48,0.7,0.68,0.9);
  leg->AddEntry(theEtaProfileAllPtRun1,"284285");
  leg->AddEntry(theEtaProfileAllPtRun2,"304008");
  leg->AddEntry(theEtaProfileAllPtRun3,"306310");
  leg->AddEntry(theEtaProfileAllPt,"MC");
  leg->Draw();

  radialetaallCanvas->Print("radialetaallplot.png");

  std::cout << endl << "    ** theRadialCorrectionEtaProfile ** Game Over. Insert coin ** " << std::endl << std::endl;
  theEtaProfileAllPt->Reset(); theEtaProfileAllPtRun1->Reset(); theEtaProfileAllPtRun2->Reset(); theEtaProfileAllPtRun3->Reset();
  inputHistosFile->Close(); inputHistosFile2->Close(); inputHistosFile3->Close(); inputHistosFileMC->Close();
  return;
}


//Main function
void theRadialCorrection()
{
  IntegratedRadialDistortion();
  theRadialCorrectionPlots();
  theRadialCorrectionEtaPlots();
  theRadialCorrectionEtaProfile();
  return;
}

