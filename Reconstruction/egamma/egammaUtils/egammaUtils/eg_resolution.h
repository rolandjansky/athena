/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EG_RESOLUTION_H
#define EG_RESOLUTION_H

#include <cmath>
#include <cstdlib>
#include <memory>
#include <array>
#include "TArrayD.h"
#include "TFile.h"
#include "TH1.h"
#include "xAODEgamma/Egamma.h"
/**
  @class eg_resolution
  @brief get resolution for electron and photons (converted / unconverted) vs
  E,eta

  Different parameterizations (gaussian core, sigma eff 90% and 80% from crystal
  ball fits) are available.

  This in MC based without pileup.
*/

class eg_resolution
{

public:
  static constexpr size_t samplings = 4;
  static constexpr size_t resolTypes = 3;
  /** @brief constructor (initialization done there reading root files with
   * resolution fit parameters */
  eg_resolution(const std::string& configuration);
  ~eg_resolution() = default;

  /** @brief get relative resolution (sigmaE/E) as a function of E (in Mev) and
     eta
      @brief particle type : 0=electron, 1=reco unconverted photon, 2=reco
     converted photon, 3=true unconverted photon
      @brief resolution type : 0=gaussian core, 1=sigma eff 80%, 2=sigma eff 90%
     (default)
  */
  double getResolution(int particle_type,
                       double energy,
                       double eta,
                       int resol_type = 2) const;
  /** @brief get relative resolution (sigmaE/E) for egamma particles
      @brief resolution type : 0=gaussian core, 1=sigma eff 80%, 2=sigma eff 90%
     (default)
  */
  double getResolution(const xAOD::Egamma& particle, int resol_type = 2) const;

private:
  // histograms to store resolution parameters
  std::array<std::array<std::unique_ptr<TH1>, resolTypes>, samplings> m_hSampling;
  std::array<std::array<std::unique_ptr<TH1>, resolTypes>, samplings> m_hNoise;
  std::array<std::array<std::unique_ptr<TH1>, resolTypes>, samplings> m_hConst;
  const TArrayD* m_etaBins; // Not owning pointer
};

#endif
