/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"

void CombLinksDef::buildKnownCombLinksDefs1()
{
  //----------------------------------------------------------------
  // mu 
  //----------------------------------------------------------------

  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_mu);
  x->addL1RoI("Muon_ROI");
  x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT", "forID");
  x->addFeature<MuonFeature, MuonFeatureContainer>("HLT");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Muon", 
							      "TrigSiTrack_Muon");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Muon", 
							      "TrigIDSCAN_Muon");
  x->addFeature<MuonFeatureDetails, MuonFeatureDetailsContainer>("HLT");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_Muon", 
							      "TrigL2SiTrackFinder_Muon");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_muonIso", 
							      "TrigL2SiTrackFinder_muonIso");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_muonIso", 
							      "TrigIDSCAN_muonIso");
  x->addFeature<CombinedMuonFeature, CombinedMuonFeatureContainer>("HLT");
  x->addFeature<IsoMuonFeature, IsoMuonFeatureContainer>("HLT");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_mu);
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrackCollection, Trk::Track>("HLT_InDetTrigTrackSlimmer_Muon_EFID", "InDetTrigTrackSlimmer_Muon_EFID");
  x->addContFeature<TrigMuonEFInfoContainer, TrigMuonEFInfo>("HLT_MuonEFInfo");
  x->addContFeature<TrigMuonEFIsolationContainer, TrigMuonEFIsolation>("HLT_MuonEFIsolation");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;
}
