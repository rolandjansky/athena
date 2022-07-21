/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKPARAMETERSAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_TRACKPARAMETERSAUXCONTAINER_V1_H



#include "xAODCore/AuxContainerBase.h"
namespace xAOD {
 class TrackParametersAuxContainer_v1 : public AuxContainerBase {
    public:
        TrackParametersAuxContainer_v1();
        std::vector<double> parameters;
        std::vector<double> covariance;
    };
}

#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::TrackParametersAuxContainer_v1, xAOD::AuxContainerBase);


#endif