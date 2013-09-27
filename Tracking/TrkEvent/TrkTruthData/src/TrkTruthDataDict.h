/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRUTHDATA_TRKTRUTHDATADICT_H
#define TRKTRUTHDATA_TRKTRUTHDATADICT_H

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TrackTruthKey.h"
#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/DetailedTrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "DataModel/DataLink.h"
#include <utility>
 
namespace 
{
  struct temp
  {
    std::pair<Trk::TrackTruthKey,TrackTruth> m_2;
    std::pair<Trk::TrackTruthKey, DetailedTrackTruth > m_4;    
    std::pair<const Trk::TrackTruthKey, DetailedTrackTruth > m_4c;
  };
}

#endif
