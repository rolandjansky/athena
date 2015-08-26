/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  JetObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/JetObjectsInRoI.h"
#include "TrigNavigation/Navigation.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "JetEvent/JetCollection.h"

using namespace std;


HLTObjectsInRoI* JetObjectsInRoI::create() {
  return new JetObjectsInRoI();
}


JetObjectsInRoI::JetObjectsInRoI() : HLTObjectsInRoI(),
 mL2Jet(0), mEFJet(0) {
  defineHltFeature(mL2Jet, 2, 1, "L2Jet");
  defineHltFeature(mEFJet, 3, 1, "EFJet");
}

JetObjectsInRoI::~JetObjectsInRoI() {
}

