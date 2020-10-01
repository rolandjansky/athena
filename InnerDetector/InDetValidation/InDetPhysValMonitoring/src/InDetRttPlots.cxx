/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetRttPlots.cxx
 * @author shaun roe
 **/

#include "InDetRttPlots.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include <cmath> // std::isnan()
#include <limits>

InDetRttPlots::InDetRttPlots(InDetPlotBase* pParent, const std::string& sDir, const int iDetailLevel) : InDetPlotBase(pParent, sDir),
  m_trackParameters(this, "Tracks/Selected/Parameters"),
  m_nTracks(this, "Tracks/Tracks"),
  m_hitResidualPlot(this, "Tracks/Hits/Residuals"),
  m_hitEffPlot(this, "Tracks/Hits/Efficiency"),
  m_fakePlots(this, "Tracks/FakeRate"),
  m_missingTruthFakePlots(this, "Tracks/Unlinked/FakeRate"),
  m_resolutionPlotPrim(this, "Tracks/Matched/Resolutions/Primary"),
  m_hitsRecoTracksPlots(this, "Tracks/Selected/HitsOnTracks"),
  m_effPlots(this, "Tracks/Efficiency"),
  m_verticesVsMuPlots(this, "Vertices/AllPrimaryVertices"),
  m_vertexPlots(this, "Vertices/AllPrimaryVertices"),
  m_hardScatterVertexPlots(this, "Vertices/HardScatteringVertex"),
  m_hardScatterVertexTruthMatchingPlots(this, "Vertices/HardScatteringVertex"),
  m_trtExtensionPlots(this, "Tracks/TRTExtension"),
  m_anTrackingPlots(this, "Tracks/ANT"),
  m_resolutionPlotSecd(nullptr),
  m_doTrackInJetPlots(true),
  m_doTrackInBJetPlots(true) //FIX CONFIGURATION
{
  this->m_iDetailLevel = iDetailLevel;
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbLowThreshold = 0.5;
  
  if(m_iDetailLevel >= 200){
    m_resolutionPlotSecd = std::make_unique<InDetPerfPlot_Resolution>(this, "Tracks/Matched/Resolutions/Secondary");
    m_hitsMatchedTracksPlots = std::make_unique<InDetPerfPlot_Hits>(this, "Tracks/Matched/HitsOnTracks");
    m_hitsFakeTracksPlots = std::make_unique<InDetPerfPlot_Hits>(this, "Tracks/Fakes/HitsOnTracks");
    m_hitsUnlinkedTracksPlots = std::make_unique<InDetPerfPlot_Hits>(this, "Tracks/Unlinked/HitsOnTracks");
    m_vertexTruthMatchingPlots = std::make_unique<InDetPerfPlot_VertexTruthMatching>(this, "Vertices/AllPrimaryVertices", m_iDetailLevel);

    //Split by track author
    m_effSiSPSeededFinderPlots = std::make_unique<InDetPerfPlot_Efficiency>(this, "TracksByAuthor/SiSPSeededFinder/Tracks/Efficiency");
    m_effInDetExtensionProcessorPlots = std::make_unique<InDetPerfPlot_Efficiency>(this, "TracksByAuthor/InDetExtensionProcessor/Tracks/Efficiency");
    m_effTRTSeededTrackFinderPlots = std::make_unique<InDetPerfPlot_Efficiency>(this, "TracksByAuthor/TRTSeededTrackFinder/Tracks/Efficiency");
    m_effTRTStandalonePlots = std::make_unique<InDetPerfPlot_Efficiency>(this, "TracksByAuthor/TRTStandalone/Tracks/Efficiency");
    m_effSiSpacePointsSeedMaker_LargeD0Plots = std::make_unique<InDetPerfPlot_Efficiency>(this, "TracksByAuthor/SiSpacePointsSeedMaker_LargeD0/Tracks/Efficiency");

    m_fakeSiSPSeededFinderPlots = std::make_unique<InDetPerfPlot_FakeRate>(this, "TracksByAuthor/SiSPSeededFinder/Tracks/FakeRate");
    m_fakeInDetExtensionProcessorPlots = std::make_unique<InDetPerfPlot_FakeRate>(this, "TracksByAuthor/InDetExtensionProcessor/Tracks/FakeRate");
    m_fakeTRTSeededTrackFinderPlots = std::make_unique<InDetPerfPlot_FakeRate>(this, "TracksByAuthor/TRTSeededTrackFinder/Tracks/FakeRate");
    m_fakeTRTStandalonePlots = std::make_unique<InDetPerfPlot_FakeRate>(this, "TracksByAuthor/TRTStandalone/Tracks/FakeRate");
    m_fakeSiSpacePointsSeedMaker_LargeD0Plots = std::make_unique<InDetPerfPlot_FakeRate>(this, "TracksByAuthor/SiSpacePointsSeedMaker_LargeD0/Tracks/FakeRate");

    m_trkParaSiSPSeededFinderPlots = std::make_unique<InDetPerfPlot_TrackParameters>(this, "TracksByAuthor/SiSPSeededFinder/Tracks/Parameters");
    m_trkParaInDetExtensionProcessorPlots = std::make_unique<InDetPerfPlot_TrackParameters>(this, "TracksByAuthor/InDetExtensionProcessor/Tracks/Parameters");
    m_trkParaTRTSeededTrackFinderPlots = std::make_unique<InDetPerfPlot_TrackParameters>(this, "TracksByAuthor/TRTSeededTrackFinder/Tracks/Parameters");
    m_trkParaTRTStandalonePlots = std::make_unique<InDetPerfPlot_TrackParameters>(this, "TracksByAuthor/TRTStandalone/Tracks/Parameters");
    m_trkParaSiSpacePointsSeedMaker_LargeD0Plots = std::make_unique<InDetPerfPlot_TrackParameters>(this, "TracksByAuthor/SiSpacePointsSeedMaker_LargeD0/Tracks/Parameters");

    m_resSiSPSeededFinderPlots = std::make_unique<InDetPerfPlot_Resolution>(this, "TracksByAuthor/SiSPSeededFinder/Tracks/Resolution");
    m_resInDetExtensionProcessorPlots = std::make_unique<InDetPerfPlot_Resolution>(this, "TracksByAuthor/InDetExtensionProcessor/Tracks/Resolution");
    m_resTRTSeededTrackFinderPlots = std::make_unique<InDetPerfPlot_Resolution>(this, "TracksByAuthor/TRTSeededTrackFinder/Tracks/Resolution");
    m_resTRTStandalonePlots = std::make_unique<InDetPerfPlot_Resolution>(this, "TracksByAuthor/TRTStandalone/Tracks/Resolution");
    m_resSiSpacePointsSeedMaker_LargeD0Plots = std::make_unique<InDetPerfPlot_Resolution>(this, "TracksByAuthor/SiSpacePointsSeedMaker_LargeD0/Tracks/Resolution");

  }

  //A lot of Jets... do we need these at all???
  if(m_doTrackInJetPlots){
    m_trkInJetPlots = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInJets/Tracks");
    if(m_doTrackInBJetPlots){
      m_trkInJetPlots_bjets = std::make_unique<InDetPerfPlot_TrkInJet>(this, "TracksInBJets/Tracks");
    }
  }
}

//
//Fill plots for matched particles
//

void
InDetRttPlots::fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle) {
  // fill measurement bias, resolution, and pull plots

  // fill ITK resolutions (bias / resolutions)
  if (particle.isAvailable<float>(m_trackParticleTruthProbKey)) {
    const float prob = particle.auxdata<float>(m_trackParticleTruthProbKey);
    float barcode = truthParticle.barcode();
    if (barcode < 200000 && barcode != 0 && prob > 0.5) {
        m_resolutionPlotPrim.fill(particle, truthParticle);
    } else if (barcode >= 200000 && prob > 0.7 && m_iDetailLevel >= 200) {
        m_resolutionPlotSecd->fill(particle, truthParticle);
    }

    if(m_iDetailLevel >= 200 and (barcode < 200000 and barcode != 0 and prob > 0.5)){
      std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
    
      bool isSiSpSeededFinder = patternInfo.test(0);
      bool isInDetExtensionProcessor = patternInfo.test(3);
      bool isTRTSeededTrackFinder = patternInfo.test(4);
      bool isTRTStandalone = patternInfo.test(20);
      bool isSiSpacePointsSeedMaker_LargeD0 = patternInfo.test(49);

      if(isSiSpSeededFinder and not isInDetExtensionProcessor) m_resSiSPSeededFinderPlots->fill(particle, truthParticle);
      if(isInDetExtensionProcessor and not (isTRTSeededTrackFinder or isSiSpacePointsSeedMaker_LargeD0)) m_resInDetExtensionProcessorPlots->fill(particle, truthParticle);
      if(isTRTSeededTrackFinder and not isTRTStandalone) m_resTRTSeededTrackFinderPlots->fill(particle, truthParticle);
      if(isTRTStandalone) m_resTRTStandalonePlots->fill(particle, truthParticle);
      if(isSiSpacePointsSeedMaker_LargeD0) m_resSiSpacePointsSeedMaker_LargeD0Plots->fill(particle, truthParticle);

    }

    if (barcode < 200000 && barcode != 0 && prob > 0.5) m_trtExtensionPlots.fill(particle, truthParticle);


  }
 
  if(m_iDetailLevel >= 200){
    float barcode = truthParticle.barcode();
    if (barcode < 200000 && barcode != 0) { 
      m_hitsMatchedTracksPlots->fill(particle);
    }
  }
}

//
//Fill basic track properties for reconstructed tracks 
//

void
InDetRttPlots::fill(const xAOD::TrackParticle& particle) {
  m_hitResidualPlot.fill(particle);
  m_hitEffPlot.fill(particle);
  // fill pt plots
  m_trackParameters.fill(particle);
  m_anTrackingPlots.fill(particle);

  if(m_iDetailLevel >= 200){
    std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = particle.patternRecoInfo();
    
    bool isSiSpSeededFinder = patternInfo.test(0);
    bool isInDetExtensionProcessor = patternInfo.test(3);
    bool isTRTSeededTrackFinder = patternInfo.test(4);
    bool isTRTStandalone = patternInfo.test(20);
    bool isSiSpacePointsSeedMaker_LargeD0 = patternInfo.test(49);

    if(isSiSpSeededFinder and not isInDetExtensionProcessor) m_trkParaSiSPSeededFinderPlots->fill(particle);
    else if(isInDetExtensionProcessor and not (isTRTSeededTrackFinder or isSiSpacePointsSeedMaker_LargeD0)) m_trkParaInDetExtensionProcessorPlots->fill(particle);
    else if(isTRTSeededTrackFinder and not isTRTStandalone) m_trkParaTRTSeededTrackFinderPlots->fill(particle);
    else if(isTRTStandalone) m_trkParaTRTStandalonePlots->fill(particle);
    else if(isSiSpacePointsSeedMaker_LargeD0) m_trkParaSiSpacePointsSeedMaker_LargeD0Plots->fill(particle);

  }

  m_hitsRecoTracksPlots.fill(particle);
  m_trtExtensionPlots.fill(particle);
}

void
InDetRttPlots::fill(const xAOD::TrackParticle& particle, const float mu, const unsigned int nVtx) {

  m_trtExtensionPlots.fill(particle, mu, nVtx);

}

void
InDetRttPlots::fill(const unsigned int nTrkANT, const unsigned int nTrkSTD, const unsigned int nTrkBAT, const float mu, const unsigned int nVtx) { 

  m_anTrackingPlots.fill(nTrkANT, nTrkSTD, nTrkBAT, mu, nVtx);

}
//
//Fill plots for selected truth particle
//

void
InDetRttPlots::fill(const xAOD::TruthParticle& truthParticle) {
  // fill truth plots 
  m_trackParameters.fill(truthParticle);
}

//
//Fill Efficiencies
//

void
InDetRttPlots::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx) {
  m_effPlots.fill(truth, isGood);

  m_anTrackingPlots.fillEfficiency(truth, track, isGood, mu, nVtx);
  if(m_iDetailLevel >= 200){
    if(isGood){
      std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
    
      bool isSiSpSeededFinder = patternInfo.test(0);
      bool isInDetExtensionProcessor = patternInfo.test(3);
      bool isTRTSeededTrackFinder = patternInfo.test(4);
      bool isTRTStandalone = patternInfo.test(20);
      bool isSiSpacePointsSeedMaker_LargeD0 = patternInfo.test(49);

      if(isSiSpSeededFinder and not isInDetExtensionProcessor) m_effSiSPSeededFinderPlots->fill(truth, isGood);
      if(isInDetExtensionProcessor and not (isTRTSeededTrackFinder or isSiSpacePointsSeedMaker_LargeD0)) m_effInDetExtensionProcessorPlots->fill(truth, isGood);
      if(isTRTSeededTrackFinder and not isTRTStandalone) m_effTRTSeededTrackFinderPlots->fill(truth, isGood);
      if(isTRTStandalone) m_effTRTStandalonePlots->fill(truth, isGood);
      if(isSiSpacePointsSeedMaker_LargeD0) m_effSiSpacePointsSeedMaker_LargeD0Plots->fill(truth, isGood);
    } else {
      m_effSiSPSeededFinderPlots->fill(truth, isGood);
      m_effInDetExtensionProcessorPlots->fill(truth, isGood);
      m_effTRTSeededTrackFinderPlots->fill(truth, isGood);
      m_effTRTStandalonePlots->fill(truth, isGood);
      m_effSiSpacePointsSeedMaker_LargeD0Plots->fill(truth, isGood);

    }
    
  }

}

//
//Fill Fake Rates
//

void
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& track, const bool isFake, const bool isAssociatedTruth, const float mu, const unsigned int nVtx){

  m_missingTruthFakePlots.fill(track, !isAssociatedTruth);
  m_anTrackingPlots.fillUnlinked(track, !isAssociatedTruth, mu, nVtx);
  if(m_iDetailLevel >= 200){
    if (!isAssociatedTruth) m_hitsUnlinkedTracksPlots->fill(track);
    else m_hitsFakeTracksPlots->fill(track);
  }
  if(isAssociatedTruth) {
    m_fakePlots.fill(track, isFake);
      m_anTrackingPlots.fillFakeRate(track, isFake, mu, nVtx);

    if(m_iDetailLevel >= 200){
      std::bitset<xAOD::TrackPatternRecoInfo::NumberOfTrackRecoInfo>  patternInfo = track.patternRecoInfo();
      
      bool isSiSpSeededFinder = patternInfo.test(0);
      bool isInDetExtensionProcessor = patternInfo.test(3);
      bool isTRTSeededTrackFinder = patternInfo.test(4);
      bool isTRTStandalone = patternInfo.test(20);
      bool isSiSpacePointsSeedMaker_LargeD0 = patternInfo.test(49);

      if(isSiSpSeededFinder and not isInDetExtensionProcessor) m_fakeSiSPSeededFinderPlots->fill(track, isFake); //No extensions 
      if(isInDetExtensionProcessor and not (isTRTSeededTrackFinder or isSiSpacePointsSeedMaker_LargeD0)) m_fakeInDetExtensionProcessorPlots->fill(track, isFake); //Extensions but not Back-tracking
      if(isTRTSeededTrackFinder and not isTRTStandalone) m_fakeTRTSeededTrackFinderPlots->fill(track, isFake); //BackTracking
      if(isTRTStandalone) m_fakeTRTStandalonePlots->fill(track, isFake); //TRT standalone
      if(isSiSpacePointsSeedMaker_LargeD0) m_fakeSiSpacePointsSeedMaker_LargeD0Plots->fill(track, isFake); //ANT
    }
  }

}



//
//Fill Vertexing Plots
//
void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices) {
  // fill vertex container general properties
  // m_verticesVsMuPlots.fill(vertexContainer); //if ever needed
  // fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) {
      ATH_MSG_DEBUG("IN InDetRttPlots::fill, found xAOD::VxType::NoVtx");
      continue; // skip dummy vertex
    }
    m_vertexPlots.fill(*vtx);
    ATH_MSG_DEBUG("IN InDetRttPlots::fill, filling for all vertices");
    if (vtx->vertexType() == xAOD::VxType::PriVtx) {
      m_hardScatterVertexPlots.fill(*vtx);
      m_hardScatterVertexTruthMatchingPlots.fill(*vtx);
      ATH_MSG_DEBUG("IN InDetRttPlots::fill, filling for all HS vertex");
    }
  }
  if(m_iDetailLevel >= 200){
    m_vertexTruthMatchingPlots->fill(vertexContainer, truthHSVertices, truthPUVertices);
  }
}


void
InDetRttPlots::fill(const xAOD::VertexContainer& vertexContainer, unsigned int nPU) {
  m_verticesVsMuPlots.fill(vertexContainer, nPU);
}

//
//Fill Counters
//
void
InDetRttPlots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter) {
  m_nTracks.fill(freq, counter);
}

//Track in Jet Plots
void
InDetRttPlots::fill(const xAOD::TrackParticle& track, const xAOD::Jet& jet, bool isBjet){
  m_trkInJetPlots->fill(track, jet);
  if(isBjet) m_trkInJetPlots_bjets->fill(track, jet);
}

void
InDetRttPlots::fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::Jet& jet, bool isEfficient, bool isBjet) {
  m_trkInJetPlots->fillEfficiency(truth, jet, isEfficient); 
  if(isBjet) m_trkInJetPlots_bjets->fillEfficiency(truth, jet, isEfficient);
}

void
InDetRttPlots::fillFakeRate(const xAOD::TrackParticle& track, const xAOD::Jet& jet, bool isFake, bool isBjet) {
   m_trkInJetPlots->fillFakeRate(track, jet, isFake); 
   if(isBjet) m_trkInJetPlots_bjets->fillFakeRate(track, jet, isFake); 
}

