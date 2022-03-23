/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODInDetMeasurement/versions/PixelClusterAuxContainer_v1.h"

namespace xAOD {
    PixelClusterAuxContainer_v1::PixelClusterAuxContainer_v1()
    : AuxContainerBase() {
        AUX_VARIABLE(identifierHash);
        AUX_VARIABLE(localPosition);
        AUX_VARIABLE(localCovariance);
        AUX_VARIABLE(globalPosition);
        AUX_VARIABLE(rdoList);
        AUX_VARIABLE(channelsInPhi);
        AUX_VARIABLE(channelsInEta);
        AUX_VARIABLE(omegaX);
        AUX_VARIABLE(omegaY);
        AUX_VARIABLE(totList);
        AUX_VARIABLE(totalToT);
        AUX_VARIABLE(chargeList);
        AUX_VARIABLE(totalCharge);
        AUX_VARIABLE(energyLoss);
        AUX_VARIABLE(isSplit);
        AUX_VARIABLE(splitProbability1);
        AUX_VARIABLE(splitProbability2);
        AUX_VARIABLE(lvl1a);
    }
}
