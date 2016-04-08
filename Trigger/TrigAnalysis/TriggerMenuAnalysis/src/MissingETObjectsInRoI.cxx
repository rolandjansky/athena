/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  MissingETObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/MissingETObjectsInRoI.h"
#include "TrigNavigation/Navigation.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include <iostream>

using namespace std;


HLTObjectsInRoI* MissingETObjectsInRoI::create() {
  return new MissingETObjectsInRoI();
}


MissingETObjectsInRoI::MissingETObjectsInRoI() : 
  HLTObjectsInRoI(),
  m_L2MissingET(0), m_EFMissingET(0) {
  defineHltFeature(m_L2MissingET, 2, 2);
  defineHltFeature(m_EFMissingET, 3, 2);
}

MissingETObjectsInRoI::~MissingETObjectsInRoI() {
}

