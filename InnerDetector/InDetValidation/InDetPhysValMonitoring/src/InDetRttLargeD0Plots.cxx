/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * @file InDetRttLargeD0Plots.cxx
 * @author shaun roe
**/

#include "InDetRttLargeD0Plots.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include <cmath> //std::isnan()
#include <limits>

using std::cout;
using std::endl;
using std::string;

namespace { 
  bool isLargeD0Track( const xAOD::TrackParticle& tp)
  {
    const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = tp.patternRecoInfo();
    if (patternReco.test(49)) return true;
    return false;
  }

}//namespace



InDetRttLargeD0Plots::InDetRttLargeD0Plots(InDetPlotBase* pParent, const std::string & sDir):InDetPlotBase(pParent, sDir),

    //=============================================================
    // plots for LargeD0 performance study
    //=============================================================

    // hits plots
    m_hitsPlots_nonfake_st(this,"LargeD0/HitsPlots/StandardTracks/NonFakeTracks"),
    m_hitsPlots_matching_truthLink_primary_st(this,"LargeD0/HitsPlots/StandardTracks/Matching/TruthLink/Primary"),
    m_hitsPlots_matching_truthLink_secondary_st(this,"LargeD0/HitsPlots/StandardTracks/Matching/TruthLink/Secondary"),
    m_hitsPlots_matching_NotruthLink_st(this,"LargeD0/HitsPlots/StandardTracks/Matching/NotruthLink"),
    m_hitsPlots_fake_st(this,"LargeD0/HitsPlots/StandardTracks/FakeTracks"),

    m_hitsPlots_nonfake_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/NonFakeTracks"),
    m_hitsPlots_matching_truthLink_primary_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/Matching/TruthLink/Primary"),
    m_hitsPlots_matching_truthLink_secondary_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/Matching/TruthLink/Secondary"),
    m_hitsPlots_matching_NotruthLink_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/Matching/NotruthLink"),
    m_hitsPlots_fake_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/FakeTracks"),

    // basic plots
    m_basicPlot_nonfake_st(this,"LargeD0/basicPlot/StandardTracks/NonFakeTracks"),
    m_basicPlot_nonfake_ld0(this,"LargeD0/basicPlot/LargeD0Tracks/NonFakeTracks"),
    m_basicPlot_fake_st(this,"LargeD0/basicPlot/StandardTracks/FakeTracks"),
    m_basicPlot_fake_ld0(this,"LargeD0/basicPlot/LargeD0Tracks/FakeTracks"),
    m_basicPlot_truth(this, "LargeD0/basicPlot/TruthTracks"),

    m_basicPlot_primary_truth(this, "LargeD0/basicPlot/TruthTracksPrimary"),
    m_basicPlot_secondary_truth(this, "LargeD0/basicPlot/TruthTracksSecondary"),

    m_basicPlot_pileup_primary_truth(this, "LargeD0/basicPlot/Pileup/TruthTracksPrimary"),
    m_basicPlot_pileup_secondary_truth(this, "LargeD0/basicPlot/Pileup/TruthTracksSecondary"),


    // pt plots
    m_ptPlot_nonfake_st(this,"LargeD0/ptPlot/StandardTracks/NonFakeTracks"),
    m_ptPlot_nonfake_ld0(this,"LargeD0/ptPlot/LargeD0Tracks/NonFakeTracks"),
    m_ptPlot_fake_st(this,"LargeD0/ptPlot/StandardTracks/FakeTracks"),
    m_ptPlot_fake_ld0(this,"LargeD0/ptPlot/LargeD0Tracks/FakeTracks"),

    m_ptPlot_primary(this,"LargeD0/ptPlot/TruthTracks/Primary"),
    m_ptPlot_secondary(this,"LargeD0/ptPlot/TruthTracks/Secondary"),
    m_ptPlot_pileup_primary(this,"LargeD0/ptPlot/Pileup/TruthTracks/Primary"),
    m_ptPlot_pileup_secondary(this,"LargeD0/ptPlot/Pileup/TruthTracks/Secondary"),

    // Reco info plots
    m_TrackRecoInfoPlots_nonfake_st(this,"LargeD0/TrackRecoInfoPlots/StandardTracks/NonFakeTracks"),
    m_TrackRecoInfoPlots_nonfake_ld0(this,"LargeD0/TrackRecoInfoPlots/LargeD0Tracks/NonFakeTracks"),
    m_TrackRecoInfoPlots_fake_st(this,"LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks"),
    m_TrackRecoInfoPlots_fake_ld0(this,"LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks"),

    // Fake rate plots
    m_fakePlots    (this,"LargeD0/FakeRatePlots/StandardTracks"),
    m_fakePlotsLRT (this,"LargeD0/FakeRatePlots/LargeD0Tracks"),

    m_fakePlots_noTruthLink    (this,"LargeD0/FakeRatePlots/StandardTracks/noTruthLink"),
    m_fakePlots_noTruthLink_matched_05    (this,"LargeD0/FakeRatePlots/StandardTracks/noTruthLink_matched_05"),
    m_fakePlots_noTruthLink_LRT    (this,"LargeD0/FakeRatePlots/LargeD0Tracks/noTruthLink"),
    m_fakePlots_noTruthLink_matched_05_LRT    (this,"LargeD0/FakeRatePlots/LargeD0Tracks/noTruthLink_matched_05"),

    // Efficiency plots
    m_effPlotsStd            (this, "LargeD0/EffPlots/StandardTracks/All"),
    m_effPlotsStdSignal      (this, "LargeD0/EffPlots/StandardTracks/Signal"),
    m_effPlotsLRT            (this, "LargeD0/EffPlots/LargeD0Tracks/All"),
    m_effPlotsLRTSignal      (this, "LargeD0/EffPlots/LargeD0Tracks/Signal"),
    m_effPlotsLRTnonStd      (this, "LargeD0/EffPlots/LargeD0TracksNonStd/All"),
    m_effPlotsLRTnonStdSignal(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal"),
    m_effPlotsAll            (this, "LargeD0/EffPlots/AllTracks/All"),
    m_effPlotsAllSignal      (this, "LargeD0/EffPlots/AllTracks/Signal"),
    m_effPlots               (this, "LargeD0/EffPlots"),

    //=============================================================
    // plots not used for LargeD0
    //=============================================================
    m_ptPlot(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    m_basicPlot(this, "LargeD0_NOTUSED/SelectedGoodTracks"),

    m_PtEtaPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks","TrackParticle"),
    m_IPPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks"),

    m_TrackRecoInfoPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    m_TrackTruthInfoPlots(this,"LargeD0_NOTUSED/Truth"),
    m_nTracks(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    m_resPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    m_hitResidualPlot(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
    m_hitsDetailedPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    // Efficiency plots
    m_BadMatchRate(this, "LargeD0_NOTUSED/SelectedFakeTracks"),
    m_verticesPlots(this, "LargeD0_NOTUSED/Vertices/AllPrimaryVertices"),
    m_vertexPlots(this,"LargeD0_NOTUSED/Vertices/AllPrimaryVertices"),
    m_hardScatterVertexPlots(this,"LargeD0_NOTUSED/Vertices/HardScatteringVertex"),
    //marked 'not useful'...
    m_hitsPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    m_hitsMatchedTracksPlots(this,"LargeD0_NOTUSED/SelectedMatchedTracks"),
    m_hitsFakeTracksPlots(this,"LargeD0_NOTUSED/SelectedFakeTracks")
{
  //These settings are probably all redundant & can be removed from this script
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbThreshold = 0.8;
  m_truthProbLowThreshold = 0.5;
  m_truthPrimaryEtaCut = 2.5;
}



//===================================================================================
// In the loop over all track particles
// filling tracks with truthMatching > minProbEffLow (truth-matched reco tracks)
//===================================================================================

void
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle& particle){

  m_hitResidualPlot.fill(particle);
  //fill pt plots
  m_ptPlot.fill(particle);
  m_basicPlot.fill(particle);
  m_PtEtaPlots.fill(particle);
  m_IPPlots.fill(particle);
  m_TrackRecoInfoPlots.fill(particle);
  m_hitsPlots.fill(particle);
  m_hitsDetailedPlots.fill(particle);
}

void
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle& particle, const int barcode){

  if (isLargeD0Track(particle)) {
    m_basicPlot_nonfake_ld0.fill(particle);
    m_ptPlot_nonfake_ld0.fill(particle);
    m_TrackRecoInfoPlots_nonfake_ld0.fill(particle);
    m_hitsPlots_nonfake_ld0.fill(particle);

    // primary tracks
    if ((barcode > 0) and (barcode <200000)) {
      m_hitsPlots_matching_truthLink_primary_ld0.fill(particle);
    }
    // secondary tracks
    if (barcode >= 200000) {
      m_hitsPlots_matching_truthLink_secondary_ld0.fill(particle);
    }
    // no truth link
    if (barcode == 0){
      m_hitsPlots_matching_NotruthLink_ld0.fill(particle);
    }
  }
  else {
    m_basicPlot_nonfake_st.fill(particle);
    m_ptPlot_nonfake_st.fill(particle);
    m_TrackRecoInfoPlots_nonfake_st.fill(particle);
    m_hitsPlots_nonfake_st.fill(particle);

    // primary tracks
    if ((barcode > 0) and (barcode <200000)) {
      m_hitsPlots_matching_truthLink_primary_st.fill(particle);
    }
    // secondary tracks
    if (barcode >= 200000) {
      m_hitsPlots_matching_truthLink_secondary_st.fill(particle);
    }
    // no truth link
    if (barcode == 0){
      m_hitsPlots_matching_NotruthLink_st.fill(particle);
    }

  }

}
//===================================================================================
// In the loop over all track particles
// filling track particle with truthMatching < minProbEffHigh (fake tracks)
//===================================================================================

void InDetRttLargeD0Plots::fillFake(const xAOD::TrackParticle& trackParticle){

  if (isLargeD0Track(trackParticle)) {
    m_hitsPlots_fake_ld0.fill(trackParticle);
    m_basicPlot_fake_ld0.fill(trackParticle);
    m_ptPlot_fake_ld0.fill(trackParticle);
    m_TrackRecoInfoPlots_fake_ld0.fill(trackParticle);
  }
  else {
    m_hitsPlots_fake_st.fill(trackParticle);
    m_basicPlot_fake_st.fill(trackParticle);
    m_ptPlot_fake_st.fill(trackParticle);
    m_TrackRecoInfoPlots_fake_st.fill(trackParticle);
  }

}

//===================================================================================
// In the loop over all truth particles
// filling all truth particle, no selection 
//===================================================================================

void
InDetRttLargeD0Plots::fillTruth(const xAOD::TruthParticle& truth){
  m_basicPlot_truth.fill(truth);

 if ((truth.barcode() > 0) and (truth.barcode() < 200000)) {
    m_basicPlot_primary_truth.fill(truth);
    m_ptPlot_primary.fill(truth);
 }
 else {
    m_basicPlot_secondary_truth.fill(truth);
    m_ptPlot_secondary.fill(truth);
 }

}

void
InDetRttLargeD0Plots::pro_fill(const xAOD::TruthParticle& truth, float weight){
  m_effPlots.pro_fill(truth, weight);
}


void
InDetRttLargeD0Plots::fillEfficiency (const xAOD::TruthParticle& truth, \
				      const bool isReconstructed,	\
				      const bool isLargeD0Track,	\
				      const bool isSignal) {

  unsigned int rec = (unsigned int) isReconstructed;
  unsigned int LRT = (unsigned int) isLargeD0Track;

  m_effPlotsAll.pro_fill(truth, rec);
  m_effPlotsStd.pro_fill(truth, rec and not LRT);
  m_effPlotsLRT.pro_fill(truth, rec and LRT);

  if(isSignal){
    m_effPlotsAllSignal.pro_fill(truth, rec);
    m_effPlotsStdSignal.pro_fill(truth, rec and not LRT);
    m_effPlotsLRTSignal.pro_fill(truth, rec and LRT);
  }

  //Not entirely sure what the LRTnonStd conditions should be

  /*
  m_effPlotsAll.fill(truth, isReconstructed);
  m_effPlotsStd.fill(truth, isReconstructed && isStandardTrack);
  m_effPlotsLRT.fill(truth, isReconstructed && isLargeD0Track);
  if (!(isStandardTrack && isReconstructed)) {
    m_effPlotsLRTnonStd.fill(truth, isReconstructed && isLargeD0Track);
  }
  
  if (isSignal) {
    m_effPlotsAllSignal.fill(truth, isReconstructed);
    m_effPlotsStdSignal.fill(truth, isReconstructed && isStandardTrack);
    m_effPlotsLRTSignal.fill(truth, isReconstructed && isLargeD0Track);
    if (!(isStandardTrack && isReconstructed)) {
      m_effPlotsLRTnonStdSignal.fill(truth, isReconstructed && isLargeD0Track);
    }
  }
  */
  return;
}

//===================================================================================
// In the loop over truth particles
// filling truth particle with truthMatching > minProbEffHigh (Truth-matched truth tracks)
//===================================================================================

void
InDetRttLargeD0Plots::fill(const xAOD::TruthParticle& truthParticle){
  //fill truth plots
  m_TrackTruthInfoPlots.fill(truthParticle);
  // this gets called for the truth particles associated to tracks - exactly what we need!
  //m_effPlots.fillNumerator(truthParticle); // @asogaard
}


void
InDetRttLargeD0Plots::fillBMR(const xAOD::TrackParticle& track, float weight){
  //fill the plot requiring truth matching probability less than 80%
  m_BadMatchRate.fillBMR(track, weight);
}

void
InDetRttLargeD0Plots::fillRF(const xAOD::TrackParticle& track, float weight){
  //fill the plots requiring truth matching probability less than 20%
  m_BadMatchRate.fillRF(track, weight);
}


void
InDetRttLargeD0Plots::fill(const xAOD::VertexContainer& vertexContainer){
  //fill vertex container general properties
  //m_verticesPlots.fill(vertexContainer); //if ever needed
  //fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()){
    if (vtx->vertexType() == xAOD::VxType::NoVtx) continue; //skip dummy vertex
    m_vertexPlots.fill(*vtx);
    if (vtx->vertexType() == xAOD::VxType::PriVtx) m_hardScatterVertexPlots.fill(*vtx);
  }
}

void
InDetRttLargeD0Plots::fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei){
  m_verticesPlots.fill(vertexContainer, ei);
}

void
InDetRttLargeD0Plots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory  counter){
  m_nTracks.fill(freq,counter);
}

void
InDetRttLargeD0Plots::fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const bool hasTruthLink, const InDetPerfPlot_fakes::Category c){

  //unsigned int link = (unsigned int) hasTruthLink;


  if (isLargeD0Track(particle)) {
    if (hasTruthLink) m_fakePlotsLRT.fill(particle, match, c);
    m_fakePlots_noTruthLink_matched_05_LRT.fill(particle, !(match) and !(hasTruthLink), c);
    m_fakePlots_noTruthLink_LRT.fill(particle,!(hasTruthLink), c);
  } else {
    if (hasTruthLink) m_fakePlots.fill(particle, match, c);
    m_fakePlots_noTruthLink_matched_05.fill(particle, !(match) and !(hasTruthLink), c);
    m_fakePlots_noTruthLink.fill(particle, !(hasTruthLink), c);
  }
}
