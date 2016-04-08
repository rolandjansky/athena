/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigParticle/TrigElectron.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

void CombLinksDef::buildKnownCombLinksDefs3()
{
  //----------------------------------------------------------------
  // e
  //----------------------------------------------------------------
  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_e);
  x->addL1RoI("EmTau_ROI");
//   x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT", "initialRoI");
  x->addFeature<TrigRoiDescriptor, TrigRoiDescriptorCollection>("HLT", "TrigT2CaloEgamma");
  x->addFeature<TrigEMCluster, TrigEMClusterContainer>("HLT_TrigT2CaloEgamma");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigSiTrack_eGamma", 
							      "TrigSiTrack_eGamma");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigIDSCAN_eGamma", 
							      "TrigIDSCAN_eGamma");
  x->addContFeature<TrigInDetTrackCollection, TrigInDetTrack>("HLT_TrigL2SiTrackFinder_eGamma", 
							      "TrigL2SiTrackFinder_eGamma");
  x->addContFeature<TrigElectronContainer, TrigElectron>("HLT_L2IDCaloFex");
  x->addContFeature<TrigPhotonContainer, TrigPhoton>("HLT_L2PhotonFex");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_e);
  x->addL1RoI("EmTau_ROI");
  x->addContFeature<Rec::TrackParticleContainer, Rec::TrackParticle>("HLT", "HLT_InDetTrigParticleCreation_Electron_EFID");
  x->addContFeature<egammaContainer, egamma>("HLT", "egamma_Electrons");
  x->addContFeature<egammaContainer, egamma>("HLT", "egamma_Photons");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;
}
