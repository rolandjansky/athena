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
  m_muonRoI(0), 
  m_tileMuFeature(0), m_tileTrackMuFeature(0), 
  m_vertexCollection(0), m_inDetTrackCollection(0), 
  m_EFMuon(0),
  m_EFMuonInfo(0) {
  defineL1RoI(m_muonRoI, "Muon_ROI");
  defineHltFeature(m_tileMuFeature, 2, 1, "TileMuFeature");
  defineHltFeature(m_vertexCollection, 2, 2, "VertexCollection");
  defineHltFeature(m_inDetTrackCollection, 2, 2, "InDetTrackCollection");
  defineHltFeature(m_tileTrackMuFeature, 2, 2, "TileTrackMuFeature");
  defineHltFeature(m_EFMuon, 3, 2, "EFMuon");
  defineHltFeature(m_EFMuonInfo, 3, 2, "EFMuonInfo");
}

TileMuObjectsInRoI::~TileMuObjectsInRoI() {
}

