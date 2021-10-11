// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkTrack/TrackStateOnSurfaceContainer.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2021
 * @brief DataVector types for TrackStateOnSurface.
 */


#ifndef TRKTRACK_TRACKSTATEONSURFACECONTAINER_H
#define TRKTRACK_TRACKSTATEONSURFACECONTAINER_H


#include "TrkTrack/TrackStateOnSurface.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/DataVectorWithAlloc.h"


namespace Trk {


using TrackStateOnSurfaceConstContainer = DataVector<const TrackStateOnSurface>;

using TrackStateOnSurfaceProtContainer = DataVectorWithAlloc<TrackStateOnSurfaceConstContainer>;


} // namespace Trk


#endif // not TRKTRACK_TRACKSTATEONSURFACECONTAINER_H
