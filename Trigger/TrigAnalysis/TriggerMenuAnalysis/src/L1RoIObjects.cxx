/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L1RoIObjects.cxx
*/
#include "TriggerMenuAnalysis/L1RoIObjects.h"


using namespace std;

L1RoIObjects::L1RoIObjects() {
}

L1RoIObjects::~L1RoIObjects() {
}

void L1RoIObjects::setMuonRoIs(const std::vector<Muon_ROI>& x) {
  std::vector<Muon_ROI>::const_iterator p;
  for (p=x.begin(); p!=x.end(); ++p) {
    addMuonRoI( &(*p));
  }
}

void L1RoIObjects::setEmTauRoIs(const std::vector<EmTau_ROI>& x) {
  std::vector<EmTau_ROI>::const_iterator p;
  for (p=x.begin(); p!=x.end(); ++p) {
    addEmTauRoI( &(*p));
  }
}

void L1RoIObjects::setJetRoIs(const std::vector<Jet_ROI>& x) {
  std::vector<Jet_ROI>::const_iterator p;
  for (p=x.begin(); p!=x.end(); ++p) {
    addJetRoI( &(*p));
  }
}

void L1RoIObjects::setJetEtRoIs(const std::vector<JetET_ROI>& x) {
  std::vector<JetET_ROI>::const_iterator p;
  for (p=x.begin(); p!=x.end(); ++p) {
    addJetEtRoI( &(*p));
  }
}

void L1RoIObjects::setEnergyRoIs(const std::vector<EnergySum_ROI>& x) {
  std::vector<EnergySum_ROI>::const_iterator p;
  for (p=x.begin(); p!=x.end(); ++p) {
    addEnergyRoI( &(*p));
  }
}
