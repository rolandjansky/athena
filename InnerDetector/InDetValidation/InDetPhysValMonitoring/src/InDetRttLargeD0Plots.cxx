/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
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
    m_hitsPlots_nonfake_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/NonFakeTracks"),
    m_hitsPlots_fake_st(this,"LargeD0/HitsPlots/StandardTracks/FakeTracks"),
    m_hitsPlots_fake_ld0(this,"LargeD0/HitsPlots/LargeD0Tracks/FakeTracks"),

    // basic plots
    m_basicPlot_nonfake_st(this,"LargeD0/basicPlot/StandardTracks/NonFakeTracks"),
    m_basicPlot_nonfake_ld0(this,"LargeD0/basicPlot/LargeD0Tracks/NonFakeTracks"),
    m_basicPlot_fake_st(this,"LargeD0/basicPlot/StandardTracks/FakeTracks"),
    m_basicPlot_fake_ld0(this,"LargeD0/basicPlot/LargeD0Tracks/FakeTracks"),
    m_basicPlot_truth(this, "LargeD0/basicPlot/TruthTracks/"),

    // pt plots
    m_ptPlot_nonfake_st(this,"LargeD0/ptPlot/StandardTracks/NonFakeTracks"),
    m_ptPlot_nonfake_ld0(this,"LargeD0/ptPlot/LargeD0Tracks/NonFakeTracks"),
    m_ptPlot_fake_st(this,"LargeD0/ptPlot/StandardTracks/FakeTracks"),
    m_ptPlot_fake_ld0(this,"LargeD0/ptPlot/LargeD0Tracks/FakeTracks"),

    // Reco info plots
    m_TrackRecoInfoPlots_nonfake_st(this,"LargeD0/TrackRecoInfoPlots/StandardTracks/NonFakeTracks"),
    m_TrackRecoInfoPlots_nonfake_ld0(this,"LargeD0/TrackRecoInfoPlots/LargeD0Tracks/NonFakeTracks"),
    m_TrackRecoInfoPlots_fake_st(this,"LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks"),
    m_TrackRecoInfoPlots_fake_ld0(this,"LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks"),

    // Fake rate (rather, fraction) plots
    m_fakePlots(this,"LargeD0/FakeRatePlots/StandardTracks"),
    m_fakePlotsLRT(this,"LargeD0/FakeRatePlots/LargeD0Tracks"),

    

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
    m_effPlots(this, "LargeD0/EffPlots","SelectedTracks"),
    m_BadMatchRate(this, "LargeD0_NOTUSED/SelectedFakeTracks"),
    m_verticesPlots(this, "LargeD0_NOTUSED/Vertices/AllPrimaryVertices"),
    m_vertexPlots(this,"LargeD0_NOTUSED/Vertices/AllPrimaryVertices"),
    m_hardScatterVertexPlots(this,"LargeD0_NOTUSED/Vertices/HardScatteringVertex"),
    //marked 'not useful'...
    m_hitsPlots(this,"LargeD0_NOTUSED/SelectedGoodTracks"),
    m_hitsMatchedTracksPlots(this,"LargeD0_NOTUSED/SelectedMatchedTracks"),
    m_hitsFakeTracksPlots(this,"LargeD0_NOTUSED/SelectedFakeTracks")
    //m_fakePlots(this,"LargeD0_NOTUSED/SelectedFakeTracks")
                                                                         //m_DuplicateTrack(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
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
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle& particle,const xAOD::TruthParticle& truthParticle){
  //fill measurement bias, resolution, and pull plots
  m_resPlots.fill(particle, truthParticle);
  //Not sure that the following hitsMatchedTracksPlots does anything...
  float barcode = truthParticle.barcode();
  if (barcode < 100000 && barcode != 0) { //Not sure why the barcode limit is 100k instead of 200k...
    m_hitsMatchedTracksPlots.fill(particle);
  }

  // filling non-fake tracks
  if (isLargeD0Track(particle)) {
    m_hitsPlots_nonfake_ld0.fill(particle);
    m_basicPlot_nonfake_ld0.fill(particle);
    m_ptPlot_nonfake_ld0.fill(particle);
    m_TrackRecoInfoPlots_nonfake_ld0.fill(particle);
  }
  else {
    m_hitsPlots_nonfake_st.fill(particle);
    m_basicPlot_nonfake_st.fill(particle);
    m_ptPlot_nonfake_st.fill(particle);
    m_TrackRecoInfoPlots_nonfake_st.fill(particle);
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
// In the loop over all track particles
// fillig all track particles, no selection 
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

//===================================================================================
// In the loop over all truth particles
// filling all truth particle, no selection 
//===================================================================================

void
InDetRttLargeD0Plots::fillTruth(const xAOD::TruthParticle& truth){
  m_basicPlot_truth.fill(truth);
  m_effPlots.fillDenominator(truth);
  //m_hitsDetailedPlots.fillDenom(truth);
}

void
InDetRttLargeD0Plots::fillEfficiency(const xAOD::TruthParticle& truth, const bool isReconstructed, const bool isLargeD0Track){
  m_effPlots.fill(truth,isReconstructed,isLargeD0Track);
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
  m_effPlots.fillNumerator(truthParticle);
}

/*
void
InDetRttLargeD0Plots::fillDupTrack(const xAOD::TrackParticle& track){
  //fill any duplicate plots requiring tracks only
  m_DuplicateTrack.fill(track);
}
*/

void InDetRttLargeD0Plots::fillBMR_Denom(const xAOD::TrackParticle& trackParticle){
  //fill the plot requiring prob < 0.8
  m_BadMatchRate.fillDenominator(trackParticle);
}

void InDetRttLargeD0Plots::fillBMR_Num(const xAOD::TrackParticle& trackParticle){
  //fill the plot requiring prob < 0.8
  m_BadMatchRate.fillNumBMR(trackParticle);
}

void InDetRttLargeD0Plots::fillRF_Num(const xAOD::TrackParticle& trackParticle){
  //fill the plot with prob < 0.2
  m_BadMatchRate.fillNumRF(trackParticle);
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
InDetRttLargeD0Plots::fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const InDetPerfPlot_fakes::Category c){
  if (isLargeD0Track(particle)) {
  m_fakePlotsLRT.fill(particle,match, c);
  } else {
  m_fakePlots.fill(particle,match, c);
  }
}

void InDetRttLargeD0Plots::SetPrimaryEtaCut( float eta/*, float pt*/ ) {
  m_truthPrimaryEtaCut = eta;
  m_resPlots.SetEtaBinning( int(2*eta/0.25), -1*eta, eta ); //Isn't this hard-coded into res.cxx anyway?

  //  m_truthPrimaryPtCut = pt;
  //m_resPlots.SetPtBinning( 20, pt, 25*pt );
}
