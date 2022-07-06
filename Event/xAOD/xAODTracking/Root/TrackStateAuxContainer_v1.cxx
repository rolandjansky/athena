/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODTracking/versions/TrackStateAuxContainer_v1.h"

namespace xAOD {
  TrackStateAuxContainer_v1::TrackStateAuxContainer_v1()
    : AuxContainerBase() {
    AUX_VARIABLE(previous);
    AUX_VARIABLE(chi2);
    AUX_VARIABLE(pathLength);
    AUX_VARIABLE(predicted);
    AUX_VARIABLE(filtered);
    AUX_VARIABLE(smoothed);
    AUX_VARIABLE(jacobian);
    AUX_VARIABLE(projector);
    AUX_VARIABLE(uncalibrated);
    AUX_VARIABLE(calibrated);
    AUX_VARIABLE(measDim);
  }
}


