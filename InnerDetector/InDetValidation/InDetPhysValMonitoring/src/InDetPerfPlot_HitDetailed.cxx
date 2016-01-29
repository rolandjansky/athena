/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetPerfPlot_HitDetailed.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

using std::cout;
using std::endl;

InDetPerfPlot_HitDetailed::InDetPerfPlot_HitDetailed(InDetPlotBase *pParent, std::string sDir):InDetPlotBase(pParent, sDir) {
  m_etaNBins = 20;
  m_etaMin = -2.5;
  m_etaMax = 2.5;
  m_etaMinDBM = 2.5;
  m_etaMaxDBM = 3.8;
}

void InDetPerfPlot_HitDetailed::setEtaBinning(int nbins, float eta_min, float eta_max) {
  m_etaNBins = nbins;
  m_etaMin = eta_min;
  m_etaMax = eta_max;
}

void InDetPerfPlot_HitDetailed::initializePlots(){  
  const bool prependDirectory(false);
  n_vs_eta_BLayerHits = BookTProfile("HitContent_vs_eta_NBlayerHits", 
				     "Number of B-Layer clusters;#eta;<Number of B-Layer clusters>",
				     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  n_vs_eta_PixelHits  = BookTProfile("HitContent_vs_eta_NPixelHits",
				     "Number of Pixel clusters;#eta; <Number of Pixel clusters>", 
				     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  n_vs_eta_DBMHitsNeg = BookTProfile("HitContent_vs_eta_NDBMHitsNeg",
				     "Number of DBM clusters;#eta; <Number of DBM clusters, - side>",
				     m_etaNBins, -1.0*m_etaMaxDBM, -1.0*m_etaMinDBM, 0, 100, prependDirectory );
  n_vs_eta_DBMHitsPos = BookTProfile("HitContent_vs_eta_NDBMHitsPos",
				     "Number of DBM clusters;#eta; <Number of DBM clusters, + side>",
				     m_etaNBins, m_etaMinDBM, m_etaMaxDBM, 0, 100, prependDirectory );
  n_vs_eta_PixelHoles = BookTProfile("HitContent_vs_eta_NPixelHoles",
				     "Number of Pixel holes;#eta;<Number of Pixel holes>", 
				     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  n_vs_eta_SCTHits    = BookTProfile("HitContent_vs_eta_NSCTHits",
				     "Number of SCT clusters;#eta;<Number of SCT Clusters>", 
				     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  n_vs_eta_SCTHoles   = BookTProfile("HitContent_vs_eta_NSCTHoles",
				     "Number of SCT holes;#eta;<Number of SCT Holes>", 
				     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  n_vs_eta_TRTHits    = BookTProfile("HitContent_vs_eta_NTRTHits",
				     "Number of TRT clusters;#eta;<Number of TRT Hits>", 
				     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  n_vs_eta_TRTHighThresholdHits = BookTProfile("HitContent_vs_eta_NTRTHighThresholdHits",
					       "Number of TRT high threshold Hits;#eta;<Number of Hight Thresh TRT Hits>", 
					       m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  tru_vs_eta_BLayer   = Book1D("truth_vs_eta_NBlayerHits",
			       "True Number of B-Layer clusters;#eta;<True Number of B-Layer clusters>",
			       m_etaNBins, m_etaMin, m_etaMax, prependDirectory );
  BLayer_efficiency   = Book1D("BLayer Efficiency", n_vs_eta_BLayerHits, "Cluster Efficiency of B-Layer;#eta;<Cluster Efficiency>", prependDirectory);


  //debug plots
  if (m_iDetailLevel >= 100) {
    n_vs_eta_BLayerOutliers   = BookTProfile("HitContent_vs_eta_NBlayerOutliers",
					     "Number of B-layer outliers;#eta;<Number of B-layer Outliers>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );  
    n_vs_eta_BLayerSharedHits = BookTProfile("HitContent_vs_eta_NBlayerSharedHits",
					     "Number of shared B-layer clusters;#eta;<Number of B-layer Shared Clusters>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_BLayerSplitHits  = BookTProfile("HitContent_vs_eta_NBlayerSplitHits", 
					     "Number of split B-layer clusters;#eta;<Number of B-layer Split Clusters>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_PixelOutliers    = BookTProfile("HitContent_vs_eta_NPixelOutliers",
					     "Number of Pixel outliers;#eta;<Number of Pixel Outliers>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_PixelContribLayers = BookTProfile("HitContent_vs_eta_NPixelContribLayers",
					       "Number of contributed Pixel layers;#eta;<Number of Pixel Layers>", 
					       m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_PixelSharedHits  = BookTProfile("HitContent_vs_eta_NPixelSharedHits",
					     "Number of shared Pixel clusters;#eta;<Number of Shared Pixel Clusters>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_PixelSplitHits   = BookTProfile("HitContent_vs_eta_NPixelSplitHits",
					     "Number of split Pixel clusters; #eta;<Number of Split Pixel Clusters>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_PixelGangedHits  = BookTProfile("HitContent_vs_eta_NPixelGangedHits",
					     "Number of ganged Pixel clusters;#eta;<Number of Ganged Pixel Clusters>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );    
    n_vs_eta_SCTOutliers      = BookTProfile("HitContent_vs_eta_NSCTOutliers",
					     "Number of SCT Outliers;#eta;<Number of SCT Outliers>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_SCTDoubleHoles   = BookTProfile("HitContent_vs_eta_NSCTDoubleHoles",
					     "Number of SCT double holes;#eta;<Number of SCT Double Holes>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_SCTSharedHits    = BookTProfile("HitContent_vs_eta_NSCTSharedHits",
					     "Number of shared SCT clusters;#eta;<Number of SCT Shared Clusters>", 
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_TRTOutliers      = BookTProfile("HitContent_vs_eta_NTRTOutliers",
					       "Number of TRT outliers;#eta;<Number of TRT Outliers>",
					     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
    n_vs_eta_TRTHighThresholdOutliers = BookTProfile("HitContent_vs_eta_NTRTHighThresholdOutliers",
						     "Number of TRT High Threshold outliers;#eta;<Number of High Thresh TRT Outliers>", 
						     m_etaNBins, m_etaMin, m_etaMax, 0, 100, prependDirectory );
  }
}

void InDetPerfPlot_HitDetailed::fill(const xAOD::TrackParticle& trk) {

  float eta = trk.eta();

  uint8_t iBLayerHits(0),iPixHits(0),iSctHits(0),iTrtHits(0);
  uint8_t iPixHoles(0),iSCTHoles(0),iTrtHTHits(0);  

  if (trk.summaryValue(iBLayerHits,xAOD::numberOfBLayerHits)) n_vs_eta_BLayerHits->Fill(eta, iBLayerHits);
  if (trk.summaryValue(iPixHits,xAOD::numberOfPixelHits))     n_vs_eta_PixelHits->Fill(eta, iPixHits);
  if (trk.summaryValue(iSctHits,xAOD::numberOfSCTHits))       n_vs_eta_SCTHits->Fill(eta, iSctHits);
  if (trk.summaryValue(iTrtHits,xAOD::numberOfTRTHits))       n_vs_eta_TRTHits->Fill(eta, iTrtHits);
  if (trk.summaryValue(iPixHoles,xAOD::numberOfPixelHoles))   n_vs_eta_PixelHoles->Fill(eta, iPixHoles);
  if (trk.summaryValue(iSCTHoles,xAOD::numberOfSCTHoles))     n_vs_eta_SCTHoles->Fill(eta, iSCTHoles);
  if (trk.summaryValue(iTrtHTHits,xAOD::numberOfTRTHighThresholdHits)) n_vs_eta_TRTHighThresholdHits->Fill(eta, iTrtHTHits);

  //expert plots
  if (m_iDetailLevel >= 100) {
    uint8_t iBLayerOutliers(0),iBLayerShared(0),iBLayerSplit(0);
    uint8_t iPixelOutliers(0),iPixelContribLayers(0),iPixelShared(0),iPixelSplit(0),iPixelGanged(0);
    uint8_t iSCTOutliers(0),iSCTDoubleHoles(0),iSCTShared(0);
    uint8_t iTRTOutliers(0),iTRTHTOutliers(0);
    if (trk.summaryValue(iBLayerOutliers,xAOD::numberOfBLayerOutliers)) n_vs_eta_BLayerOutliers->Fill(eta, iBLayerOutliers);
    if (trk.summaryValue(iBLayerShared,xAOD::numberOfBLayerSharedHits)) n_vs_eta_BLayerSharedHits->Fill(eta, iBLayerShared);
    if (trk.summaryValue(iBLayerSplit,xAOD::numberOfBLayerSplitHits)) n_vs_eta_BLayerSplitHits->Fill(eta, iBLayerSplit);
    if (trk.summaryValue(iPixelOutliers,xAOD::numberOfPixelOutliers)) n_vs_eta_PixelOutliers->Fill(eta, iPixelOutliers);
    if (trk.summaryValue(iPixelContribLayers,xAOD::numberOfContribPixelLayers)) n_vs_eta_PixelContribLayers->Fill(eta, iPixelContribLayers);
    if (trk.summaryValue(iPixelShared,xAOD::numberOfPixelSharedHits)) n_vs_eta_PixelSharedHits->Fill(eta, iPixelShared);
    if (trk.summaryValue(iPixelSplit,xAOD::numberOfPixelSplitHits)) n_vs_eta_PixelSplitHits->Fill(eta, iPixelSplit);
    if (trk.summaryValue(iPixelGanged,xAOD::numberOfGangedPixels)) n_vs_eta_PixelGangedHits->Fill(eta, iPixelGanged);
    if (trk.summaryValue(iSCTOutliers,xAOD::numberOfSCTOutliers)) n_vs_eta_PixelOutliers->Fill(eta, iSCTOutliers);
    if (trk.summaryValue(iSCTDoubleHoles,xAOD::numberOfSCTDoubleHoles)) n_vs_eta_SCTDoubleHoles->Fill(eta, iSCTDoubleHoles);
    if (trk.summaryValue(iSCTShared,xAOD::numberOfSCTSharedHits)) n_vs_eta_SCTSharedHits->Fill(eta, iSCTShared);
    if (trk.summaryValue(iTRTOutliers,xAOD::numberOfTRTOutliers)) n_vs_eta_TRTOutliers->Fill(eta, iTRTOutliers);
    if (trk.summaryValue(iTRTHTOutliers,xAOD::numberOfTRTHighThresholdOutliers)) n_vs_eta_TRTHighThresholdOutliers->Fill(eta, iTRTHTOutliers);
  }
	
}
/*
void InDetPerfPlot_HitDetailed::fillDenom(const xAOD::TruthParticle& truth) {

  float eta = truth.eta();
  //uint8_t truBLayerHits(1);
  //tru_vs_eta_BLayer->Fill(eta);

}


void InDetPerfPlot_HitDetailed::finalizePlots(){
  EfficiencyPurityCalculator calc;

  //Whatever this is doing it is NOT getting the right efficiencies
  //On the bright side, the x-axis is wrong in the same way as the good Eff plots,
  //so we know now what is causing it.
  //calc.calculateEfficiency(tru_vs_eta_BLayer, n_vs_eta_BLayerHits, BLayer_efficiency);


}
*/
