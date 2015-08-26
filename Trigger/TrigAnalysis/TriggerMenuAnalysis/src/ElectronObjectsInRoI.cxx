/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  ElectronObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/ElectronObjectsInRoI.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigNavigation/Navigation.h"
#include "egammaEvent/egammaContainer.h"
#include "TrigParticle/TrigElectronContainer.h"

using namespace std;

HLTObjectsInRoI* ElectronObjectsInRoI::create() {
  return new ElectronObjectsInRoI();
}

ElectronObjectsInRoI::ElectronObjectsInRoI() : 
  HLTObjectsInRoI(), 
  mEMCluster(0), mVertexCollection(0), mInDetTrackCollection(0), 
  mTrigElectronContainer(0), 
  mEFegamma(0) {
  defineHltFeature(mEMCluster, 2, 1, "TrigEMCluster");
  defineHltFeature(mVertexCollection, 2, 2, "VertexCollection");
  defineHltFeature(mInDetTrackCollection, 2, 2);
  defineHltFeature(mTrigElectronContainer, 2, 3);
  defineHltFeature(mEFegamma, 3, 3);
}

ElectronObjectsInRoI::~ElectronObjectsInRoI() {
}

