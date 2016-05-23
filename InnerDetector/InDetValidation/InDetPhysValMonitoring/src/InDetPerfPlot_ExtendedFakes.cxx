/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_ExtendedFakes.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_ExtendedFakes.h"
#include <math.h> //for M_PI




InDetPerfPlot_ExtendedFakes::InDetPerfPlot_ExtendedFakes (InDetPlotBase* pParent, const std::string& sDir) :
  InDetPerfPlot_fakes(pParent, sDir)
{
  //nop
}

void 
InDetPerfPlot_ExtendedFakes::initializePlots () {
  InDetPerfPlot_fakes::initializePlots();

  //
  for (const std::string& var : m_variables) {
    std::cout << "<InDetPerfPlot_ExtendedFakes::initializePlots> Booking 'track_fakerate_vs_" << var << "'." << std::endl;
    book(m_HitInfoFakerates[var], "track_fakerate_vs_" + var);
  }


  book(m_KinematicFakerates["RfirstHit"], "track_fakerate_vs_RfirstHit");
  book(m_KinematicFakerates["ZfirstHit"], "track_fakerate_vs_ZfirstHit");

}

void 
InDetPerfPlot_ExtendedFakes::fill (const xAOD::TrackParticle& trk,const bool isFake, const InDetPerfPlot_fakes::Category& /*f*/){

  InDetPerfPlot_fakes::fill(trk, isFake);

  unsigned int fakeNum = (unsigned int) isFake;
  
  // "Kinematics".
  unsigned int index = 0;
  if (trk.indexOfParameterAtPosition(index, xAOD::FirstMeasurement)) {
    xAOD::CurvilinearParameters_t parameters = trk.trackParameters(index);
    double x = parameters(0);
    double y = parameters(1);
    double z = parameters(2);
    double R = sqrt(x*x + y*y);
    //double za = fabs(z); unused

    m_KinematicFakerates["RfirstHit"]->Fill(R, fakeNum);
    m_KinematicFakerates["ZfirstHit"]->Fill(z, fakeNum);
  }

  // Hit information.
  uint8_t iBLayerHits(0), iBLayerOutliers(0), iBLayerSplitHits(0), iBLayerSharedHits(0);
  uint8_t iPixHits(0), iPixHoles(0), iPixSharedHits(0), iPixOutliers(0), iPixContribLayers(0), iPixSplitHits(0), iPixGangedHits(0);
  uint8_t iSCTHits(0), iSCTHoles(0), iSCTDoubleHoles(0), iSCTSharedHits(0), iSCTOutliers(0);
  uint8_t iSiHits(0); 
  uint8_t iTRTHits(0), iTRTHighThresholdHits(0), iTRTOutliers(0), iTRTHighThresholdOutliers(0);
  
  
  if (trk.summaryValue(iBLayerHits,       xAOD::numberOfBLayerHits))       m_HitInfoFakerates["nBLayerHits"      ]->Fill(iBLayerHits,       fakeNum);
  if (trk.summaryValue(iBLayerOutliers,   xAOD::numberOfBLayerOutliers))   m_HitInfoFakerates["nBLayerOutliers"  ]->Fill(iBLayerOutliers,   fakeNum);
  if (trk.summaryValue(iBLayerSharedHits, xAOD::numberOfBLayerSharedHits)) m_HitInfoFakerates["nBLayerSharedHits"]->Fill(iBLayerSharedHits, fakeNum);
  if (trk.summaryValue(iBLayerSplitHits,  xAOD::numberOfBLayerSplitHits))  m_HitInfoFakerates["nBLayerSplitHits" ]->Fill(iBLayerSplitHits,  fakeNum);

  if (trk.summaryValue(iPixHits,          xAOD::numberOfPixelHits))          m_HitInfoFakerates["nPixHits"         ]->Fill(iPixHits, fakeNum);
  if (trk.summaryValue(iPixHoles,         xAOD::numberOfPixelHoles))         m_HitInfoFakerates["nPixHoles"        ]->Fill(iPixHoles, fakeNum);
  if (trk.summaryValue(iPixOutliers,      xAOD::numberOfPixelOutliers))      m_HitInfoFakerates["nPixOutliers"     ]->Fill(iPixOutliers, fakeNum);
  if (trk.summaryValue(iPixContribLayers, xAOD::numberOfContribPixelLayers)) m_HitInfoFakerates["nPixContribLayers"]->Fill(iPixContribLayers, fakeNum);
  if (trk.summaryValue(iPixSharedHits,    xAOD::numberOfPixelSharedHits))    m_HitInfoFakerates["nPixSharedHits"   ]->Fill(iPixSharedHits, fakeNum);
  if (trk.summaryValue(iPixSplitHits,     xAOD::numberOfPixelSplitHits))     m_HitInfoFakerates["nPixSplitHits"    ]->Fill(iPixSplitHits, fakeNum);
  if (trk.summaryValue(iPixGangedHits,    xAOD::numberOfGangedPixels))       m_HitInfoFakerates["nPixGangedHits"   ]->Fill(iPixGangedHits, fakeNum);
  
  if (trk.summaryValue(iSCTHits,        xAOD::numberOfSCTHits))        m_HitInfoFakerates["nSCTHits"       ]->Fill(iSCTHits, fakeNum);
  if (trk.summaryValue(iSCTHoles,       xAOD::numberOfSCTHoles))       m_HitInfoFakerates["nSCTHoles"      ]->Fill(iSCTHoles, fakeNum);
  if (trk.summaryValue(iSCTOutliers,    xAOD::numberOfSCTOutliers))    m_HitInfoFakerates["nSCTOutliers"   ]->Fill(iSCTOutliers, fakeNum);
  if (trk.summaryValue(iSCTDoubleHoles, xAOD::numberOfSCTDoubleHoles)) m_HitInfoFakerates["nSCTDoubleHoles"]->Fill(iSCTDoubleHoles, fakeNum);
  if (trk.summaryValue(iSCTSharedHits,  xAOD::numberOfSCTSharedHits))  m_HitInfoFakerates["nSCTSharedHits" ]->Fill(iSCTSharedHits, fakeNum);

  iSiHits = iPixHits + iSCTHits;  
  m_HitInfoFakerates["nSiHits"]->Fill(iSiHits, fakeNum);

  if (trk.summaryValue(iTRTOutliers,              xAOD::numberOfTRTOutliers))              m_HitInfoFakerates["nTRTOutliers"             ]->Fill(iTRTOutliers,              fakeNum);
  if (trk.summaryValue(iTRTHighThresholdOutliers, xAOD::numberOfTRTHighThresholdOutliers)) m_HitInfoFakerates["nTRTHighThresholdOutliers"]->Fill(iTRTHighThresholdOutliers, fakeNum);
  if (trk.summaryValue(iTRTHits,                  xAOD::numberOfTRTHits))                  m_HitInfoFakerates["nTRTHits"                 ]->Fill(iTRTHits,                  fakeNum);
  if (trk.summaryValue(iTRTHighThresholdHits,     xAOD::numberOfTRTHighThresholdHits))     m_HitInfoFakerates["nTRTHighThresholdHits"    ]->Fill(iTRTHighThresholdHits,     fakeNum);

  

//m_track_fakerate_vs_nPixHits->Fill(nPixHits, fakeNum);

}
