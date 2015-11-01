/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGMUONEVENT
#define TRIGSTORAGEDEF_TRIGMUONEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

namespace Muon{
  class MuonSegmentCombination;
  class MuonPatternCombination;
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
#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "xAODTrigMuon/L2IsoMuonAuxContainer.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(class MuonFeature, class  MuonFeature , class  MuonFeatureContainer)
  HLT_REGISTER_TYPE(class MuonFeatureDetails,class MuonFeatureDetails,class MuonFeatureDetailsContainer)
  HLT_REGISTER_TYPE(class CombinedMuonFeature, class CombinedMuonFeature,class CombinedMuonFeatureContainer)
  HLT_REGISTER_TYPE(class IsoMuonFeature, class  IsoMuonFeature, class IsoMuonFeatureContainer)
  HLT_REGISTER_TYPE(class TileMuFeature, class TileMuFeature, class TileMuFeatureContainer)
  HLT_REGISTER_TYPE(class TileTrackMuFeature,class TileTrackMuFeature,class TileTrackMuFeatureContainer)
  HLT_REGISTER_TYPE(class TrigMuonClusterFeature,class TrigMuonClusterFeature,class TrigMuonClusterFeatureContainer)
  
 // LVL1 
 //  HLT_REGISTER_TYPE( LVL1::RecMuonRoI, DataVector<LVL1::RecMuonRoI> ) 
 // EF 
  HLT_REGISTER_TYPE(class TrigMuonEF,class  TrigMuonEFContainer,class TrigMuonEFContainer )
  HLT_REGISTER_TYPE(class TrigMuonEFInfo, class TrigMuonEFInfoContainer, class TrigMuonEFInfoContainer )
  HLT_REGISTER_TYPE(class TrigMuonEFIsolation, class TrigMuonEFIsolationContainer,class TrigMuonEFIsolationContainer)
  
  HLT_REGISTER_TYPE(Muon::MuonSegmentCombination, MuonSegmentCombinationCollection, MuonSegmentCombinationCollection)
  HLT_REGISTER_TYPE(Muon::MuonPatternCombination, MuonPatternCombinationCollection, MuonPatternCombinationCollection)
  HLT_REGISTER_TYPE(class MuidTrack, class  MuidTrackContainer,class MuidTrackContainer)
  HLT_REGISTER_TYPE(int, CachingFeatureCollection, CachingFeatureCollection)
  // xAOD
  HLT_REGISTER_TYPE(xAOD::Muon, xAOD::MuonContainer, xAOD::MuonContainer, xAOD::MuonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::L2StandAloneMuon, xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuonContainer, xAOD::L2StandAloneMuonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::L2CombinedMuon, xAOD::L2CombinedMuonContainer, xAOD::L2CombinedMuonContainer, xAOD::L2CombinedMuonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::L2IsoMuon, xAOD::L2IsoMuonContainer, xAOD::L2IsoMuonContainer, xAOD::L2IsoMuonAuxContainer)
HLT_END_TYPE_REGISTRATION(TrigMuonEvent)

#endif
