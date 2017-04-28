/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef get_MaterialResolutionEffect_H
#define get_MaterialResolutionEffect_H
#include <stdlib.h>
#include <math.h>
#include <memory>

#include "AsgTools/AsgMessaging.h"

#include "TH1.h"
#include "TH2D.h"
#include "TFile.h"
#include "TArrayD.h"

/**
  @class get_MaterialResolutionEffect
  @brief get resolution systematics from material effect (mc12c setup, MVA calibration)

  Different parameterizations (gaussian core, sigma eff 90% and 80% from crystal ball fits)
   are available

*/

class get_MaterialResolutionEffect : public asg::AsgMessaging {

 public:
  /** @brief constructor (initialization done there reading root files with resolution fit parameters */
  get_MaterialResolutionEffect();
  ~get_MaterialResolutionEffect();

  /** @brief get material effect on resolution from distorted geometry as difference to 40 GeV Et electrons smearing
      @brief particle_type: 0=electron, 1=reco unconverted photon, 2=reco converted photon
      @brief energy : energy in MeV
      @brief eta : eta
      @brief response_type:  0=gaussian core, 1=sigma eff 80%, 2=sigma eff 90%
      @brief isyst : 0 configA, 1 configCD, 2 configE, 3 configFMX
      @brief  returned value is sigmaE/E change in quadrature to resolution
  */
  double getDelta(int particle_type, double energy, double eta,int response_type,int isyst) const;

 private:

  // histograms to store resolution parameters
  TH1D* m_hSystPeak[4][8][3];
  TH1D* m_hSystResol[4][8][3];
  TH2D* m_hsyst_IBL_PP0[3];
  std::unique_ptr <TFile> m_file0;
  //const TArrayD* m_etaBins;
  const TArrayD* m_etBins;


};

#endif
