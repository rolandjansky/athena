/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODTracking/versions/TrackState_v1.h"


namespace xAOD {
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, double, chi2, setChi2)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, double, pathLength, setPathLength)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, previous, setPrevious)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, predicted, setPredicted)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, filtered, setFiltered)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, smoothed, setSmoothed)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, jacobian, setJacobian)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, projector, setProjector)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, uncalibrated, setUncalibrated)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, calibrated, setCalibrated)
    AUXSTORE_PRIMITIVE_SETTER_AND_GETTER(TrackState_v1, TrackStateIndexType, measDim, setMeasDim)
}