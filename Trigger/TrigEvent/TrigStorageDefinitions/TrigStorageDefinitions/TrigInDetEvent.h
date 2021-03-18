/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGINDETEVENT
#define STORAGEDEF_TRIGINDETEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"
#include "AthContainers/DataVector.h"

namespace Trk{
  class Track;
  class VxCandidate;
  class VxSecVertexInfo;
  class Segment;
  class PRDtoTrackMap;
  typedef DataVector<Trk::Segment> SegmentCollection;
  typedef DataVector<Trk::VxSecVertexInfo> VxSecVertexInfoContainer;
}

namespace Rec{
  class TrackParticle;
  class TrackParticleContainer;
}

typedef DataVector<Trk::PRDtoTrackMap> PRDtoTrackMapContainer;
typedef DataVector<Trk::Track> TrackCollection;



typedef DataVector<class PRD_MultiTruthCollection>  PRD_MultiTruthCollectionContainer;
typedef DataVector<class DetailedTrackTruthCollection> DetailedTrackTruthCollectionContainer;
typedef DataVector<class TrackTruthCollection> TrackTruthCollectionContainer;
typedef DataVector<class TrackParticleTruthCollection> TrackParticleTruthCollectionContainer;
typedef DataVector<class TrackExtensionMap> TrackExtensionMapContainer;


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
     HLT_REGISTER_TYPE(class TrigInDetTrack,class TrigInDetTrack,class TrigInDetTrackCollection)
     HLT_REGISTER_TYPE(class TrigInDetTrack,class TrigInDetTrackCollection,class TrigInDetTrackCollection)
     HLT_REGISTER_TYPE(class TrigTauTracksInfo,class TrigTauTracksInfo,class TrigTauTracksInfoCollection )
     HLT_REGISTER_TYPE(class TrigVertex,class TrigVertex,class TrigVertexCollection )
     HLT_REGISTER_TYPE(class TrigVertex, class TrigVertexCollection,class TrigVertexCollection )
     HLT_REGISTER_TYPE(class TrigSpacePointCounts,class  TrigSpacePointCounts,class TrigSpacePointCountsCollection )
     HLT_REGISTER_TYPE(class TrigTrtHitCounts,class TrigTrtHitCounts,class TrigTrtHitCountsCollection )
     HLT_REGISTER_TYPE(class TrigTrackCounts,class TrigTrackCounts,class TrigTrackCountsCollection )
     HLT_REGISTER_TYPE(class TrigVertexCounts,class TrigVertexCounts,class TrigVertexCountsCollection )
     HLT_REGISTER_TYPE(class TrigInDetTrackFitPar, DataVector<TrigInDetTrackFitPar >, DataVector<TrigInDetTrackFitPar> )
  
// EF
     HLT_REGISTER_TYPE(Trk::Track, TrackCollection,TrackCollection)
     HLT_REGISTER_TYPE(Trk::VxCandidate,class  VxContainer,class VxContainer)
     HLT_REGISTER_TYPE(Trk::VxSecVertexInfo,  Trk::VxSecVertexInfo, Trk::VxSecVertexInfoContainer ) 
     HLT_REGISTER_TYPE(Trk::VxSecVertexInfo, Trk::VxSecVertexInfoContainer, Trk::VxSecVertexInfoContainer)
  
     HLT_REGISTER_TYPE(PRD_MultiTruthCollection, PRD_MultiTruthCollection,  PRD_MultiTruthCollectionContainer)
     HLT_REGISTER_TYPE(DetailedTrackTruthCollection, DetailedTrackTruthCollection,  DetailedTrackTruthCollectionContainer)
     HLT_REGISTER_TYPE(TrackTruthCollection, TrackTruthCollection,  TrackTruthCollectionContainer)
     HLT_REGISTER_TYPE(class TrackParticleTruthCollection, class  TrackParticleTruthCollection,TrackParticleTruthCollectionContainer)
     HLT_REGISTER_TYPE(Trk::Segment,  Trk::SegmentCollection, Trk::SegmentCollection )

     HLT_REGISTER_TYPE(Trk::PRDtoTrackMap, class Trk::PRDtoTrackMap, PRDtoTrackMapContainer )

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
