/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigMuonEvent/TileMuFeature.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeature.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

void CombLinksDef::buildKnownCombLinksDefs2()
{
  // Tilemu
  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_TileMu);
  x->addL1RoI("Muon_ROI");
  x->addFeature<TileMuFeature, TileMuFeatureContainer>("HLT");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Tile", 
							      "TrigSiTrack_Tile");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Tile", 
							      "TrigIDSCAN_Tile");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrack_Tile", 
							      "TriL2gSiTrack_Tile");
  x->addFeature<TileTrackMuFeature, TileTrackMuFeatureContainer>("HLT");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_TileMu);
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrigMuonEFContainer, TrigMuonEF>("HLT");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_MuonEFInfo");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_eMuonEFInfo", "MuGirl");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_MuTagIMO_EF", "MuTagIMO_EF");

  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;
}
