/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L2MuonObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigNavigation/Navigation.h"
#include <iostream>
#include <vector>

using namespace std;

HLTObjectsInRoI* MuonObjectsInRoI::create() {
  return new MuonObjectsInRoI();
}

MuonObjectsInRoI::MuonObjectsInRoI() :
  HLTObjectsInRoI(), 
  mMuonRoI(0), 
  mMuonFeature(0), mMuonFeatureDetails(0), mVertexCollection(0), mInDetTrackCollection(0), 
  mCombinedMuon(0), mIsoMuonFeature(0), mEFMuon(0), mEFMuonInfo(0) {
  // defineHltFeature(object, level, step, name)
  defineL1RoI(mMuonRoI, "Muon_ROI");
  defineHltFeature(mMuonFeature, 2, 1, "MuonFeature");
  defineHltFeature(mMuonFeatureDetails, 2, 1, "MuonFeatureDetails");
  defineHltFeature(mVertexCollection, 2, 2, "VertexCollection");
  defineHltFeature(mInDetTrackCollection, 2, 2, "InDetTrackCollection");
  defineHltFeature(mCombinedMuon, 2, 2, "CombinedMuonFeature");
  defineHltFeature(mIsoMuonFeature, 2, 3, "IsoMuonFeature");
  defineHltFeature(mEFMuon, 3, 2, "EFMuon");
  defineHltFeature(mEFMuonInfo, 3, 2, "EFMuonInfo");
}

MuonObjectsInRoI::~MuonObjectsInRoI() {
}

