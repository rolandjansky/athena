/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h>

int main() {

  AtlasRoot::egammaEnergyCorrectionTool ect;
  ect.setESModel(egEnergyCorr::es2011c);  
  ect.initialize();



}
