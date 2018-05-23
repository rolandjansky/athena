/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_EFFRECPLOTS_H
#define EGAMMAVALIDATION_EFFRECPLOTS_H

#include "TH1.h"
#include "TMath.h"

#include <TROOT.h>
#include <TStyle.h>

namespace egammaMonitoring{

  class EffRecPlots{
  public:

    EffRecPlots();

    // Efficiency Histos

    TH1D *tConvRConv_pT_Eff    = nullptr; //!
    TH1D *tConvRC1Si_pT_Eff    = nullptr; //!
    TH1D *tConvRC1TRT_pT_Eff   = nullptr; //!
    TH1D *tConvRC2Si_pT_Eff    = nullptr; //!
    TH1D *tConvRC2TRT_pT_Eff   = nullptr; //!
    TH1D *tConvRC2SiTRT_pT_Eff = nullptr; //!
    TH1D *tConvRUnconv_pT_Eff  = nullptr; //!

    TH1D *tConvRConv_eta_Eff    = nullptr; //!
    TH1D *tConvRC1Si_eta_Eff    = nullptr; //!
    TH1D *tConvRC1TRT_eta_Eff   = nullptr; //!
    TH1D *tConvRC2Si_eta_Eff    = nullptr; //!
    TH1D *tConvRC2TRT_eta_Eff   = nullptr; //!
    TH1D *tConvRC2SiTRT_eta_Eff = nullptr; //!
    TH1D *tConvRUnconv_eta_Eff  = nullptr; //!

    TH1D *tConvRConv_phi_Eff    = nullptr; //!
    TH1D *tConvRC1Si_phi_Eff    = nullptr; //!
    TH1D *tConvRC1TRT_phi_Eff   = nullptr; //!
    TH1D *tConvRC2Si_phi_Eff    = nullptr; //!
    TH1D *tConvRC2TRT_phi_Eff   = nullptr; //!
    TH1D *tConvRC2SiTRT_phi_Eff = nullptr; //!
    TH1D *tConvRUnconv_phi_Eff  = nullptr; //!

    TH1D *tConvRConv_CR_Eff    = nullptr; //!
    TH1D *tConvRC1Si_CR_Eff    = nullptr; //!
    TH1D *tConvRC1TRT_CR_Eff   = nullptr; //!
    TH1D *tConvRC2Si_CR_Eff    = nullptr; //!
    TH1D *tConvRC2TRT_CR_Eff   = nullptr; //!
    TH1D *tConvRC2SiTRT_CR_Eff = nullptr; //!
    TH1D *tConvRUnconv_CR_Eff  = nullptr; //!

    TH1D *tConvRConv_mu_Eff    = nullptr; //!
    TH1D *tConvRC1Si_mu_Eff    = nullptr; //!
    TH1D *tConvRC1TRT_mu_Eff   = nullptr; //!
    TH1D *tConvRC2Si_mu_Eff    = nullptr; //!
    TH1D *tConvRC2TRT_mu_Eff   = nullptr; //!
    TH1D *tConvRC2SiTRT_mu_Eff = nullptr; //!
    TH1D *tConvRUnconv_mu_Eff  = nullptr; //!
    
    TH1D *tUnconvRConv_pT_Eff    = nullptr; //!
    TH1D *tUnconvRC1Si_pT_Eff    = nullptr; //!
    TH1D *tUnconvRC1TRT_pT_Eff   = nullptr; //!
    TH1D *tUnconvRC2Si_pT_Eff    = nullptr; //!
    TH1D *tUnconvRC2TRT_pT_Eff   = nullptr; //!
    TH1D *tUnconvRC2SiTRT_pT_Eff = nullptr; //!
    TH1D *tUnconvRUnconv_pT_Eff  = nullptr; //!

    TH1D *tUnconvRConv_eta_Eff    = nullptr; //!
    TH1D *tUnconvRC1Si_eta_Eff    = nullptr; //!
    TH1D *tUnconvRC1TRT_eta_Eff   = nullptr; //!
    TH1D *tUnconvRC2Si_eta_Eff    = nullptr; //!
    TH1D *tUnconvRC2TRT_eta_Eff   = nullptr; //!
    TH1D *tUnconvRC2SiTRT_eta_Eff = nullptr; //!
    TH1D *tUnconvRUnconv_eta_Eff  = nullptr; //!

    TH1D *tUnconvRConv_phi_Eff    = nullptr; //!
    TH1D *tUnconvRC1Si_phi_Eff    = nullptr; //!
    TH1D *tUnconvRC1TRT_phi_Eff   = nullptr; //!
    TH1D *tUnconvRC2Si_phi_Eff    = nullptr; //!
    TH1D *tUnconvRC2TRT_phi_Eff   = nullptr; //!
    TH1D *tUnconvRC2SiTRT_phi_Eff = nullptr; //!
    TH1D *tUnconvRUnconv_phi_Eff  = nullptr; //!

    TH1D *tUnconvRConv_CR_Eff    = nullptr; //!
    TH1D *tUnconvRC1Si_CR_Eff    = nullptr; //!
    TH1D *tUnconvRC1TRT_CR_Eff   = nullptr; //!
    TH1D *tUnconvRC2Si_CR_Eff    = nullptr; //!
    TH1D *tUnconvRC2TRT_CR_Eff   = nullptr; //!
    TH1D *tUnconvRC2SiTRT_CR_Eff = nullptr; //!
    TH1D *tUnconvRUnconv_CR_Eff  = nullptr; //!
    
    TH1D *tUnconvRConv_mu_Eff    = nullptr; //!
    TH1D *tUnconvRC1Si_mu_Eff    = nullptr; //!
    TH1D *tUnconvRC1TRT_mu_Eff   = nullptr; //!
    TH1D *tUnconvRC2Si_mu_Eff    = nullptr; //!
    TH1D *tUnconvRC2TRT_mu_Eff   = nullptr; //!
    TH1D *tUnconvRC2SiTRT_mu_Eff = nullptr; //!
    TH1D *tUnconvRUnconv_mu_Eff   = nullptr; //!

    void initializePlots();
    void divide(const char* EffPlot, TH1* h1, TH1* h2);

  private:

    float m_cR_bins[15] = {0, 50, 89, 123, 170, 210, 250, 299, 335, 371, 443, 514, 554, 800, 1085};

  };

}

#endif
