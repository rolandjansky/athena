/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EffIDPlots.h"

namespace egammaMonitoring {

  EffIDPlots::EffIDPlots(){}

  void EffIDPlots::initializePlots() {
    
    gROOT->ForceStyle();
    gStyle->SetOptStat(0);

    LLH_pT_Eff = new TH1D("LLH_pT_Eff", ";p_{T} [GeV];p_{T} Efficiency LLH",  200, 0, 200);
    MLH_pT_Eff = new TH1D("MLH_pT_Eff", ";p_{T} [GeV];p_{T} Efficiency MLH",  200, 0, 200); 
    TLH_pT_Eff = new TH1D("TLH_pT_Eff", ";p_{T} [GeV];p_{T} Efficiency TLH",  200, 0, 200);

    LLH_eta_Eff = new TH1D("LLH_eta_Eff", ";#eta;#eta Efficiency LLH",  120, -3, 3);
    MLH_eta_Eff = new TH1D("MLH_eta_Eff", ";#eta;#eta Efficiency MLH",  120, -3, 3);
    TLH_eta_Eff = new TH1D("TLH_eta_Eff", ";#eta;#eta Efficiency TLH",  120, -3, 3);

    LLH_phi_Eff = new TH1D("LLH_phi_Eff", ";#phi;#phi Efficiency LLH",  50, -TMath::Pi(), TMath::Pi());
    MLH_phi_Eff = new TH1D("MLH_phi_Eff", ";#phi;#phi Efficiency MLH",  50, -TMath::Pi(), TMath::Pi());
    TLH_phi_Eff = new TH1D("TLH_phi_Eff", ";#phi;#phi Efficiency TLH",  50, -TMath::Pi(), TMath::Pi());

  } // initializePlots

  void EffIDPlots::divide(const char* EffPlot, TH1* h1, TH1* h2) {

    if(strcmp("pT_LLH", EffPlot) == 0) LLH_pT_Eff->Divide(h1, h2);
    if(strcmp("pT_MLH", EffPlot) == 0) MLH_pT_Eff->Divide(h1, h2);
    if(strcmp("pT_TLH", EffPlot) == 0) TLH_pT_Eff->Divide(h1, h2);

    if(strcmp("eta_LLH", EffPlot) == 0) LLH_eta_Eff->Divide(h1, h2);
    if(strcmp("eta_MLH", EffPlot) == 0) MLH_eta_Eff->Divide(h1, h2);
    if(strcmp("eta_TLH", EffPlot) == 0) TLH_eta_Eff->Divide(h1, h2);

    if(strcmp("phi_LLH", EffPlot) == 0) LLH_phi_Eff->Divide(h1, h2);
    if(strcmp("phi_MLH", EffPlot) == 0) MLH_phi_Eff->Divide(h1, h2);
    if(strcmp("phi_TLH", EffPlot) == 0) TLH_phi_Eff->Divide(h1, h2);

  } // divide

} // namespace
