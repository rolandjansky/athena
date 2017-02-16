/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HitsInfo_h__
#define __HitsInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>

//::: EDM includes
#include "xAODMuon/Muon.h"

class HitsInfo { 
  
private:
  Int_t m_NumberOfPrecisionLayers, m_NumberOfPrecisionHoleLayers;
  Int_t m_NumberOfPhiLayers, m_NumberOfPhiHoleLayers;
  Int_t m_NumberOfTriggerEtaLayers, m_NumberOfTriggerEtaHoleLayers;
  Int_t m_InnerSmallHits, m_InnerLargeHits, m_InnerSmallHoles, m_InnerLargeHoles;
  Int_t m_MiddleSmallHits, m_MiddleLargeHits, m_MiddleSmallHoles, m_MiddleLargeHoles;
  Int_t m_OuterSmallHits, m_OuterLargeHits, m_OuterSmallHoles, m_OuterLargeHoles;
  Int_t m_ExtendedSmallHits, m_ExtendedLargeHits, m_ExtendedSmallHoles, m_ExtendedLargeHoles;
  Int_t m_phiLayer1Hits, m_phiLayer2Hits, m_phiLayer3Hits, m_phiLayer4Hits, m_phiLayer1Holes, m_phiLayer2Holes, m_phiLayer3Holes, m_phiLayer4Holes;
  Int_t m_etaLayer1Hits, m_etaLayer2Hits, m_etaLayer3Hits, m_etaLayer4Hits, m_etaLayer1Holes, m_etaLayer2Holes, m_etaLayer3Holes, m_etaLayer4Holes;
  Int_t m_NumberOfPixelHits, m_NumberOfPixelDeadSensors, m_NumberOfPixelHoles;
  Int_t m_NumberOfSCTHits, m_NumberOfSCTDeadSensors, m_NumberOfSCTHoles;
  Int_t m_NumberOfTRTHits, m_NumberOfTRTOutliers; 

public:
  HitsInfo();
  ~HitsInfo() {}

  void Reset();
  void Register( TTree* t, std::string p );
  void Fill( const xAOD::Muon* muon );
  Int_t Get( const xAOD::Muon* muon, const xAOD::SummaryType information );
  Int_t Get( const xAOD::Muon* muon, const xAOD::MuonSummaryType information );

};

#endif
