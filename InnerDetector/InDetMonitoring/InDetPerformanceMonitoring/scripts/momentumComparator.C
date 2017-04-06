//
#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH3.h"
#include "TLegend.h"
#include "TPad.h"
#include "TString.h"
#include "TStyle.h"

int m_etabin;
int m_phibin;

TH1* h_PtPosMu;
TH1* h_PtNegMu;
TH1* h_PosOverNegRatio;

TH3F* h_3DPtPosMu; 
TH3F* h_3DPtNegMu; 

TCanvas* m_Canvas;
TPad* m_ptPad;
TPad* m_ratioPad;

// prototypes
int  EtaBinCheck (int);
int  PhiBinCheck (int);
void WelcomeBanner();
TH1* ProjectPt (int, TString theOptions=""); 
TH1* ComputeRatio ();
void ClosingBanner ();
void CheckRanges ();
void PrepareCanvas ();

/////////////////////////////////////
void momentumComparator( int etabin=0, int phibin=0)
{
  TFile *inputHistosFile = TFile::Open("ZmumuValidationExampleOutput_test.root");

  WelcomeBanner ();
  
  //////////////////
  gStyle->SetPalette(1);
  gStyle->SetPadRightMargin(0.08);
  //

  h_3DPtPosMu = (TH3F*)inputHistosFile->Get("h_ptpos_vs_etaphi"); 
  h_3DPtNegMu = (TH3F*)inputHistosFile->Get("h_ptneg_vs_etaphi"); 

  //
  m_etabin = EtaBinCheck (etabin);
  m_phibin = PhiBinCheck (phibin);
  std::cout << " ** Projecting bings : eta: " << m_etabin << "  phi: " << m_phibin << endl;


  // project the 3D histos into 1D histos selecting the eta-phi bin
  h_PtPosMu = ProjectPt (1, "e");
  h_PtNegMu = ProjectPt(-1);
  h_PosOverNegRatio = ComputeRatio();
  CheckRanges();

  // canvas for drawing
  PrepareCanvas();

  // draw the pt
  m_ptPad->cd();
  h_PtPosMu->Draw("hist");
  h_PtNegMu->Draw("same");
  TLegend* theLegend = new TLegend(0.75, 0.7, 0.85,0.85);
  theLegend->AddEntry(h_PtPosMu,"#mu^{+}","l");
  theLegend->AddEntry(h_PtNegMu,"#mu^{-}","l");
  theLegend->Draw();


  // draw the ratio
  m_ratioPad->cd();
  h_PosOverNegRatio->Draw();
  TLine theLine;
  theLine.SetLineStyle(kDashed);
  theLine.DrawLine(h_PosOverNegRatio->GetXaxis()->GetXmin(), 1., h_PosOverNegRatio->GetXaxis()->GetXmax(), 1.);

  // 
  ClosingBanner();
  return;
}

///////////////////////////////////
void WelcomeBanner ()
{
  std::cout << std::endl << " ** momentumComparator ** START ** " << std::endl << std::endl;
  return;
}

///////////////////////////////////
void ClosingBanner ()
{
  std::cout << std::endl << " ** momentumComparator **  G A M E   O V E R   **  I N S E R T   C O I N  ** " << std::endl << std::endl;
  return;
}

///////////////////////////////////
int EtaBinCheck (int inputEtabin)
{
  int theEtabin = inputEtabin;
  bool goodInput = true;
  if (inputEtabin < 0) {goodInput = false; theEtabin = 0;}
  if (inputEtabin > h_3DPtPosMu->GetXaxis()->GetNbins()) {goodInput = false; theEtabin = 0;}

  if (! goodInput) std::cout << " ** EtaBinCheck ** problems with input etabin = " << inputEtabin << ". Value set to: " << theEtabin << std::endl; 

  return theEtabin;
}

///////////////////////////////////
int PhiBinCheck (int inputPhibin)
{
  int thePhibin = inputPhibin;
  bool goodInput = true;
  if (inputPhibin < 0) {goodInput = false; thePhibin = 0;}
  if (inputPhibin > h_3DPtPosMu->GetXaxis()->GetNbins()) {goodInput = false; thePhibin = 0;}

  if (! goodInput) std::cout << " ** PhiBinCheck ** problems with input phibin = " << inputPhibin << ". Value set to: " << thePhibin << std::endl; 

  return thePhibin;
}

///////////////////////////////////
TH1* ProjectPt (int charge, TString theOptions = "")
{ 
  TH1* h_projectz = NULL;

  if (charge > 1) charge = 1;
  if (charge == 0) charge = 1;
  if (charge <-1) charge =-1; 

  if (charge == 1) {
    h_projectz = h_3DPtPosMu->ProjectionZ("h_PtPosMu", m_etabin, m_etabin, m_phibin, m_phibin, theOptions.Data()); // --> for errors,"e");
    h_projectz->SetLineColor(kBlue+1);
  }
  if (charge ==-1) {
    h_projectz = h_3DPtNegMu->ProjectionZ("h_PtNegMu", m_etabin, m_etabin, m_phibin, m_phibin);
    h_projectz->SetLineColor(kRed+1);
  }

  TString htitle("p_{T} of the #mu  (#eta= ");
  TString etapart("all");
  if (m_etabin != 0) etapart.Form("%3.2f", h_3DPtPosMu->GetXaxis()->GetBinCenter(m_etabin));
  htitle.Append(etapart.Data());
  htitle.Append(",");
  TString phipart(" #phi = all)");
  if (m_phibin != 0) phipart.Form("#phi = %3.2f)", h_3DPtPosMu->GetYaxis()->GetBinCenter(m_phibin));
  htitle.Append(phipart.Data());
  
  h_projectz->SetTitle(htitle.Data());
  h_projectz->SetStats(kFALSE);
  h_projectz->SetLineWidth(2);

  return h_projectz; 
}

///////////////////////////////////
TH1* ComputeRatio ()
{ 
  TH1* h_ratio = NULL;
  h_ratio = (TH1*) h_PtPosMu->Clone();
  h_ratio->Divide(h_PtNegMu);
  h_ratio->SetTitle(""); //h_ratio->SetTitle("p_{T}: N_{#mu^{+}} / N_{#mu^{-}}");
  h_ratio->SetYTitle("N_{#mu^{+}} / N_{#mu^{-}}");
  h_ratio->SetLineColor(kBlack);

  float theMax = h_ratio->GetMaximum();
  float theMin = 999999.; //h_ratio->GetMinimum();
  for (int i=1; i <= h_ratio->GetNbinsX(); i++) {
    if (h_ratio->GetBinContent(i) > 0 && h_ratio->GetBinContent(i) < theMin) theMin = h_ratio->GetBinContent(i);  
  }


  float delta_plus = theMax - 1;
  float delta_minus = 1 - theMin;

  float good_delta = std::max(delta_plus, delta_minus);
  if (good_delta > 0.5) good_delta = 0.5;

  h_ratio->SetMaximum(1+good_delta);
  h_ratio->SetMinimum(1-good_delta);
  h_ratio->SetStats(kFALSE);

  h_ratio->GetXaxis()->SetLabelSize(0.08);
  h_ratio->GetYaxis()->SetLabelSize(0.08);

  h_ratio->GetXaxis()->SetTitleSize(0.08);
  h_ratio->GetYaxis()->SetTitleOffset(0.50);
  h_ratio->GetYaxis()->SetTitleSize(0.08);
  h_ratio->GetXaxis()->SetTickSize(0.06);

  return h_ratio;
}

///////////////////////////////////
void CheckRanges()
{
  float theMax = -99999.;
  float theMin =  99999.;

  if (h_PtPosMu->GetMaximum() > theMax) theMax = h_PtPosMu->GetMaximum();
  if (h_PtNegMu->GetMaximum() > theMax) theMax = h_PtNegMu->GetMaximum();

  if (h_PtPosMu->GetMinimum() < theMin) theMin = h_PtPosMu->GetMinimum();
  if (h_PtNegMu->GetMinimum() < theMin) theMin = h_PtNegMu->GetMinimum();


  theMax = 1.05*(theMax-theMin);

  h_PtPosMu->SetMaximum(theMax);
  h_PtNegMu->SetMaximum(theMax);
  
  h_PtPosMu->SetMinimum(theMin);
  h_PtNegMu->SetMinimum(theMin);

  return;
}

///////////////////////////////////
void PrepareCanvas()
{
  m_ptCanvas = new TCanvas ("c_ptCanvas","ptCanvas", 900,900);

  m_ptPad = new TPad("m_ptPad", "The upper pad",0.0, 0.32, 1.0, 1.0, kWhite);
  m_ptPad->SetBottomMargin(0.005);
  m_ptPad->SetTickx(); 
  m_ptPad->SetTicky(); 
  m_ptPad->Draw();

  m_ratioPad = new TPad("pad2", "The lower pad",0.0, 0.0, 1.0, 0.32, kWhite);
  m_ratioPad->SetTopMargin(0.005);
  m_ratioPad->SetBottomMargin(0.20);
  m_ratioPad->SetTickx(); 
  m_ratioPad->SetTicky(); 
  m_ratioPad->Draw();

  return;
}
