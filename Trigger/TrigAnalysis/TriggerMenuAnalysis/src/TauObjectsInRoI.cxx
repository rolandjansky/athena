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
 HLTObjectsInRoI(), m_tauCluster(0), m_tau(0), m_EFTau(0) {
  defineHltFeature(m_tauCluster, 2, 1);
  defineHltFeature(m_tau, 2, 3);
  defineHltFeature(m_EFTau, 3, 3);
}

TauObjectsInRoI::~TauObjectsInRoI() {
}

