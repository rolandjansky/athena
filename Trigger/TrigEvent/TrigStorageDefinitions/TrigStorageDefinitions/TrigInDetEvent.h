/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGINDETEVENT
#define STORAGEDEF_TRIGINDETEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"
#include "DataModel/DataVector.h"

namespace Trk{
  struct Track;
  struct VxCandidate;
  struct VxSecVertexInfo;
  struct Segment;
  typedef DataVector<Trk::Segment> SegmentCollection;
  typedef DataVector<Trk::VxSecVertexInfo> VxSecVertexInfoContainer;
}

namespace Rec{
  struct TrackParticle;
  struct TrackParticleContainer;
}

typedef DataVector<Trk::Track> TrackCollection;



typedef DataVector<struct PRD_MultiTruthCollection>  PRD_MultiTruthCollectionContainer;
typedef DataVector<struct DetailedTrackTruthCollection> DetailedTrackTruthCollectionContainer;
typedef DataVector<struct TrackTruthCollection> TrackTruthCollectionContainer;
typedef DataVector<struct TrackParticleTruthCollection> TrackParticleTruthCollectionContainer;
typedef DataVector<struct TrackExtensionMap> TrackExtensionMapContainer;


#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"

#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "xAODTrigMinBias/TrigSpacePointCountsContainer.h"
#include "xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h"


#include "xAODTrigMinBias/TrigHisto2D.h"
#include "xAODTrigMinBias/TrigHisto2DContainer.h"
#include "xAODTrigMinBias/TrigHisto2DAuxContainer.h"

#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "xAODTrigMinBias/TrigVertexCountsContainer.h"
#include "xAODTrigMinBias/TrigVertexCountsAuxContainer.h"

#include "xAODTrigMinBias/TrigTrackCounts.h"
#include "xAODTrigMinBias/TrigTrackCountsContainer.h"
#include "xAODTrigMinBias/TrigTrackCountsAuxContainer.h"


HLT_BEGIN_TYPE_REGISTRATION
     HLT_REGISTER_TYPE(struct TrigInDetTrack,struct TrigInDetTrack,struct TrigInDetTrackCollection)
     HLT_REGISTER_TYPE(struct TrigInDetTrack,struct TrigInDetTrackCollection,struct TrigInDetTrackCollection)
     HLT_REGISTER_TYPE(struct TrigTauTracksInfo,struct TrigTauTracksInfo,struct TrigTauTracksInfoCollection )
     HLT_REGISTER_TYPE(struct TrigVertex,struct TrigVertex,struct TrigVertexCollection )
     HLT_REGISTER_TYPE(struct TrigVertex, struct TrigVertexCollection,struct TrigVertexCollection )
     HLT_REGISTER_TYPE(struct TrigSpacePointCounts,struct  TrigSpacePointCounts,struct TrigSpacePointCountsCollection )
     HLT_REGISTER_TYPE(struct TrigTrtHitCounts,struct TrigTrtHitCounts,struct TrigTrtHitCountsCollection )
     HLT_REGISTER_TYPE(struct TrigTrackCounts,struct TrigTrackCounts,struct TrigTrackCountsCollection )
     HLT_REGISTER_TYPE(struct TrigVertexCounts,struct TrigVertexCounts,struct TrigVertexCountsCollection )
     HLT_REGISTER_TYPE(struct TrigInDetTrackFitPar, DataVector<TrigInDetTrackFitPar >, DataVector<TrigInDetTrackFitPar> )
  
// EF
     HLT_REGISTER_TYPE(Trk::Track, TrackCollection,TrackCollection)
     HLT_REGISTER_TYPE(Trk::VxCandidate,struct  VxContainer,struct VxContainer)
     HLT_REGISTER_TYPE(Trk::VxSecVertexInfo,  Trk::VxSecVertexInfo, Trk::VxSecVertexInfoContainer ) 
     HLT_REGISTER_TYPE(Trk::VxSecVertexInfo, Trk::VxSecVertexInfoContainer, Trk::VxSecVertexInfoContainer)
  
     HLT_REGISTER_TYPE(PRD_MultiTruthCollection, PRD_MultiTruthCollection,  PRD_MultiTruthCollectionContainer)
     HLT_REGISTER_TYPE(DetailedTrackTruthCollection, DetailedTrackTruthCollection,  DetailedTrackTruthCollectionContainer)
     HLT_REGISTER_TYPE(TrackTruthCollection, TrackTruthCollection,  TrackTruthCollectionContainer)
     HLT_REGISTER_TYPE(struct TrackParticleTruthCollection, struct  TrackParticleTruthCollection,TrackParticleTruthCollectionContainer)
     HLT_REGISTER_TYPE(Trk::Segment,  Trk::SegmentCollection, Trk::SegmentCollection )
   
     HLT_REGISTER_TYPE(TrackExtensionMap, TrackExtensionMap, TrackExtensionMapContainer )
     HLT_REGISTER_TYPE(Rec::TrackParticle,  Rec::TrackParticleContainer, Rec::TrackParticleContainer  )
     // xAOD 
     HLT_REGISTER_TYPE(xAOD::TrackParticle, xAOD::TrackParticleContainer, xAOD::TrackParticleContainer, xAOD::TrackParticleAuxContainer )
     HLT_REGISTER_TYPE(xAOD::Vertex, xAOD::VertexContainer, xAOD::VertexContainer, xAOD::VertexAuxContainer )
     HLT_REGISTER_TYPE(xAOD::TrigSpacePointCounts, xAOD::TrigSpacePointCounts, xAOD::TrigSpacePointCountsContainer, xAOD::TrigSpacePointCountsAuxContainer )
     HLT_REGISTER_TYPE(xAOD::TrigHisto2D, xAOD::TrigHisto2D, xAOD::TrigHisto2DContainer, xAOD::TrigHisto2DAuxContainer )
     HLT_REGISTER_TYPE(xAOD::TrigVertexCounts,xAOD::TrigVertexCounts,xAOD::TrigVertexCountsContainer,xAOD::TrigVertexCountsAuxContainer )
     HLT_REGISTER_TYPE(xAOD::TrigTrackCounts,xAOD::TrigTrackCounts,xAOD::TrigTrackCountsContainer, xAOD::TrigTrackCountsAuxContainer )

HLT_END_TYPE_REGISTRATION(TrigInDetEvent) 


#endif
