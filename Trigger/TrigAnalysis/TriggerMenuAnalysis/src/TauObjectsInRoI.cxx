/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TauObjectsInRoI.cxx
*/
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "TriggerMenuAnalysis/TauObjectsInRoI.h"
#include "TrigNavigation/Navigation.h"
#include <iostream>
#include <vector>

using namespace std;

HLTObjectsInRoI* TauObjectsInRoI::create() {
  return new TauObjectsInRoI();
}

TauObjectsInRoI::TauObjectsInRoI() :
 HLTObjectsInRoI(), mTauCluster(0), mTau(0), mEFTau(0) {
  defineHltFeature(mTauCluster, 2, 1);
  defineHltFeature(mTau, 2, 3);
  defineHltFeature(mEFTau, 3, 3);
}

TauObjectsInRoI::~TauObjectsInRoI() {
}

