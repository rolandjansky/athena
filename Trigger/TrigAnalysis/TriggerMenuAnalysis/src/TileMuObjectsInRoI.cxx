/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TileMuObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/TileMuObjectsInRoI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigNavigation/Navigation.h"
#include <iostream>
#include <vector>

using namespace std;

HLTObjectsInRoI* TileMuObjectsInRoI::create() {
  return new TileMuObjectsInRoI();
}

TileMuObjectsInRoI::TileMuObjectsInRoI() :
  HLTObjectsInRoI(), 
  mMuonRoI(0), 
  mTileMuFeature(0), mTileTrackMuFeature(0), 
  mVertexCollection(0), mInDetTrackCollection(0), 
  mEFMuon(0),
  mEFMuonInfo(0) {
  defineL1RoI(mMuonRoI, "Muon_ROI");
  defineHltFeature(mTileMuFeature, 2, 1, "TileMuFeature");
  defineHltFeature(mVertexCollection, 2, 2, "VertexCollection");
  defineHltFeature(mInDetTrackCollection, 2, 2, "InDetTrackCollection");
  defineHltFeature(mTileTrackMuFeature, 2, 2, "TileTrackMuFeature");
  defineHltFeature(mEFMuon, 3, 2, "EFMuon");
  defineHltFeature(mEFMuonInfo, 3, 2, "EFMuonInfo");
}

TileMuObjectsInRoI::~TileMuObjectsInRoI() {
}

