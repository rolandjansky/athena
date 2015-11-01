/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGCOSMICEVENT
#define STORAGEDEF_TRIGCOSMICEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(class CosmicMuon, class CosmicMuonCollection,class CosmicMuonCollection)
  HLT_REGISTER_TYPE(class MdtTrackSegment, class MdtTrackSegmentCollection,class MdtTrackSegmentCollection)
  HLT_REGISTER_TYPE(class CombinedIDHalfTracksObject, class CombinedIDHalfTracksObjectCollection,class CombinedIDHalfTracksObjectCollection)
  HLT_REGISTER_TYPE(class CombinedIDMuonObject, class CombinedIDMuonObjectCollection, class CombinedIDMuonObjectCollection)
  HLT_REGISTER_TYPE(class CombinedIDHalfTrackMuonObject,DataVector<CombinedIDHalfTrackMuonObject>,DataVector<CombinedIDHalfTrackMuonObject>)
HLT_END_TYPE_REGISTRATION(TrigCosmicEvent)

#endif
