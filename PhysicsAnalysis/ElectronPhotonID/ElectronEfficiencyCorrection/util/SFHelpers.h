/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

#ifndef __ELECTRONEFFICIENCYCORRECTION_SFHELPER__
#define __ELECTRONEFFICIENCYCORRECTION_SFHELPER__

#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include <xAODEgamma/Electron.h>

namespace SFHelpers {
int
result(AsgElectronEfficiencyCorrectionTool& tool,
       const xAOD::Electron& el,
       double& nominalSF,
       double& totalPos,
       double& totalNeg,
       const bool isToys);
}
#endif
