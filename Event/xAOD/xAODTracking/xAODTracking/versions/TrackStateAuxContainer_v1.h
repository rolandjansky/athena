/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKSTATEAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_TRACKSTATEAUXCONTAINER_V1_H



#include "xAODCore/AuxContainerBase.h"
namespace xAOD {
 using TrackStateIndexType=uint32_t;    
 class TrackStateAuxContainer_v1 : public AuxContainerBase {
    public:
        TrackStateAuxContainer_v1();
        std::vector<double> chi2;
        std::vector<double> pathLength;
        std::vector<TrackStateIndexType> previous;
        std::vector<TrackStateIndexType> predicted;
        std::vector<TrackStateIndexType> filtered;
        std::vector<TrackStateIndexType> smoothed;
        std::vector<TrackStateIndexType> jacobian;
        std::vector<TrackStateIndexType> projector;
        std::vector<TrackStateIndexType> uncalibrated;
        std::vector<TrackStateIndexType> calibrated;
        std::vector<TrackStateIndexType> measDim;
    };
}

#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::TrackStateAuxContainer_v1, xAOD::AuxContainerBase);


#endif