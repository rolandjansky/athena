/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"
#include "TrigInDetEvent/TrigTrackCountsCollection.h"
#include "TrigInDetEvent/TrigVertexCountsCollection.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrkTrack/TrackCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"
#include "TrkTruthData/TrkTruthARA.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/TrackExtensionMap.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"
#include "VxSecVertex/VxSecVertexInfo.h" 

typedef DataVector<TrackExtensionMap> TrackExtensionMapContainer;
CLASS_DEF( TrackExtensionMapContainer , 1280217485 , 1 )

#include "TrigNavigation/TypeRegistration.h"
REGISTER_PACKAGE_WITH_NAVI(TrigInDetEvent)
