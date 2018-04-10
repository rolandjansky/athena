/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_SHOWERSHAPESPLOTS_H
#define EGAMMAVALIDATION_SHOWERSHAPESPLOTS_H

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "TH1.h"

namespace egammaMonitoring{

  class ShowerShapesPlots{
  public:

    // Histos

    TH1D* hadleak = nullptr; //!
    TH1D* reta    = nullptr; //!
    TH1D* rphi    = nullptr; //!
    TH1D* weta2   = nullptr; //!
    TH1D* eratio  = nullptr; //!
    TH1D* deltae  = nullptr; //!
    TH1D* f1      = nullptr; //!
    TH1D* fside   = nullptr; //!
    TH1D* wtots1  = nullptr; //!
    TH1D* ws3     = nullptr; //!


    void initializePlots();
    void fill(const xAOD::Egamma& egamma);
    
  private:

    float m_eta2, m_rhad, m_rhad1, m_hadrleak, m_Reta, m_Rphi, m_shweta2, m_Eratio, m_DeltaE, m_frac_f1, m_shfside, m_shwtots1, m_shws3;

  };

}

#endif
