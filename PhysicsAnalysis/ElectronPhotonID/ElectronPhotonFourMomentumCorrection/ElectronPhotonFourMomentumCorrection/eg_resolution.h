/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EG_RESOLUTION_H
#define EG_RESOLUTION_H

#include <stdlib.h>
#include <math.h>

#include "TAxis.h"
#include "Riostream.h"
#include "TH1.h"
#include "TFile.h"
#include "TArrayD.h"

/**
  @class eg_resolution
  @brief get resolution for electron and photons (converted / unconverted) vs E,eta

  Different parameterizations (gaussian core, sigma eff 90% and 80% from crystal ball fits)
   are available

*/

class eg_resolution {

 public:
  /** @brief constructor (initialization done there reading root files with resolution fit parameters */
  eg_resolution();
  ~eg_resolution();

  /** @brief get relative resolution (sigmaE/E) as a function of E (in Mev) and eta
      @brief particle type : 0=electron, 1=reco unconverted photon, 2=reco converted photon, 3=true unconverted photon
      @brief resolution type : 0=gaussian core, 1=sigma eff 80%, 2=sigma eff 90% (default)
  */
  double getResolution(int particle_type, double energy, double eta, int resol_type=2) const;

 private:

  // histograms to store resolution parameters
  TH1D* hSampling[4][3];
  TH1D* hNoise[4][3];
  TH1D* hConst[4][3];
  TFile* file0;
  TFile* file1;
  TFile* file2;
  TFile* file3;
  const TArrayD* etaBins;

};

#endif
