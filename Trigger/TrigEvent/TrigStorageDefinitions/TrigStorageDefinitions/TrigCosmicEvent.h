/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGCOSMICEVENT
#define STORAGEDEF_TRIGCOSMICEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct CosmicMuon, struct CosmicMuonCollection,struct CosmicMuonCollection)
  HLT_REGISTER_TYPE(struct MdtTrackSegment, struct MdtTrackSegmentCollection,struct MdtTrackSegmentCollection)
  HLT_REGISTER_TYPE(struct CombinedIDHalfTracksObject, struct CombinedIDHalfTracksObjectCollection,struct CombinedIDHalfTracksObjectCollection)
  HLT_REGISTER_TYPE(struct CombinedIDMuonObject, struct CombinedIDMuonObjectCollection, struct CombinedIDMuonObjectCollection)
  HLT_REGISTER_TYPE(struct CombinedIDHalfTrackMuonObject,DataVector<CombinedIDHalfTrackMuonObject>,DataVector<CombinedIDHalfTrackMuonObject>)
HLT_END_TYPE_REGISTRATION(TrigCosmicEvent)

#endif
