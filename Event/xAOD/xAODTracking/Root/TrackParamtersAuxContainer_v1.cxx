/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODTracking/versions/TrackParametersAuxContainer_v1.h"

namespace xAOD {
  TrackParametersAuxContainer_v1::TrackParametersAuxContainer_v1()
    : AuxContainerBase() {
    AUX_VARIABLE(parameters);
    AUX_VARIABLE(covariance);
  }
}


