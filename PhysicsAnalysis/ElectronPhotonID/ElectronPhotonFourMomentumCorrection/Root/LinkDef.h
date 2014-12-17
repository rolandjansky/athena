/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"
#include "ElectronPhotonFourMomentumCorrection/eg_resolution.h"
#include "ElectronPhotonFourMomentumCorrection/get_MaterialResolutionEffect.h"
#include "ElectronPhotonFourMomentumCorrection/e1hg_systematics.h"
#include "egammaLayerRecalibTool/egammaLayerRecalibTool.h"
#include "egammaMVACalib/egammaMVACalib.h"
#include "ElectronPhotonFourMomentumCorrection/GainTool.h"

#include <vector>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ namespace AtlasRoot;
#pragma link C++ class egGain::GainTool;
#pragma link C++ class AtlasRoot::egammaEnergyCorrectionTool;
#pragma link C++ class AtlasRoot::egammaEnergyCorrectionTool::ParticleInformation;
#pragma link C++ class eg_resolution;
#pragma link C++ class get_MaterialResolutionEffect;
#pragma link C++ class e1hg_systematics;
#pragma link C++ namespace egEnergyCorr;
#pragma link C++ enum egEnergyCorr::ESModel;
#pragma link C++ namespace egEnergyCorr::Resolution;
#pragma link C++ enum egEnergyCorr::Resolution::Variation;
#pragma link C++ namespace egEnergyCorr::Scale;
#pragma link C++ enum egEnergyCorr::Scale::Variation;

#endif
