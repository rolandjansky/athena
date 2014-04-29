/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKPARTICLEBASE_TRKTRACKPARTICLEBASECOLLECTION_H
#define TRKPARTICLEBASE_TRKTRACKPARTICLEBASECOLLECTION_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "TrkParticleBase/TrackParticleBase.h"

namespace Trk
{
  typedef DataVector<TrackParticleBase>  TrackParticleBaseCollection;
}

CLASS_DEF( Trk::TrackParticleBaseCollection, 1166776095, 1)

#endif


