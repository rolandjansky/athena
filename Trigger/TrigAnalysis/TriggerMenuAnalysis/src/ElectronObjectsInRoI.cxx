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
  m_EMCluster(0), m_vertexCollection(0), m_inDetTrackCollection(0), 
  m_trigElectronContainer(0), 
  m_EFegamma(0) {
  defineHltFeature(m_EMCluster, 2, 1, "TrigEMCluster");
  defineHltFeature(m_vertexCollection, 2, 2, "VertexCollection");
  defineHltFeature(m_inDetTrackCollection, 2, 2);
  defineHltFeature(m_trigElectronContainer, 2, 3);
  defineHltFeature(m_EFegamma, 3, 3);
}

ElectronObjectsInRoI::~ElectronObjectsInRoI() {
}

