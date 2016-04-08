/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ParticleEnergyShape_h
#define ParticleEnergyShape_h

#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"

class FastShowerInfo;

class ParticleEnergyShape {
public:
  double Ecal;
  double dist_in;
  double dist000;
  double dist_rec;

  double E;
  double E_layer[CaloCell_ID_FCS::MaxSample];

  double fcal_tot;
  double fcal_layer[CaloCell_ID_FCS::MaxSample];
  double fcal_tot_uncor;
  
  ParticleEnergyShape();

  void CopyDebugInfo(FastShowerInfo* fsi);
};

#endif
