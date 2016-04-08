/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParticleEnergyParametrizationInDistbin_h
#define ParticleEnergyParametrizationInDistbin_h

#include <iostream>

#include <TNamed.h>
#include <TH1D.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"

class ParticleEnergyParametrizationInDistbin:public TNamed {
public:
  TH1*      m_ElayerProp[CaloCell_ID_FCS::MaxSample];
  TVectorD  m_mean;
  TVectorD  m_RMS;
  TMatrixD  m_corr;
  
  ParticleEnergyParametrizationInDistbin();

  void SetNoDirectoryHisto();

  ClassDef(ParticleEnergyParametrizationInDistbin,1)  //ParticleEnergyParametrizationInDistbin
};

#endif
