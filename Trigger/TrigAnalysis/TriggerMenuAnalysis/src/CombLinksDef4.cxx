/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigCaloEvent/TrigTauClusterDetails.h"
#include "TrigCaloEvent/TrigTauClusterDetailsContainer.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "tauEvent/TauJetContainer.h"

void CombLinksDef::buildKnownCombLinksDefs4()
{
  //----------------------------------------------------------------
  // tau 
  //----------------------------------------------------------------
  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_tau);
  x->addL1RoI("EmTau_ROI");
  x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT_initialRoI");
  x->addFeature<TrigTauCluster, TrigTauClusterContainer>("HLT_TrigT2CaloTau");
  x->addFeature<TrigTauClusterDetails, TrigTauClusterDetailsContainer>("HLT_TrigT2CaloTauDetails");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_Tau", 
							      "TrigSiTrack_Tau");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_Tau", 
							      "TrigIDSCAN_Tau");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_Tau",
							      "TrigL2SiTrackFinder_Tau");
  x->addFeature<TrigTauTracksInfo, TrigTauTracksInfoCollection>("HLT");
//   x->addContFeature<TrigVertexCollection, TrigVertex>("HLT_T2HistoPrmVtx");
  x->addFeature<TrigTau, TrigTauContainer>("HLT");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_tau);
  x->addContFeature<Analysis::TauJetContainer, Analysis::TauJet>("HLT");
  x->addL1RoI("EmTau_ROI");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

}
