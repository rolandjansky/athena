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
#include <cmath> // std::isnan()
#include <limits>

using std::cout;
using std::endl;
using std::string;

namespace {
  bool
  isLargeD0Track(const xAOD::TrackParticle &tp) {
    const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = tp.patternRecoInfo();

    if (patternReco.test(49)) {
      return true;
    }
    return false;
  }
}// namespace



InDetRttLargeD0Plots::InDetRttLargeD0Plots(InDetPlotBase *pParent, const std::string &sDir) :
  InDetPlotBase(pParent, sDir),

  // =============================================================
  // plots for LargeD0 performance study
  // =============================================================
  
  // //Testing TRT extension efficiency
  m_trtTesting_nonfake(this, "LargeD0/TRTTesting/NonFakeTracks/"),
  m_trtTesting_matching_truthLink_primary(this, "LargeD0/TRTTesting/Matching/TruthLink/Primary"),
  m_trtTesting_matching_truthLink_secondary(this, "LargeD0/TRTTesting/Matching/TruthLink/Secondary/"),
  m_trtTesting_matching_noTruthLink(this, "LargeD0/TRTTesting/Matching/noTruthLink/"),
  m_trtTesting_fakes(this, "LargeD0/TRTTesting/FakeTracks/"),
  
  // hits plots
  m_hitsPlots_nonfake_st(this, "LargeD0/HitsPlots/StandardTracks/NonFakeTracks"),
  m_hitsPlots_matching_truthLink_primary_st(this, "LargeD0/HitsPlots/StandardTracks/Matching/TruthLink/Primary"),
  m_hitsPlots_matching_truthLink_secondary_st(this, "LargeD0/HitsPlots/StandardTracks/Matching/TruthLink/Secondary"),
  m_hitsPlots_matching_NotruthLink_st(this, "LargeD0/HitsPlots/StandardTracks/Matching/NotruthLink"),
  m_hitsPlots_fake_st(this, "LargeD0/HitsPlots/StandardTracks/FakeTracks"),
  
  m_hitsPlots_nonfake_ld0(this, "LargeD0/HitsPlots/LargeD0Tracks/NonFakeTracks"),
  m_hitsPlots_matching_truthLink_primary_ld0(this, "LargeD0/HitsPlots/LargeD0Tracks/Matching/TruthLink/Primary"),
  m_hitsPlots_matching_truthLink_secondary_ld0(this, "LargeD0/HitsPlots/LargeD0Tracks/Matching/TruthLink/Secondary"),
  m_hitsPlots_matching_NotruthLink_ld0(this, "LargeD0/HitsPlots/LargeD0Tracks/Matching/NotruthLink"),
  m_hitsPlots_fake_ld0(this, "LargeD0/HitsPlots/LargeD0Tracks/FakeTracks"),
  
  // basic plots
  m_basicPlot_MatchedPrimary_st(this, "LargeD0/basicPlot/StandardTracks/MatchedPrimary"),
  m_basicPlot_MatchedSecondary_st(this, "LargeD0/basicPlot/StandardTracks/MatchedSecondary"),
  m_basicPlot_NoTruthLink_st(this, "LargeD0/basicPlot/StandardTracks/NoTruthLink"),
  m_basicPlot_Fake_st(this, "LargeD0/basicPlot/StandardTracks/Fake"),
  
  m_basicPlot_MatchedPrimary_ld0(this, "LargeD0/basicPlot/LargeD0Tracks/MatchedPrimary"),
  m_basicPlot_MatchedSecondary_ld0(this, "LargeD0/basicPlot/LargeD0Tracks/MatchedSecondary"),
  m_basicPlot_NoTruthLink_ld0(this, "LargeD0/basicPlot/LargeD0Tracks/NoTruthLink"),
  m_basicPlot_Fake_ld0(this, "LargeD0/basicPlot/LargeD0Tracks/Fake"),

  // pt plots
  m_ptPlot_nonfake_st(this, "LargeD0/ptPlot/StandardTracks/NonFakeTracks"),
  m_ptPlot_nonfake_ld0(this, "LargeD0/ptPlot/LargeD0Tracks/NonFakeTracks"),
  m_ptPlot_fake_st(this, "LargeD0/ptPlot/StandardTracks/FakeTracks"),
  m_ptPlot_fake_ld0(this, "LargeD0/ptPlot/LargeD0Tracks/FakeTracks"),
  
  m_ptPlot_primary(this, "LargeD0/ptPlot/TruthTracks/Primary"),
  m_ptPlot_secondary(this, "LargeD0/ptPlot/TruthTracks/Secondary"),
  m_ptPlot_pileup_primary(this, "LargeD0/ptPlot/Pileup/TruthTracks/Primary"),
  m_ptPlot_pileup_secondary(this, "LargeD0/ptPlot/Pileup/TruthTracks/Secondary"),
  
  // Reco info plots
  m_TrackRecoInfoPlots_nonfake_st(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/NonFakeTracks"),
  m_TrackRecoInfoPlots_nonfake_ld0(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/NonFakeTracks"),
  m_TrackRecoInfoPlots_fake_st(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks"),
  m_TrackRecoInfoPlots_fake_ld0(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks"),
  
  // Fake rate plots
  m_fakePlots_primary(this, "LargeD0/FakeRatePlots/StandardTracks/Primary"),
  m_fakePlots_secondary(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary"),
  m_fakePlots_linked(this, "LargeD0/FakeRatePlots/StandardTracks/Linked"),
  m_fakePlots_notruth(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth"),
  m_fakePlots_combined(this, "LargeD0/FakeRatePlots/StandardTracks/Combined"),
  
  m_fakePlots_primary_LRT(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary"),
  m_fakePlots_secondary_LRT(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary"),
  m_fakePlots_linked_LRT(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked"),
  m_fakePlots_notruth_LRT(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth"),
  m_fakePlots_combined_LRT(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined"),
  
  // Efficiency plots
  m_effPlotsStd(this, "LargeD0/EffPlots/StandardTracks/All"), 
  m_effPlotsStdSignal(this, "LargeD0/EffPlots/StandardTracks/Signal"),
  m_effPlotsStdTRT(this, "LargeD0/EffPlots/StandardTracks/TRT/"),
  m_effPlotsStdTRTSignal(this, "LargeD0/EffPlots/StandardTracks/TRT/Signal"),
  m_effPlotsStdnoTRT(this, "LargeD0/EffPlots/StandardTracks/noTRT/"),
  m_effPlotsStdnoTRTSignal(this, "LargeD0/EffPlots/StandardTracks/noTRT/Signal"),
  m_effPlotsLRT(this, "LargeD0/EffPlots/LargeD0Tracks/All"),
  m_effPlotsLRTSignal(this, "LargeD0/EffPlots/LargeD0Tracks/Signal"),
  m_effPlotsLRTTRT(this, "LargeD0/EffPlots/LargeD0Tracks/TRT"),
  m_effPlotsLRTTRTSignal(this, "LargeD0/EffPlots/LargeD0Tracks/TRT/Signal"),
  m_effPlotsLRTnoTRT(this, "LargeD0/EffPlots/LargeD0Tracks/noTRT"),
  m_effPlotsLRTnoTRTSignal(this, "LargeD0/EffPlots/LargeD0Tracks/noTRT/Signal"),
  m_effPlotsLRTnonStd(this, "LargeD0/EffPlots/LargeD0TracksNonStd/All"),
  m_effPlotsLRTnonStdSignal(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal"),
  m_effPlotsLRTnonStdTRT(this, "LargeD0/EffPlots/LargeD0TracksNonStd/TRT"),
  m_effPlotsLRTnonStdnoTRT(this, "LargeD0/EffPlots/LargeD0TracksNonStd/noTRT"),
  m_effPlotsAll(this, "LargeD0/EffPlots/AllTracks/All"),
  m_effPlotsAllSignal(this, "LargeD0/EffPlots/AllTracks/Signal"),
  m_effPlotsAllTRT(this, "LargeD0/EffPlots/AllTracks/TRT"),
  m_effPlotsAllTRTSignal(this, "LargeD0/EffPlots/AllTracks/TRT/Signal"),
  m_effPlotsAllnoTRT(this, "LargeD0/EffPlots/AllTracks/noTRT"),
  m_effPlotsAllnoTRTSignal(this, "LargeD0/EffPlots/AllTracks/noTRT/Signal"),
  m_effPlots(this, "LargeD0/EffPlots"),
  
  // =============================================================
  // plots not used for LargeD0
  // =============================================================
  m_ptPlot(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  m_basicPlot(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  
  m_PtEtaPlots(this, "LargeD0_NOTUSED/SelectedGoodTracks", "TrackParticle"),
  m_IPPlots(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  
  m_TrackRecoInfoPlots(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  m_TrackTruthInfoPlots(this, "LargeD0_NOTUSED/Truth"),
  m_nTracks(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  m_resPlots(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  m_hitResidualPlot(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  m_hitsDetailedPlots(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  // Efficiency plots
  m_BadMatchRate(this, "LargeD0_NOTUSED/SelectedFakeTracks"),
  m_verticesPlots(this, "LargeD0_NOTUSED/Vertices/AllPrimaryVertices"),
  m_vertexPlots(this, "LargeD0_NOTUSED/Vertices/AllPrimaryVertices"),
  m_hardScatterVertexPlots(this, "LargeD0_NOTUSED/Vertices/HardScatteringVertex"),
  // marked 'not useful'...
  m_hitsPlots(this, "LargeD0_NOTUSED/SelectedGoodTracks"),
  m_hitsMatchedTracksPlots(this, "LargeD0_NOTUSED/SelectedMatchedTracks"),
  m_hitsFakeTracksPlots(this, "LargeD0_NOTUSED/SelectedFakeTracks") {
  // These settings are probably all redundant & can be removed from this script
  m_trackParticleTruthProbKey = "truthMatchProbability";
  m_truthProbLowThreshold = 0.5;
}

// ===================================================================================
// In the loop over all track particles
// filling tracks with truthMatching > minProbEffLow (truth-matched reco tracks)
// ===================================================================================

void
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle &particle) {
  m_hitResidualPlot.fill(particle);
  // fill pt plots
  m_ptPlot.fill(particle);
  m_basicPlot.fill(particle);
  m_PtEtaPlots.fill(particle);
  m_IPPlots.fill(particle);
  m_TrackRecoInfoPlots.fill(particle);
  m_hitsPlots.fill(particle);
  m_hitsDetailedPlots.fill(particle);
}

void
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle &particle, const int& barcode, const bool& fake) {
  m_trtTesting_nonfake.fill(particle);
  if ((barcode > 0)and(barcode < 200000)) {
    m_trtTesting_matching_truthLink_primary.fill(particle);
  }else if (barcode >= 200000) {
    m_trtTesting_matching_truthLink_secondary.fill(particle);
  }else if (barcode == -1 or barcode == 0) {
    m_trtTesting_matching_noTruthLink.fill(particle);
  }


  if (isLargeD0Track(particle)) {
    //m_basicPlot_nonfake_ld0.fill(particle);
    m_ptPlot_nonfake_ld0.fill(particle);
    m_TrackRecoInfoPlots_nonfake_ld0.fill(particle);
    m_hitsPlots_nonfake_ld0.fill(particle);

    // primary tracks
    if ((barcode > 0)and(barcode < 200000)) {
      m_hitsPlots_matching_truthLink_primary_ld0.fill(particle);
    }
    // secondary tracks
    else if (barcode >= 200000) {
      m_hitsPlots_matching_truthLink_secondary_ld0.fill(particle);
    }
    // no truth link
    else if (barcode == -1 or barcode == 0) {
      m_hitsPlots_matching_NotruthLink_ld0.fill(particle);
    }
  }else {
    //m_basicPlot_nonfake_st.fill(particle);
    m_ptPlot_nonfake_st.fill(particle);
    m_TrackRecoInfoPlots_nonfake_st.fill(particle);
    m_hitsPlots_nonfake_st.fill(particle);

    // primary tracks
    if ((barcode > 0)and(barcode < 200000)) {
      m_hitsPlots_matching_truthLink_primary_st.fill(particle);
    }
    // secondary tracks
    else if (barcode >= 200000) {
      m_hitsPlots_matching_truthLink_secondary_st.fill(particle);
    }
    // no truth link
    else if (barcode == -1 or barcode == 0) {
      m_hitsPlots_matching_NotruthLink_st.fill(particle);
    }
  }

  // -----------------------------------------------------

  if (isLargeD0Track(particle)) {
    if (barcode == 0 or barcode == -1) {
      m_basicPlot_NoTruthLink_ld0.fill(particle);
    } else if (fake) {
      m_basicPlot_Fake_ld0.fill(particle);
    } else {
      if (barcode >= 200000) {
	m_basicPlot_MatchedSecondary_ld0.fill(particle);
      } else if (barcode > 0) {
	m_basicPlot_MatchedPrimary_ld0.fill(particle);
      } else {
	std::cout << "<InDetRttLargeD0Plots::fill> Barcode " << barcode << " was not recognised." << std::endl;
      }
    }
  } else {
    if (barcode == 0 or barcode == -1) {
      m_basicPlot_NoTruthLink_st.fill(particle);
    } else if (fake) {
      m_basicPlot_Fake_st.fill(particle);
    } else {
      if (barcode >= 200000) {
	m_basicPlot_MatchedSecondary_st.fill(particle);
      } else if (barcode > 0) {
	m_basicPlot_MatchedPrimary_st.fill(particle);
      } else {
	std::cout << "<InDetRttLargeD0Plots::fill> Barcode " << barcode << " was not recognised." << std::endl;
      }
    }
  }
  
}

// ===================================================================================
// In the loop over all track particles
// filling track particle with truthMatching < minProbEffHigh (fake tracks)
// ===================================================================================

void
InDetRttLargeD0Plots::fillFake(const xAOD::TrackParticle &trackParticle) {
  m_trtTesting_fakes.fill(trackParticle);

  if (isLargeD0Track(trackParticle)) {
    m_hitsPlots_fake_ld0.fill(trackParticle);
    //m_basicPlot_fake_ld0.fill(trackParticle);
    m_ptPlot_fake_ld0.fill(trackParticle);
    m_TrackRecoInfoPlots_fake_ld0.fill(trackParticle);
  }else {
    m_hitsPlots_fake_st.fill(trackParticle);
    //m_basicPlot_fake_st.fill(trackParticle);
    m_ptPlot_fake_st.fill(trackParticle);
    m_TrackRecoInfoPlots_fake_st.fill(trackParticle);
  }
}

// ===================================================================================
// In the loop over all truth particles
// filling all truth particle, no selection
// ===================================================================================

void
InDetRttLargeD0Plots::fillTruth(const xAOD::TruthParticle &truth) {
  //m_basicPlot_truth.fill(truth);

  if ((truth.barcode() > 0)and(truth.barcode() < 200000)) {
    //m_basicPlot_primary_truth.fill(truth);
    m_ptPlot_primary.fill(truth);
  }else {
    //m_basicPlot_secondary_truth.fill(truth);
    m_ptPlot_secondary.fill(truth);
  }
}

void
InDetRttLargeD0Plots::pro_fill(const xAOD::TruthParticle &truth, float weight) {
  m_effPlots.pro_fill(truth, weight);
}

void
InDetRttLargeD0Plots::fillEfficiency(const xAOD::TruthParticle &truth, \
                                     const bool isReconstructed,   \
                                     const bool isLargeD0Track,    \
                                     const bool isTRTExtended,     \
                                     const bool isSignal) {
  unsigned int rec = (unsigned int) isReconstructed;
  unsigned int LRT = (unsigned int) isLargeD0Track;
  unsigned int TRT = (unsigned int) isTRTExtended;
  // unsigned int sig = (unsigned int) isSignal;
  bool isStandardTrack = false;

  if (isReconstructed && !isLargeD0Track) {
    isStandardTrack = true;
  }

  m_effPlotsAll.pro_fill(truth, rec);
  m_effPlotsAllTRT.pro_fill(truth, rec and TRT);
  m_effPlotsAllnoTRT.pro_fill(truth, rec and not TRT);
  m_effPlotsStd.pro_fill(truth, rec and not LRT);
  m_effPlotsLRT.pro_fill(truth, rec and LRT);
  m_effPlotsStdTRT.pro_fill(truth, rec and TRT and not LRT);
  m_effPlotsStdnoTRT.pro_fill(truth, rec and not TRT and not LRT);
  m_effPlotsLRTTRT.pro_fill(truth, rec and LRT and TRT);
  m_effPlotsLRTnoTRT.pro_fill(truth, rec and LRT and not TRT);

  if (!(isStandardTrack)) {
    m_effPlotsLRTnonStd.pro_fill(truth, rec and LRT);
    m_effPlotsLRTnonStdTRT.pro_fill(truth, rec and LRT and TRT);
    m_effPlotsLRTnonStdnoTRT.pro_fill(truth, rec and LRT and not TRT);
  }

  if (isSignal) {
    m_effPlotsAllSignal.pro_fill(truth, rec);
    m_effPlotsAllTRTSignal.pro_fill(truth, rec and TRT);
    m_effPlotsAllnoTRTSignal.pro_fill(truth, rec and not TRT);
    m_effPlotsStdSignal.pro_fill(truth, rec and not LRT);
    m_effPlotsStdTRTSignal.pro_fill(truth, rec and TRT and not LRT);
    m_effPlotsStdnoTRTSignal.pro_fill(truth, rec and not TRT and not LRT);
    m_effPlotsLRTSignal.pro_fill(truth, rec and LRT);
    m_effPlotsLRTTRTSignal.pro_fill(truth, rec and LRT and TRT);
    m_effPlotsLRTnoTRTSignal.pro_fill(truth, rec and LRT and not TRT);
  }


  return;
}

// ===================================================================================
// In the loop over truth particles
// filling truth particle with truthMatching > minProbEffHigh (Truth-matched truth tracks)
// ===================================================================================

void
InDetRttLargeD0Plots::fill(const xAOD::TruthParticle &truthParticle) {
  // fill truth plots
  m_TrackTruthInfoPlots.fill(truthParticle);
  // this gets called for the truth particles associated to tracks - exactly what we need!
  // m_effPlots.fillNumerator(truthParticle); // @asogaard
}

void
InDetRttLargeD0Plots::fillBMR(const xAOD::TrackParticle &track, float weight) {
  // fill the plot requiring truth matching probability less than 80%
  m_BadMatchRate.fillBMR(track, weight);
}

void
InDetRttLargeD0Plots::fillRF(const xAOD::TrackParticle &track, float weight) {
  // fill the plots requiring truth matching probability less than 20%
  m_BadMatchRate.fillRF(track, weight);
}

void
InDetRttLargeD0Plots::fill(const xAOD::VertexContainer &vertexContainer) {
  // fill vertex container general properties
  // m_verticesPlots.fill(vertexContainer); //if ever needed
  // fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto &vtx : vertexContainer.stdcont()) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) {
      continue; // skip dummy vertex
    }
    m_vertexPlots.fill(*vtx);
    if (vtx->vertexType() == xAOD::VxType::PriVtx) {
      m_hardScatterVertexPlots.fill(*vtx);
    }
  }
}

void
InDetRttLargeD0Plots::fill(const xAOD::VertexContainer &vertexContainer, const xAOD::EventInfo &ei) {
  m_verticesPlots.fill(vertexContainer, ei);
}

void
InDetRttLargeD0Plots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter) {
  m_nTracks.fill(freq, counter);
}

void
InDetRttLargeD0Plots::fillFakeRate(const xAOD::TrackParticle &particle, const bool fake,
                                   const InDetPerfPlot_fakes::Category /* c*/) {
  // unsigned int link = (unsigned int) hasTruthLink;

  // Get associated truth particle, is a valid link exists.
  typedef ElementLink<xAOD::TruthParticleContainer> ElementTruthLink_t;
  const xAOD::TruthParticle *truth = nullptr;
  if (particle.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
    const ElementTruthLink_t truthContainer = particle.auxdata<ElementTruthLink_t>("truthParticleLink");
    if (truthContainer.isValid()) {
      truth = *truthContainer;
    }
  }

  // Declare and get necessary variables.
  const bool isLargeD0 = isLargeD0Track(particle);
  const int  barcode   = (truth ? truth->barcode() : -1);
  const bool primary   = (barcode < 200000 and barcode > 0); // match and ...
  const bool secondary = (barcode >= 200000); // match and ...
  const bool notruthlink = (barcode == 0 or barcode == -1);

  // Fill fakerate distributions.
  /* "New" definition, cf. [https://indico.cern.ch/event/559023/contributions/2255673/attachments/1314623/1968909/2016.07.25.pdf] */
  if (isLargeD0) {
    m_fakePlots_primary_LRT  .fill(particle, fake and primary);
    m_fakePlots_secondary_LRT.fill(particle, fake and secondary); 
    m_fakePlots_linked_LRT   .fill(particle, fake and (primary or secondary)); 
    m_fakePlots_notruth_LRT  .fill(particle, fake and notruthlink);
    m_fakePlots_combined_LRT .fill(particle, fake or  notruthlink);
  } else {
    m_fakePlots_primary      .fill(particle, fake and primary);
    m_fakePlots_secondary    .fill(particle, fake and secondary); 
    m_fakePlots_linked       .fill(particle, fake and (primary or secondary)); 
    m_fakePlots_notruth      .fill(particle, fake and notruthlink);
    m_fakePlots_combined     .fill(particle, fake or  notruthlink);
  }

}
