/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODInDetMeasurement/versions/StripClusterAuxContainer_v1.h"

namespace xAOD {
    StripClusterAuxContainer_v1::StripClusterAuxContainer_v1()
    : AuxContainerBase() {
        AUX_VARIABLE(identifierHash);
        AUX_VARIABLE(localPosition);
        AUX_VARIABLE(localCovariance);
        AUX_VARIABLE(globalPosition);
        AUX_VARIABLE(rdoList);
        AUX_VARIABLE(channelsInPhi);
        AUX_VARIABLE(hitsInThirdTimeBin);
    }
}
