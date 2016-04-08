/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticleContainer.h"

void CombLinksDef::buildKnownCombLinksDefs6()
{
  //----------------------------------------------------------------
  // b 
  //----------------------------------------------------------------
  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_b);
  x->addL1RoI("Jet_ROI");
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrigL2BjetContainer, TrigL2Bjet>("HLT_L2BjetFex", "");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Jet", 
							      "TrigSiTrack_Jet");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Jet", 
							      "TrigIDSCAN_Jet");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder", 
							      "TrigL2SiTrackFinder");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_Jet", 
							      "TrigL2SiTrackFinder_Jet");
  x->addContFeature<TrigVertexCollection, TrigVertex>("HLT_T2HistoPrmVtx");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_b);
  x->addL1RoI("Jet_ROI");
  x->addL1RoI("Muon_ROI");
  x->addContFeature<TrigEFBjetContainer, TrigEFBjet>("HLT_EFBjetFex", "");
  x->addContFeature<Rec::TrackParticleContainer, Rec::TrackParticle>("HLT_InDetTrigParticleCreation_Bjet_EFID");
  x->addContFeature<TrigVertexCollection, TrigVertex>("HLT_EFHistoPrmVtx");
  x->addContFeature<VxContainer, Trk::VxCandidate>("HLT_PrimVx", "");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;
}
