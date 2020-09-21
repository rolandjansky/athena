/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EG_RESOLUTION_H
#define EG_RESOLUTION_H

#include <cstdlib>
#include <cmath>
#include "xAODEgamma/Egamma.h"
#include "TH1.h"
#include "TFile.h"
#include "TArrayD.h"
#include <memory>
/**
  @class eg_resolution
  @brief get resolution for electron and photons (converted / unconverted) vs E,eta

  Different parameterizations (gaussian core, sigma eff 90% and 80% from crystal ball fits)
   are available.

  This in MC based without pileup.

*/

class eg_resolution {

 public:
  /** @brief constructor (initialization done there reading root files with resolution fit parameters */
  eg_resolution(const std::string& configuration);
  ~eg_resolution();

  /** @brief get relative resolution (sigmaE/E) as a function of E (in Mev) and eta
      @brief particle type : 0=electron, 1=reco unconverted photon, 2=reco converted photon, 3=true unconverted photon
      @brief resolution type : 0=gaussian core, 1=sigma eff 80%, 2=sigma eff 90% (default)
  */
  double getResolution(int particle_type, double energy, double eta, int resol_type=2) const;
  /** @brief get relative resolution (sigmaE/E) for egamma particles
      @brief resolution type : 0=gaussian core, 1=sigma eff 80%, 2=sigma eff 90% (default)
  */
  double getResolution(const xAOD::Egamma& particle, int resol_type=2) const;

 private:

  // histograms to store resolution parameters
  TH1* m_hSampling[4][3];
  TH1* m_hNoise[4][3];
  TH1* m_hConst[4][3];
  std::unique_ptr <TFile>  m_file0;
  std::unique_ptr <TFile>  m_file1;
  std::unique_ptr <TFile>  m_file2;
  std::unique_ptr <TFile>  m_file3;
  const TArrayD* m_etaBins;

};

#endif
