/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  L2MuonObjectsInRoI.cxx
*/
#include "TriggerMenuAnalysis/MuonObjectsInRoI.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonEvent/IsoMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigNavigation/Navigation.h"
#include <iostream>
#include <vector>

using namespace std;

HLTObjectsInRoI* MuonObjectsInRoI::create() {
  return new MuonObjectsInRoI();
}

MuonObjectsInRoI::MuonObjectsInRoI() :
  HLTObjectsInRoI(), 
  m_muonRoI(0), 
  m_muonFeature(0), m_muonFeatureDetails(0), m_vertexCollection(0), m_inDetTrackCollection(0), 
  m_combinedMuon(0), m_isoMuonFeature(0), m_EFMuon(0), m_EFMuonInfo(0) {
  // defineHltFeature(object, level, step, name)
  defineL1RoI(m_muonRoI, "Muon_ROI");
  defineHltFeature(m_muonFeature, 2, 1, "MuonFeature");
  defineHltFeature(m_muonFeatureDetails, 2, 1, "MuonFeatureDetails");
  defineHltFeature(m_vertexCollection, 2, 2, "VertexCollection");
  defineHltFeature(m_inDetTrackCollection, 2, 2, "InDetTrackCollection");
  defineHltFeature(m_combinedMuon, 2, 2, "CombinedMuonFeature");
  defineHltFeature(m_isoMuonFeature, 2, 3, "IsoMuonFeature");
  defineHltFeature(m_EFMuon, 3, 2, "EFMuon");
  defineHltFeature(m_EFMuonInfo, 3, 2, "EFMuonInfo");
}

MuonObjectsInRoI::~MuonObjectsInRoI() {
}

