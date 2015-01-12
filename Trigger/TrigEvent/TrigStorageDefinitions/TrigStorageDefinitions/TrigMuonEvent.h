/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGMUONEVENT
#define TRIGSTORAGEDEF_TRIGMUONEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

namespace Muon{
  struct MuonSegmentCombination;
  struct MuonPatternCombination;
}

typedef DataVector<Muon::MuonSegmentCombination> MuonSegmentCombinationCollection;
typedef DataVector<Muon::MuonPatternCombination> MuonPatternCombinationCollection;
typedef DataVector<int> CachingFeatureCollection;

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonAuxContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonAuxContainer.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct MuonFeature, struct  MuonFeature , struct  MuonFeatureContainer)
  HLT_REGISTER_TYPE(struct MuonFeatureDetails,struct MuonFeatureDetails,struct MuonFeatureDetailsContainer)
  HLT_REGISTER_TYPE(struct CombinedMuonFeature, struct CombinedMuonFeature,struct CombinedMuonFeatureContainer)
  HLT_REGISTER_TYPE(struct IsoMuonFeature, struct  IsoMuonFeature, struct IsoMuonFeatureContainer)
  HLT_REGISTER_TYPE(struct TileMuFeature, struct TileMuFeature, struct TileMuFeatureContainer)
  HLT_REGISTER_TYPE(struct TileTrackMuFeature,struct TileTrackMuFeature,struct TileTrackMuFeatureContainer)
  HLT_REGISTER_TYPE(struct TrigMuonClusterFeature,struct TrigMuonClusterFeature,struct TrigMuonClusterFeatureContainer)
  
 // LVL1 
 //  HLT_REGISTER_TYPE( LVL1::RecMuonRoI, DataVector<LVL1::RecMuonRoI> ) 
 // EF 
  HLT_REGISTER_TYPE(struct TrigMuonEF,struct  TrigMuonEFContainer,struct TrigMuonEFContainer )
  HLT_REGISTER_TYPE(struct TrigMuonEFInfo, struct TrigMuonEFInfoContainer, struct TrigMuonEFInfoContainer )
  HLT_REGISTER_TYPE(struct TrigMuonEFIsolation, struct TrigMuonEFIsolationContainer,struct TrigMuonEFIsolationContainer)
  
  HLT_REGISTER_TYPE(Muon::MuonSegmentCombination, MuonSegmentCombinationCollection, MuonSegmentCombinationCollection)
  HLT_REGISTER_TYPE(Muon::MuonPatternCombination, MuonPatternCombinationCollection, MuonPatternCombinationCollection)
  HLT_REGISTER_TYPE(struct MuidTrack, struct  MuidTrackContainer,struct MuidTrackContainer)
  HLT_REGISTER_TYPE(int, CachingFeatureCollection, CachingFeatureCollection)
  // xAOD
  HLT_REGISTER_TYPE(xAOD::Muon, xAOD::MuonContainer, xAOD::MuonContainer, xAOD::MuonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::L2StandAloneMuon, xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::L2CombinedMuon, xAOD::L2CombinedMuonContainer, xAOD::L2CombinedMuonContainer, xAOD::L2CombinedMuonAuxContainer)
HLT_END_TYPE_REGISTRATION(TrigMuonEvent)

#endif
