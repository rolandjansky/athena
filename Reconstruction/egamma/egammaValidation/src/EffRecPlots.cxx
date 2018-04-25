/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaValidation/EffRecPlots.h"

namespace egammaMonitoring {

  EffRecPlots::EffRecPlots(){}

  void EffRecPlots::initializePlots() {

    gROOT->ForceStyle();
    gStyle->SetOptStat(0);

    tConvRConv_pT_Eff    = new TH1D("tConvRConv_pT_Eff"      , ";p_{T} [GeV];p_{T} Efficiency truthConvRecoConv"      , 200, 0, 200);
    tConvRC1Si_pT_Eff    = new TH1D("tConvRConv1Si_pT_Eff"   , ";p_{T} [GeV];p_{T} Efficiency truthConvRecoConv1Si"   , 200, 0, 200);
    tConvRC1TRT_pT_Eff   = new TH1D("tConvRConv1TRT_pT_Eff"  , ";p_{T} [GeV];p_{T} Efficiency truthConvRecoConv1TRT"  , 200, 0, 200);
    tConvRC2Si_pT_Eff    = new TH1D("tConvRConv2Si_pT_Eff"   , ";p_{T} [GeV];p_{T} Efficiency truthConvRecoConv2Si"   , 200, 0, 200);
    tConvRC2TRT_pT_Eff   = new TH1D("tConvRConv2TRT_pT_Eff"  , ";p_{T} [GeV];p_{T} Efficiency truthConvRecoConv2TRT"  , 200, 0, 200);
    tConvRC2SiTRT_pT_Eff = new TH1D("tConvRConv2SiTRT_pT_Eff", ";p_{T} [GeV];p_{T} Efficiency truthConvRecoConv2SiTRT", 200, 0, 200);
    tConvRUnconv_pT_Eff  = new TH1D("tConvRUnconv_pT_Eff"    , ";p_{T} [GeV];p_{T} Efficiency truthConvRecoUnconv"    , 200, 0, 200);

    tConvRConv_eta_Eff    = new TH1D("tConvRConv_eta_Eff"      , ";#eta;#eta Efficiency truthConvRecoConv"      , 200, -3, 3);
    tConvRC1Si_eta_Eff    = new TH1D("tConvRConv1Si_eta_Eff"   , ";#eta;#eta Efficiency truthConvRecoConv1Si"   , 200, -3, 3);
    tConvRC1TRT_eta_Eff   = new TH1D("tConvRConv1TRT_eta_Eff"  , ";#eta;#eta Efficiency truthConvRecoConv1TRT"  , 200, -3, 3);
    tConvRC2Si_eta_Eff    = new TH1D("tConvRConv2Si_eta_Eff"   , ";#eta;#eta Efficiency truthConvRecoConv2Si"   , 200, -3, 3);
    tConvRC2TRT_eta_Eff   = new TH1D("tConvRConv2TRT_eta_Eff"  , ";#eta;#eta Efficiency truthConvRecoConv2TRT"  , 200, -3, 3);
    tConvRC2SiTRT_eta_Eff = new TH1D("tConvRConv2SiTRT_eta_Eff", ";#eta;#eta Efficiency truthConvRecoConv2SiTRT", 200, -3, 3);
    tConvRUnconv_eta_Eff  = new TH1D("tConvRUnconv_eta_Eff"    , ";#eta;#eta Efficiency truthConvRecoUnconv"    , 200, -3, 3);

    tConvRConv_phi_Eff    = new TH1D("tConvRConv_phi_Eff"      , ";#phi;#phi Efficiency truthConvRecoConv"      , 100, -TMath::Pi(), TMath::Pi());
    tConvRC1Si_phi_Eff    = new TH1D("tConvRConv1Si_phi_Eff"   , ";#phi;#phi Efficiency truthConvRecoConv1Si"   , 100, -TMath::Pi(), TMath::Pi());
    tConvRC1TRT_phi_Eff   = new TH1D("tConvRConv1TRT_phi_Eff"  , ";#phi;#phi Efficiency truthConvRecoConv1TRT"  , 100, -TMath::Pi(), TMath::Pi());
    tConvRC2Si_phi_Eff    = new TH1D("tConvRConv2Si_phi_Eff"   , ";#phi;#phi Efficiency truthConvRecoConv2Si"   , 100, -TMath::Pi(), TMath::Pi());
    tConvRC2TRT_phi_Eff   = new TH1D("tConvRConv2TRT_phi_Eff"  , ";#phi;#phi Efficiency truthConvRecoConv2TRT"  , 100, -TMath::Pi(), TMath::Pi());
    tConvRC2SiTRT_phi_Eff = new TH1D("tConvRConv2SiTRT_phi_Eff", ";#phi;#phi Efficiency truthConvRecoConv2SiTRT", 100, -TMath::Pi(), TMath::Pi());
    tConvRUnconv_phi_Eff  = new TH1D("tConvRUnconv_phi_Eff"    , ";#phi;#phi Efficiency truthConvRecoUnconv"    , 100, -TMath::Pi(), TMath::Pi());

    tConvRConv_CR_Eff    = new TH1D("tConvRConv_CR_Eff"      , ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoConv"      , 14, m_cR_bins);
    tConvRC1Si_CR_Eff    = new TH1D("tConvRConv1Si_CR_Eff"   , ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoConv1Si"   , 14, m_cR_bins);
    tConvRC1TRT_CR_Eff   = new TH1D("tConvRConv1TRT_CR_Eff"  , ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoConv1TRT"  , 14, m_cR_bins);
    tConvRC2Si_CR_Eff    = new TH1D("tConvRConv2Si_CR_Eff"   , ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoConv2Si"   , 14, m_cR_bins);
    tConvRC2TRT_CR_Eff   = new TH1D("tConvRConv2TRT_CR_Eff"  , ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoConv2TRT"  , 14, m_cR_bins);
    tConvRC2SiTRT_CR_Eff = new TH1D("tConvRConv2SiTRT_CR_Eff", ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoConv2SiTRT", 14, m_cR_bins);
    tConvRUnconv_CR_Eff  = new TH1D("tConvRUnconv_CR_Eff"    , ";Conversion Radius [mm];Conversion Radius Efficiency truthConvRecoUnconv"    , 14, m_cR_bins);
    
    tConvRConv_mu_Eff    = new TH1D("tConvRConv_mu_Eff"      , ";<#mu>;<#mu> Efficiency truthConvRecoConv"      , 15, 0, 60);
    tConvRC1Si_mu_Eff    = new TH1D("tConvRConv1Si_mu_Eff"   , ";<#mu>;<#mu> Efficiency truthConvRecoConv1Si"   , 15, 0, 60);
    tConvRC1TRT_mu_Eff   = new TH1D("tConvRConv1TRT_mu_Eff"  , ";<#mu>;<#mu> Efficiency truthConvRecoConv1TRT"  , 15, 0, 60);
    tConvRC2Si_mu_Eff    = new TH1D("tConvRConv2Si_mu_Eff"   , ";<#mu>;<#mu> Efficiency truthConvRecoConv2Si"   , 15, 0, 60);
    tConvRC2TRT_mu_Eff   = new TH1D("tConvRConv2TRT_mu_Eff"  , ";<#mu>;<#mu> Efficiency truthConvRecoConv2TRT"  , 15, 0, 60);
    tConvRC2SiTRT_mu_Eff = new TH1D("tConvRConv2SiTRT_mu_Eff", ";<#mu>;<#mu> Efficiency truthConvRecoConv2SiTRT", 15, 0, 60);
    tConvRUnconv_mu_Eff  = new TH1D("tConvRUnconv_mu_Eff"    , ";<#mu>;<#mu> Efficiency truthConvRecoUnconv"    , 15, 0, 60);

    tUnconvRConv_pT_Eff    = new TH1D("tUnconvRConv_pT_Eff"      , ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoConv"      , 200, 0, 200);
    tUnconvRC1Si_pT_Eff    = new TH1D("tUnconvRConv1Si_pT_Eff"   , ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoConv1Si"   , 200, 0, 200);
    tUnconvRC1TRT_pT_Eff   = new TH1D("tUnconvRConv1TRT_pT_Eff"  , ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoConv1TRT"  , 200, 0, 200);
    tUnconvRC2Si_pT_Eff    = new TH1D("tUnconvRConv2Si_pT_Eff"   , ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoConv2Si"   , 200, 0, 200);
    tUnconvRC2TRT_pT_Eff   = new TH1D("tUnconvRConv2TRT_pT_Eff"  , ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoConv2TRT"  , 200, 0, 200);
    tUnconvRC2SiTRT_pT_Eff = new TH1D("tUnconvRConv2SiTRT_pT_Eff", ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoConv2SiTRT", 200, 0, 200);
    tUnconvRUnconv_pT_Eff  = new TH1D("tUnconvRUnconv_pT_Eff"    , ";p_{T} [GeV];p_{T} Efficiency truthUnconvRecoUnconv"    , 200, 0, 200);

    tUnconvRConv_eta_Eff    = new TH1D("tUnconvRConv_eta_Eff"      , ";#eta;#eta Efficiency truthUnconvRecoConv"      , 200, -3, 3);
    tUnconvRC1Si_eta_Eff    = new TH1D("tUnconvRConv1Si_eta_Eff"   , ";#eta;#eta Efficiency truthUnconvRecoConv1Si"   , 200, -3, 3);
    tUnconvRC1TRT_eta_Eff   = new TH1D("tUnconvRConv1TRT_eta_Eff"  , ";#eta;#eta Efficiency truthUnconvRecoConv1TRT"  , 200, -3, 3);
    tUnconvRC2Si_eta_Eff    = new TH1D("tUnconvRConv2Si_eta_Eff"   , ";#eta;#eta Efficiency truthUnconvRecoConv2Si"   , 200, -3, 3);
    tUnconvRC2TRT_eta_Eff   = new TH1D("tUnconvRConv2TRT_eta_Eff"  , ";#eta;#eta Efficiency truthUnconvRecoConv2TRT"  , 200, -3, 3);
    tUnconvRC2SiTRT_eta_Eff = new TH1D("tUnconvRConv2SiTRT_eta_Eff", ";#eta;#eta Efficiency truthUnconvRecoConv2SiTRT", 200, -3, 3);
    tUnconvRUnconv_eta_Eff  = new TH1D("tUnconvRUnconv_eta_Eff"    , ";#eta;#eta Efficiency truthUnconvRecoUnconv"    , 200, -3, 3);

    tUnconvRConv_phi_Eff    = new TH1D("tUnconvRConv_phi_Eff"      , ";#phi;#phi Efficiency truthUnconvRecoConv"      , 100, -TMath::Pi(), TMath::Pi());
    tUnconvRC1Si_phi_Eff    = new TH1D("tUnconvRConv1Si_phi_Eff"   , ";#phi;#phi Efficiency truthUnconvRecoConv1Si"   , 100, -TMath::Pi(), TMath::Pi());
    tUnconvRC1TRT_phi_Eff   = new TH1D("tUnconvRConv1TRT_phi_Eff"  , ";#phi;#phi Efficiency truthUnconvRecoConv1TRT"  , 100, -TMath::Pi(), TMath::Pi());
    tUnconvRC2Si_phi_Eff    = new TH1D("tUnconvRConv2Si_phi_Eff"   , ";#phi;#phi Efficiency truthUnconvRecoConv2Si"   , 100, -TMath::Pi(), TMath::Pi());
    tUnconvRC2TRT_phi_Eff   = new TH1D("tUnconvRConv2TRT_phi_Eff"  , ";#phi;#phi Efficiency truthUnconvRecoConv2TRT"  , 100, -TMath::Pi(), TMath::Pi());
    tUnconvRC2SiTRT_phi_Eff = new TH1D("tUnconvRConv2SiTRT_phi_Eff", ";#phi;#phi Efficiency truthUnconvRecoConv2SiTRT", 100, -TMath::Pi(), TMath::Pi());
    tUnconvRUnconv_phi_Eff  = new TH1D("tUnconvRUnconv_phi_Eff"    , ";#phi;#phi Efficiency truthUnconvRecoUnconv"    , 100, -TMath::Pi(), TMath::Pi());

    tUnconvRConv_CR_Eff    = new TH1D("tUnconvRConv_CR_Eff"      , ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoConv"      , 14, m_cR_bins);
    tUnconvRC1Si_CR_Eff    = new TH1D("tUnconvRConv1Si_CR_Eff"   , ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoConv1Si"   , 14, m_cR_bins);
    tUnconvRC1TRT_CR_Eff   = new TH1D("tUnconvRConv1TRT_CR_Eff"  , ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoConv1TRT"  , 14, m_cR_bins);
    tUnconvRC2Si_CR_Eff    = new TH1D("tUnconvRConv2Si_CR_Eff"   , ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoConv2Si"   , 14, m_cR_bins);
    tUnconvRC2TRT_CR_Eff   = new TH1D("tUnconvRConv2TRT_CR_Eff"  , ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoConv2TRT"  , 14, m_cR_bins);
    tUnconvRC2SiTRT_CR_Eff = new TH1D("tUnconvRConv2SiTRT_CR_Eff", ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoConv2SiTRT", 14, m_cR_bins);
    tUnconvRUnconv_CR_Eff  = new TH1D("tUnconvRUnconv_CR_Eff"    , ";Conversion Radius [mm];Conversion Radius Efficiency truthUnconvRecoUnconv"    , 14, m_cR_bins);

    tUnconvRConv_mu_Eff    = new TH1D("tUnconvRConv_mu_Eff"      , ";<#mu>;<#mu> Efficiency truthUnconvRecoConv"      , 15, 0, 60);
    tUnconvRC1Si_mu_Eff    = new TH1D("tUnconvRConv1Si_mu_Eff"   , ";<#mu>;<#mu> Efficiency truthUnconvRecoConv1Si"   , 15, 0, 60);
    tUnconvRC1TRT_mu_Eff   = new TH1D("tUnconvRConv1TRT_mu_Eff"  , ";<#mu>;<#mu> Efficiency truthUnconvRecoConv1TRT"  , 15, 0, 60);
    tUnconvRC2Si_mu_Eff    = new TH1D("tUnconvRConv2Si_mu_Eff"   , ";<#mu>;<#mu> Efficiency truthUnconvRecoConv2Si"   , 15, 0, 60);
    tUnconvRC2TRT_mu_Eff   = new TH1D("tUnconvRConv2TRT_mu_Eff"  , ";<#mu>;<#mu> Efficiency truthUnconvRecoConv2TRT"  , 15, 0, 60);
    tUnconvRC2SiTRT_mu_Eff = new TH1D("tUnconvRConv2SiTRT_mu_Eff", ";<#mu>;<#mu> Efficiency truthUnconvRecoConv2SiTRT", 15, 0, 60);
    tUnconvRUnconv_mu_Eff  = new TH1D("tUnconvRUnconv_mu_Eff"    , ";<#mu>;<#mu> Efficiency truthUnconvRecoUnconv"    , 15, 0, 60);

  } // initializePlots

  void EffRecPlots::divide(const char* EffPlot, TH1* h1, TH1* h2) {

    if(strcmp("tConvRConv_pT"   , EffPlot) == 0) tConvRConv_pT_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1Si_pT"   , EffPlot) == 0) tConvRC1Si_pT_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1TRT_pT"  , EffPlot) == 0) tConvRC1TRT_pT_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2Si_pT"   , EffPlot) == 0) tConvRC2Si_pT_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC2TRT_pT"  , EffPlot) == 0) tConvRC2TRT_pT_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2SiTRT_pT", EffPlot) == 0) tConvRC2SiTRT_pT_Eff->Divide(h1, h2);
    if(strcmp("tConvRUnconv_pT" , EffPlot) == 0) tConvRUnconv_pT_Eff ->Divide(h1, h2);

    if(strcmp("tConvRConv_eta"   , EffPlot) == 0) tConvRConv_eta_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1Si_eta"   , EffPlot) == 0) tConvRC1Si_eta_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1TRT_eta"  , EffPlot) == 0) tConvRC1TRT_eta_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2Si_eta"   , EffPlot) == 0) tConvRC2Si_eta_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC2TRT_eta"  , EffPlot) == 0) tConvRC2TRT_eta_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2SiTRT_eta", EffPlot) == 0) tConvRC2SiTRT_eta_Eff->Divide(h1, h2);
    if(strcmp("tConvRUnconv_eta" , EffPlot) == 0) tConvRUnconv_eta_Eff ->Divide(h1, h2);

    if(strcmp("tConvRConv_phi"   , EffPlot) == 0) tConvRConv_phi_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1Si_phi"   , EffPlot) == 0) tConvRC1Si_phi_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1TRT_phi"  , EffPlot) == 0) tConvRC1TRT_phi_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2Si_phi"   , EffPlot) == 0) tConvRC2Si_phi_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC2TRT_phi"  , EffPlot) == 0) tConvRC2TRT_phi_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2SiTRT_phi", EffPlot) == 0) tConvRC2SiTRT_phi_Eff->Divide(h1, h2);
    if(strcmp("tConvRUnconv_phi" , EffPlot) == 0) tConvRUnconv_phi_Eff ->Divide(h1, h2);
    
    if(strcmp("tConvRConv_CR"   , EffPlot) == 0) tConvRConv_CR_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1Si_CR"   , EffPlot) == 0) tConvRC1Si_CR_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1TRT_CR"  , EffPlot) == 0) tConvRC1TRT_CR_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2Si_CR"   , EffPlot) == 0) tConvRC2Si_CR_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC2TRT_CR"  , EffPlot) == 0) tConvRC2TRT_CR_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2SiTRT_CR", EffPlot) == 0) tConvRC2SiTRT_CR_Eff->Divide(h1, h2);
    if(strcmp("tConvRUnconv_CR" , EffPlot) == 0) tConvRUnconv_CR_Eff ->Divide(h1, h2);

    if(strcmp("tConvRConv_mu"   , EffPlot) == 0) tConvRConv_mu_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1Si_mu"   , EffPlot) == 0) tConvRC1Si_mu_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC1TRT_mu"  , EffPlot) == 0) tConvRC1TRT_mu_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2Si_mu"   , EffPlot) == 0) tConvRC2Si_mu_Eff   ->Divide(h1, h2);
    if(strcmp("tConvRC2TRT_mu"  , EffPlot) == 0) tConvRC2TRT_mu_Eff  ->Divide(h1, h2);
    if(strcmp("tConvRC2SiTRT_mu", EffPlot) == 0) tConvRC2SiTRT_mu_Eff->Divide(h1, h2);
    if(strcmp("tConvRUnconv_mu" , EffPlot) == 0) tConvRUnconv_mu_Eff ->Divide(h1, h2);

    if(strcmp("tUnconvRConv_pT"   , EffPlot) == 0) tUnconvRConv_pT_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1Si_pT"   , EffPlot) == 0) tUnconvRC1Si_pT_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1TRT_pT"  , EffPlot) == 0) tUnconvRC1TRT_pT_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2Si_pT"   , EffPlot) == 0) tUnconvRC2Si_pT_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC2TRT_pT"  , EffPlot) == 0) tUnconvRC2TRT_pT_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2SiTRT_pT", EffPlot) == 0) tUnconvRC2SiTRT_pT_Eff->Divide(h1, h2);
    if(strcmp("tUnconvRUnconv_pT" , EffPlot) == 0) tUnconvRUnconv_pT_Eff ->Divide(h1, h2);

    if(strcmp("tUnconvRConv_eta"   , EffPlot) == 0) tUnconvRConv_eta_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1Si_eta"   , EffPlot) == 0) tUnconvRC1Si_eta_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1TRT_eta"  , EffPlot) == 0) tUnconvRC1TRT_eta_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2Si_eta"   , EffPlot) == 0) tUnconvRC2Si_eta_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC2TRT_eta"  , EffPlot) == 0) tUnconvRC2TRT_eta_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2SiTRT_eta", EffPlot) == 0) tUnconvRC2SiTRT_eta_Eff->Divide(h1, h2);
    if(strcmp("tUnconvRUnconv_eta" , EffPlot) == 0) tUnconvRUnconv_eta_Eff ->Divide(h1, h2);
 
    if(strcmp("tUnconvRConv_phi"   , EffPlot) == 0) tUnconvRConv_phi_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1Si_phi"   , EffPlot) == 0) tUnconvRC1Si_phi_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1TRT_phi"  , EffPlot) == 0) tUnconvRC1TRT_phi_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2Si_phi"   , EffPlot) == 0) tUnconvRC2Si_phi_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC2TRT_phi"  , EffPlot) == 0) tUnconvRC2TRT_phi_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2SiTRT_phi", EffPlot) == 0) tUnconvRC2SiTRT_phi_Eff->Divide(h1, h2);
    if(strcmp("tUnconvRUnconv_phi" , EffPlot) == 0) tUnconvRUnconv_phi_Eff ->Divide(h1, h2);

    if(strcmp("tUnconvRConv_CR"   , EffPlot) == 0) tUnconvRConv_CR_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1Si_CR"   , EffPlot) == 0) tUnconvRC1Si_CR_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1TRT_CR"  , EffPlot) == 0) tUnconvRC1TRT_CR_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2Si_CR"   , EffPlot) == 0) tUnconvRC2Si_CR_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC2TRT_CR"  , EffPlot) == 0) tUnconvRC2TRT_CR_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2SiTRT_CR", EffPlot) == 0) tUnconvRC2SiTRT_CR_Eff->Divide(h1, h2);
    if(strcmp("tUnconvRUnconv_CR" , EffPlot) == 0) tUnconvRUnconv_CR_Eff ->Divide(h1, h2);

    if(strcmp("tUnconvRConv_mu"   , EffPlot) == 0) tUnconvRConv_mu_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1Si_mu"   , EffPlot) == 0) tUnconvRC1Si_mu_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC1TRT_mu"  , EffPlot) == 0) tUnconvRC1TRT_mu_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2Si_mu"   , EffPlot) == 0) tUnconvRC2Si_mu_Eff   ->Divide(h1, h2);
    if(strcmp("tUnconvRC2TRT_mu"  , EffPlot) == 0) tUnconvRC2TRT_mu_Eff  ->Divide(h1, h2);
    if(strcmp("tUnconvRC2SiTRT_mu", EffPlot) == 0) tUnconvRC2SiTRT_mu_Eff->Divide(h1, h2);
    if(strcmp("tUnconvRUnconv_mu" , EffPlot) == 0) tUnconvRUnconv_mu_Eff ->Divide(h1, h2);
    
  } // divide

} // namespace
