/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_TRACKCOLLECTION_H
#define TRKTRACK_TRACKCOLLECTION_H

#include "DataModel/DataVector.h"
#include "TrkTrack/Track.h"
#include "CLIDSvc/CLASS_DEF.h"

/**This typedef represents a collection of Trk::Track objects. 
It is a DataVector. It can be saved
to storegate and persistified using POOL*/


// namespace Trk { class Track; }

typedef DataVector<Trk::Track> TrackCollection;
CLASS_DEF(TrackCollection , 117682415 , 1 )

#endif
