/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVIEWS_TRACKVIEWVECTOR_H
#define TRKVIEWS_TRACKVIEWVECTOR_H
#include "TrkTrack/TrackCollection.h"
#include "AthContainers/ViewVector.h"
#include "xAODCore/CLASS_DEF.h"

typedef ViewVector<TrackCollection> TrackViewVector;
VIEWVECTOR_CLASS_DEF(TrackViewVector, 240208070)

#endif // TRKVIEWS_TRACKVIEWVECTOR_H
