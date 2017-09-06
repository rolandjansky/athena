/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_ExtendedFakes.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_ExtendedFakes.h"
#include <cmath> // for M_PI




InDetPerfPlot_ExtendedFakes::InDetPerfPlot_ExtendedFakes (InDetPlotBase* pParent, const std::string& sDir) :
  InDetPerfPlot_fakes(pParent, sDir) {
  // nop
}

void
InDetPerfPlot_ExtendedFakes::initializePlots() {
  InDetPerfPlot_fakes::initializePlots();

  //
  for (const std::string& var : m_variables) {
    book(m_HitInfoFakerates[var], "track_fakerate_vs_" + var);
  }


  book(m_KinematicFakerates["RfirstHit"], "track_fakerate_vs_RfirstHit");
  book(m_KinematicFakerates["ZfirstHit"], "track_fakerate_vs_ZfirstHit");
}

void
InDetPerfPlot_ExtendedFakes::fill(const xAOD::TrackParticle& trk, const bool isFake,
                                  const InDetPerfPlot_fakes::Category& /*f*/) {
  InDetPerfPlot_fakes::fill(trk, isFake);

  unsigned int fakeNum = (unsigned int) isFake;

  // "Kinematics".
  unsigned int index = 0;

  if (trk.indexOfParameterAtPosition(index, xAOD::FirstMeasurement)) {
    xAOD::CurvilinearParameters_t parameters = trk.trackParameters(index);
    double x = parameters(0);
    double y = parameters(1);
    double z = parameters(2);
    double R = std::sqrt(x * x + y * y);
    // double za = fabs(z); unused

    fillHisto(m_KinematicFakerates["RfirstHit"], R, fakeNum);
    fillHisto(m_KinematicFakerates["ZfirstHit"], z, fakeNum);
  }

  // Hit information.
  uint8_t iBLayerHits(0), iBLayerOutliers(0), iBLayerSplitHits(0), iBLayerSharedHits(0);
  uint8_t iPixHits(0), iPixHoles(0), iPixSharedHits(0), iPixOutliers(0), iPixContribLayers(0), iPixSplitHits(0),
  iPixGangedHits(0);
  uint8_t iSCTHits(0), iSCTHoles(0), iSCTDoubleHoles(0), iSCTSharedHits(0), iSCTOutliers(0);
  uint8_t iSiHits(0);
  uint8_t iTRTHits(0), iTRTHighThresholdHits(0), iTRTOutliers(0), iTRTHighThresholdOutliers(0);


  if (trk.summaryValue(iBLayerHits, xAOD::numberOfInnermostPixelLayerHits)) {
    fillHisto(m_HitInfoFakerates["nBLayerHits"      ], iBLayerHits, fakeNum);
  }
  if (trk.summaryValue(iBLayerOutliers, xAOD::numberOfInnermostPixelLayerOutliers)) {
    fillHisto(m_HitInfoFakerates["nBLayerOutliers"  ], iBLayerOutliers, fakeNum);
  }
  if (trk.summaryValue(iBLayerSharedHits, xAOD::numberOfInnermostPixelLayerSharedHits)) {
    fillHisto(m_HitInfoFakerates["nBLayerSharedHits"], iBLayerSharedHits, fakeNum);
  }
  if (trk.summaryValue(iBLayerSplitHits, xAOD::numberOfInnermostPixelLayerSplitHits)) {
    fillHisto(m_HitInfoFakerates["nBLayerSplitHits" ], iBLayerSplitHits, fakeNum);
  }

  if (trk.summaryValue(iPixHits, xAOD::numberOfPixelHits)) {
    fillHisto(m_HitInfoFakerates["nPixHits"         ], iPixHits, fakeNum);
  }
  if (trk.summaryValue(iPixHoles, xAOD::numberOfPixelHoles)) {
    fillHisto(m_HitInfoFakerates["nPixHoles"        ], iPixHoles, fakeNum);
  }
  if (trk.summaryValue(iPixOutliers, xAOD::numberOfPixelOutliers)) {
    fillHisto(m_HitInfoFakerates["nPixOutliers"     ], iPixOutliers, fakeNum);
  }
  if (trk.summaryValue(iPixContribLayers, xAOD::numberOfContribPixelLayers)) {
    fillHisto(m_HitInfoFakerates["nPixContribLayers"], iPixContribLayers, fakeNum);
  }
  if (trk.summaryValue(iPixSharedHits, xAOD::numberOfPixelSharedHits)) {
    fillHisto(m_HitInfoFakerates["nPixSharedHits"   ], iPixSharedHits, fakeNum);
  }
  if (trk.summaryValue(iPixSplitHits, xAOD::numberOfPixelSplitHits)) {
    fillHisto(m_HitInfoFakerates["nPixSplitHits"    ], iPixSplitHits, fakeNum);
  }
  if (trk.summaryValue(iPixGangedHits, xAOD::numberOfGangedPixels)) {
    fillHisto(m_HitInfoFakerates["nPixGangedHits"   ], iPixGangedHits, fakeNum);
  }

  if (trk.summaryValue(iSCTHits, xAOD::numberOfSCTHits)) {
    fillHisto(m_HitInfoFakerates["nSCTHits"       ], iSCTHits, fakeNum);
  }
  if (trk.summaryValue(iSCTHoles, xAOD::numberOfSCTHoles)) {
    fillHisto(m_HitInfoFakerates["nSCTHoles"      ], iSCTHoles, fakeNum);
  }
  if (trk.summaryValue(iSCTOutliers, xAOD::numberOfSCTOutliers)) {
    fillHisto(m_HitInfoFakerates["nSCTOutliers"   ], iSCTOutliers, fakeNum);
  }
  if (trk.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) {
    fillHisto(m_HitInfoFakerates["nSCTDoubleHoles"], iSCTDoubleHoles, fakeNum);
  }
  if (trk.summaryValue(iSCTSharedHits, xAOD::numberOfSCTSharedHits)) {
    fillHisto(m_HitInfoFakerates["nSCTSharedHits" ], iSCTSharedHits, fakeNum);
  }

  iSiHits = iPixHits + iSCTHits;
  fillHisto(m_HitInfoFakerates["nSiHits"], iSiHits, fakeNum);

  if (trk.summaryValue(iTRTOutliers, xAOD::numberOfTRTOutliers)) {
    fillHisto(m_HitInfoFakerates["nTRTOutliers"             ], iTRTOutliers, fakeNum);
  }
  if (trk.summaryValue(iTRTHighThresholdOutliers, xAOD::numberOfTRTHighThresholdOutliers)) {
    fillHisto(m_HitInfoFakerates["nTRTHighThresholdOutliers"], iTRTHighThresholdOutliers, fakeNum);
  }
  if (trk.summaryValue(iTRTHits, xAOD::numberOfTRTHits)) {
    fillHisto(m_HitInfoFakerates["nTRTHits"                 ], iTRTHits, fakeNum);
  }
  if (trk.summaryValue(iTRTHighThresholdHits, xAOD::numberOfTRTHighThresholdHits)) {
    fillHisto(m_HitInfoFakerates["nTRTHighThresholdHits"    ], iTRTHighThresholdHits, fakeNum);
  }



// m_track_fakerate_vs_nPixHits->Fill(nPixHits, fakeNum);
}
