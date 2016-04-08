/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKROAD_TRKROADCOLLECTION_H
#define TRKROAD_TRKROADCOLLECTION_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TrkRoad/TrackRoad.h"

namespace Trk
{
    typedef DataVector<Trk::TrackRoad> TrackRoadCollection;
}

CLASS_DEF(Trk::TrackRoadCollection, 1112120524, 1)

#endif //TRKROAD_TRKROADCOLLECTION_H
