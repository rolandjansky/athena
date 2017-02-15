/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __HitsInfo_cxx__
#include "MuonPtCalibNtupleMaker/HitsInfo.h"

HitsInfo::HitsInfo() {

  Reset();

}

void HitsInfo::Reset() {

  m_NumberOfPrecisionLayers = 0;
  m_NumberOfPrecisionHoleLayers = 0;
  m_NumberOfPhiLayers = 0;
  m_NumberOfPhiHoleLayers = 0;
  m_NumberOfTriggerEtaLayers = 0;
  m_NumberOfTriggerEtaHoleLayers = 0;
  m_InnerSmallHits = 0;
  m_InnerLargeHits = 0;
  m_MiddleSmallHits = 0;
  m_MiddleLargeHits = 0;
  m_OuterSmallHits = 0;
  m_OuterLargeHits = 0;
  m_ExtendedSmallHits = 0;
  m_ExtendedLargeHits = 0;
  m_InnerSmallHoles = 0;
  m_InnerLargeHoles = 0;
  m_MiddleSmallHoles = 0;
  m_MiddleLargeHoles = 0;
  m_OuterSmallHoles = 0;
  m_OuterLargeHoles = 0;
  m_ExtendedSmallHoles = 0;
  m_ExtendedLargeHoles = 0;
  m_phiLayer1Hits = 0;
  m_phiLayer2Hits = 0;
  m_phiLayer3Hits = 0;
  m_phiLayer4Hits = 0;
  m_etaLayer1Hits = 0;
  m_etaLayer2Hits = 0;
  m_etaLayer3Hits = 0;
  m_etaLayer4Hits = 0;
  m_phiLayer1Holes = 0;
  m_phiLayer2Holes = 0;
  m_phiLayer3Holes = 0;
  m_phiLayer4Holes = 0;
  m_etaLayer1Holes = 0;
  m_etaLayer2Holes = 0;
  m_etaLayer3Holes = 0;
  m_etaLayer4Holes = 0;
  m_NumberOfPixelHits = 0;
  m_NumberOfPixelDeadSensors = 0;
  m_NumberOfPixelHoles = 0;
  m_NumberOfSCTHits = 0;
  m_NumberOfSCTDeadSensors = 0;
  m_NumberOfSCTHoles = 0;
  m_NumberOfTRTHits = 0;
  m_NumberOfTRTOutliers = 0;

}

void HitsInfo::Register( TTree* t, std::string p ) {

  t->Branch( ( p + "_NumberOfPrecisionLayers" ).c_str(), &m_NumberOfPrecisionLayers );
  t->Branch( ( p + "_NumberOfPrecisionHoleLayers" ).c_str(), &m_NumberOfPrecisionHoleLayers );
  t->Branch( ( p + "_NumberOfPhiLayers" ).c_str(), &m_NumberOfPhiLayers );
  t->Branch( ( p + "_NumberOfPhiHoleLayers" ).c_str(), &m_NumberOfPhiHoleLayers );
  t->Branch( ( p + "_NumberOfTriggerEtaLayers" ).c_str(), &m_NumberOfTriggerEtaLayers );
  t->Branch( ( p + "_NumberOfTriggerEtaHoleLayers" ).c_str(), &m_NumberOfTriggerEtaHoleLayers );
  t->Branch( ( p + "_InnerSmallHits" ).c_str(), &m_InnerSmallHits );
  t->Branch( ( p + "_InnerLargeHits" ).c_str(), &m_InnerLargeHits );
  t->Branch( ( p + "_MiddleSmallHits" ).c_str(), &m_MiddleSmallHits );
  t->Branch( ( p + "_MiddleLargeHits" ).c_str(), &m_MiddleLargeHits );
  t->Branch( ( p + "_OuterSmallHits" ).c_str(), &m_OuterSmallHits );
  t->Branch( ( p + "_OuterLargeHits" ).c_str(), &m_OuterLargeHits );
  t->Branch( ( p + "_ExtendedSmallHits" ).c_str(), &m_ExtendedSmallHits );
  t->Branch( ( p + "_ExtendedLargeHits" ).c_str(), &m_ExtendedLargeHits );
  t->Branch( ( p + "_InnerSmallHoles" ).c_str(), &m_InnerSmallHoles );
  t->Branch( ( p + "_InnerLargeHoles" ).c_str(), &m_InnerLargeHoles );
  t->Branch( ( p + "_MiddleSmallHoles" ).c_str(), &m_MiddleSmallHoles );
  t->Branch( ( p + "_MiddleLargeHoles" ).c_str(), &m_MiddleLargeHoles );
  t->Branch( ( p + "_OuterSmallHoles" ).c_str(), &m_OuterSmallHoles );
  t->Branch( ( p + "_OuterLargeHoles" ).c_str(), &m_OuterLargeHoles );
  t->Branch( ( p + "_ExtendedSmallHoles" ).c_str(), &m_ExtendedSmallHoles );
  t->Branch( ( p + "_ExtendedLargeHoles" ).c_str(), &m_ExtendedLargeHoles );
  t->Branch( ( p + "_phiLayer1Hits" ).c_str(), &m_phiLayer1Hits );
  t->Branch( ( p + "_phiLayer2Hits" ).c_str(), &m_phiLayer2Hits );
  t->Branch( ( p + "_phiLayer3Hits" ).c_str(), &m_phiLayer3Hits );
  t->Branch( ( p + "_phiLayer4Hits" ).c_str(), &m_phiLayer4Hits );
  t->Branch( ( p + "_etaLayer1Hits" ).c_str(), &m_etaLayer1Hits );
  t->Branch( ( p + "_etaLayer2Hits" ).c_str(), &m_etaLayer2Hits );
  t->Branch( ( p + "_etaLayer3Hits" ).c_str(), &m_etaLayer3Hits );
  t->Branch( ( p + "_etaLayer4Hits" ).c_str(), &m_etaLayer4Hits );
  t->Branch( ( p + "_phiLayer1Holes" ).c_str(), &m_phiLayer1Holes );
  t->Branch( ( p + "_phiLayer2Holes" ).c_str(), &m_phiLayer2Holes );
  t->Branch( ( p + "_phiLayer3Holes" ).c_str(), &m_phiLayer3Holes );
  t->Branch( ( p + "_phiLayer4Holes" ).c_str(), &m_phiLayer4Holes );
  t->Branch( ( p + "_etaLayer1Holes" ).c_str(), &m_etaLayer1Holes );
  t->Branch( ( p + "_etaLayer2Holes" ).c_str(), &m_etaLayer2Holes );
  t->Branch( ( p + "_etaLayer3Holes" ).c_str(), &m_etaLayer3Holes );
  t->Branch( ( p + "_etaLayer4Holes" ).c_str(), &m_etaLayer4Holes );
  t->Branch( ( p + "_NumberOfPixelHits" ).c_str(), &m_NumberOfPixelHits );
  t->Branch( ( p + "_NumberOfPixelDeadSensors" ).c_str(), &m_NumberOfPixelDeadSensors );
  t->Branch( ( p + "_NumberOfPixelHoles" ).c_str(), &m_NumberOfPixelHoles );
  t->Branch( ( p + "_NumberOfSCTHits" ).c_str(), &m_NumberOfSCTHits );
  t->Branch( ( p + "_NumberOfSCTDeadSensors" ).c_str(), &m_NumberOfSCTDeadSensors );
  t->Branch( ( p + "_NumberOfSCTHoles" ).c_str(), &m_NumberOfSCTHoles );
  t->Branch( ( p + "_NumberOfTRTHits" ).c_str(), &m_NumberOfTRTHits );
  t->Branch( ( p + "_NumberOfTRTOutliers" ).c_str(), &m_NumberOfTRTOutliers );

}

void HitsInfo::Fill( const xAOD::Muon* muon ) {
   
  m_NumberOfPrecisionLayers      = Get( muon, xAOD::SummaryType::numberOfPrecisionLayers );
  m_NumberOfPrecisionHoleLayers  = Get( muon, xAOD::SummaryType::numberOfPrecisionHoleLayers );
  m_NumberOfPhiLayers            = Get( muon, xAOD::SummaryType::numberOfPhiLayers );
  m_NumberOfPhiHoleLayers        = Get( muon, xAOD::SummaryType::numberOfPhiHoleLayers );
  m_NumberOfTriggerEtaLayers     = Get( muon, xAOD::SummaryType::numberOfTriggerEtaLayers );
  m_NumberOfTriggerEtaHoleLayers = Get( muon, xAOD::SummaryType::numberOfTriggerEtaHoleLayers );
  m_InnerSmallHits               = Get( muon, xAOD::MuonSummaryType::innerSmallHits );
  m_InnerLargeHits               = Get( muon, xAOD::MuonSummaryType::innerLargeHits );
  m_MiddleSmallHits              = Get( muon, xAOD::MuonSummaryType::middleSmallHits );
  m_MiddleLargeHits              = Get( muon, xAOD::MuonSummaryType::middleLargeHits );
  m_OuterSmallHits               = Get( muon, xAOD::MuonSummaryType::outerSmallHits );
  m_OuterLargeHits               = Get( muon, xAOD::MuonSummaryType::outerLargeHits );
  m_ExtendedSmallHits            = Get( muon, xAOD::MuonSummaryType::extendedSmallHits );
  m_ExtendedLargeHits            = Get( muon, xAOD::MuonSummaryType::extendedLargeHits );
  m_InnerSmallHoles               = Get( muon, xAOD::MuonSummaryType::innerSmallHoles );
  m_InnerLargeHoles               = Get( muon, xAOD::MuonSummaryType::innerLargeHoles );
  m_MiddleSmallHoles              = Get( muon, xAOD::MuonSummaryType::middleSmallHoles );
  m_MiddleLargeHoles              = Get( muon, xAOD::MuonSummaryType::middleLargeHoles );
  m_OuterSmallHoles               = Get( muon, xAOD::MuonSummaryType::outerSmallHoles );
  m_OuterLargeHoles               = Get( muon, xAOD::MuonSummaryType::outerLargeHoles );
  m_ExtendedSmallHoles            = Get( muon, xAOD::MuonSummaryType::extendedSmallHoles );
  m_ExtendedLargeHoles            = Get( muon, xAOD::MuonSummaryType::extendedLargeHoles );
  m_phiLayer1Hits                = Get( muon, xAOD::MuonSummaryType::phiLayer1Hits );
  m_phiLayer2Hits                = Get( muon, xAOD::MuonSummaryType::phiLayer2Hits );
  m_phiLayer3Hits                = Get( muon, xAOD::MuonSummaryType::phiLayer3Hits );
  m_phiLayer4Hits                = Get( muon, xAOD::MuonSummaryType::phiLayer4Hits );
  m_etaLayer1Hits                = Get( muon, xAOD::MuonSummaryType::etaLayer1Hits );
  m_etaLayer2Hits                = Get( muon, xAOD::MuonSummaryType::etaLayer2Hits );
  m_etaLayer3Hits                = Get( muon, xAOD::MuonSummaryType::etaLayer3Hits );
  m_etaLayer4Hits                = Get( muon, xAOD::MuonSummaryType::etaLayer4Hits );
  m_phiLayer1Holes                = Get( muon, xAOD::MuonSummaryType::phiLayer1Holes );
  m_phiLayer2Holes                = Get( muon, xAOD::MuonSummaryType::phiLayer2Holes );
  m_phiLayer3Holes                = Get( muon, xAOD::MuonSummaryType::phiLayer3Holes );
  m_phiLayer4Holes                = Get( muon, xAOD::MuonSummaryType::phiLayer4Holes );
  m_etaLayer1Holes                = Get( muon, xAOD::MuonSummaryType::etaLayer1Holes );
  m_etaLayer2Holes                = Get( muon, xAOD::MuonSummaryType::etaLayer2Holes );
  m_etaLayer3Holes                = Get( muon, xAOD::MuonSummaryType::etaLayer3Holes );
  m_etaLayer4Holes                = Get( muon, xAOD::MuonSummaryType::etaLayer4Holes );
  m_NumberOfPixelHits            = Get( muon, xAOD::SummaryType::numberOfPixelHits );
  m_NumberOfPixelDeadSensors     = Get( muon, xAOD::SummaryType::numberOfPixelDeadSensors );
  m_NumberOfPixelHoles           = Get( muon, xAOD::SummaryType::numberOfPixelHoles );
  m_NumberOfSCTHits              = Get( muon, xAOD::SummaryType::numberOfSCTHits );
  m_NumberOfSCTDeadSensors       = Get( muon, xAOD::SummaryType::numberOfSCTDeadSensors );
  m_NumberOfSCTHoles             = Get( muon, xAOD::SummaryType::numberOfSCTHoles );
  m_NumberOfTRTHits              = Get( muon, xAOD::SummaryType::numberOfTRTHits );
  m_NumberOfTRTOutliers          = Get( muon, xAOD::SummaryType::numberOfTRTOutliers );

}

Int_t HitsInfo::Get( const xAOD::Muon* muon, const xAOD::SummaryType information ) {

  uint8_t temp_value = -1;
  muon->summaryValue( temp_value, information );
  return temp_value;

}

Int_t HitsInfo::Get( const xAOD::Muon* muon, const xAOD::MuonSummaryType information ) {

  uint8_t temp_value = -1;
  muon->summaryValue( temp_value, information );
  return temp_value;

}
