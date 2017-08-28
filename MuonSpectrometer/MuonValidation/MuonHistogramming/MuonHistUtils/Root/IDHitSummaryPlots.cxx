/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/IDHitSummaryPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

    /////////// 
HitFracTypePlots::HitFracTypePlots(PlotBase* pParent,std::string sHitType, std::string sHitLabel):
  PlotBase(pParent, ""), fracHits(NULL), fracHitsVsEta(NULL) , m_sHitType(sHitType), m_sHitLabel(sHitLabel)
{}  
void HitFracTypePlots::initializePlots()
{
  fracHits      = Book1D(m_sHitType, m_sHitLabel + ";" + m_sHitLabel + ";Entries", 30 ,0., 0.6);
  fracHitsVsEta = Book2D(m_sHitType + "vsEta", m_sHitLabel + ";" + m_sHitLabel + ";#eta;Entries", 30 , 0., 0.3, 54, -2.7, 2.7);
}
void HitFracTypePlots::fill(float fHits, float fEta)
{
  fracHits->Fill(fHits);
  fracHitsVsEta->Fill(fHits, fEta);
}  
  ///////////
  
IDHitSummaryPlots::IDHitSummaryPlots(PlotBase* pParent, std::string sDir):
    PlotBase(pParent, sDir)
  , nBLayerHitsIfExpected(this, "nBLayerHitsIfExpected", "B-Layer clusters",0,4)
  , nPixelHitsPlusDead(this, "nPixelHitsPlusDead","Pixel clusters",0,9)
  , nSCTHitsPlusDead(this, "nSCTHitsPlusDead","SCT clusters",0,20)
  , nTRTHitsPlusDead(this, "nTRTHitsPlusDead","TRT clusters",0,60)
  , nTRTHitsPlusOutliers(this, "nTRTHitsPlusOutliers","TRT hits+Outliers",0,60)
  , nPixSCTHoles(this, "nPixSCTHoles","Pix+SCT holes",0,5)
  , fPixelOutliers(this, "fPixelOutliers","Fraction of Pix Outliers")
  , fSCTOutliers(this, "fSCTOutliers","Fraction of SCT outliers")
  , fTRTOutliers(this, "fTRTOutliers","Fraction of TRT Outliers")
{}
  
void IDHitSummaryPlots::fill(const xAOD::TrackParticle& trk)
{
  float eta=trk.eta();
  float phi=trk.phi();
  
  uint8_t iBLayerHits(0),bExpectBLayerHit(0);
  trk.summaryValue(bExpectBLayerHit,xAOD::expectInnermostPixelLayerHit);
  trk.summaryValue(iBLayerHits,xAOD::numberOfInnermostPixelLayerHits);
  nBLayerHitsIfExpected.fill(bExpectBLayerHit*iBLayerHits,eta,phi);

  uint8_t iPixHits(0),iPixDead(0);
  trk.summaryValue(iPixHits,xAOD::numberOfPixelHits);
  trk.summaryValue(iPixDead,xAOD::numberOfPixelDeadSensors);
  nPixelHitsPlusDead.fill(iPixHits+iPixDead,eta,phi);

  uint8_t iSCTHits(0),iSCTDead(0);
  trk.summaryValue(iSCTHits,xAOD::numberOfSCTHits);
  trk.summaryValue(iSCTDead,xAOD::numberOfSCTDeadSensors);
  nSCTHitsPlusDead.fill(iSCTHits+iSCTDead,eta,phi);

  uint8_t iTRTHits(0),iTRTDead(0),iTRTOutliers(0);
  trk.summaryValue(iTRTHits,xAOD::numberOfTRTHits);
  trk.summaryValue(iTRTDead,xAOD::numberOfTRTDeadStraws);
  trk.summaryValue(iTRTOutliers,xAOD::numberOfTRTOutliers);
  nTRTHitsPlusDead.fill(iTRTHits+iTRTDead,eta,phi);
  nTRTHitsPlusOutliers.fill(iTRTHits+iTRTOutliers,eta,phi);

  uint8_t iPixHoles(0),iSCTHoles(0);
  trk.summaryValue(iPixHoles,xAOD::numberOfPixelHoles);
  trk.summaryValue(iSCTHoles,xAOD::numberOfSCTHoles);
  nPixSCTHoles.fill(iPixHoles+iSCTHoles,eta,phi);

  uint8_t iPixelOutliers(0);
  trk.summaryValue(iPixelOutliers,xAOD::numberOfPixelOutliers);
  float nPix=(int)iPixelOutliers+(int)iPixHits;
  float fracPixOutliers = (nPix>0)? 1.*((int)iPixelOutliers)/nPix : 0;
  fPixelOutliers.fill(fracPixOutliers,eta);

  uint8_t iSCTOutliers(0);
  trk.summaryValue(iSCTOutliers,xAOD::numberOfSCTOutliers);
  float nSCT=(int)iSCTOutliers+(int)iSCTHits;
  float fracSCTOutliers = (nSCT>0)? 1.*((int)iSCTOutliers)/nSCT : 0;
  fSCTOutliers.fill(fracSCTOutliers,eta);

  float nTRT=((int)iTRTOutliers)+((int)iTRTHits);
  float fracTRTOutliers = (nTRT>0)? ((int)iTRTOutliers)/nTRT : 0;
  fTRTOutliers.fill(fracTRTOutliers,eta);
}



}
