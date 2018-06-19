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
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include <cmath> // std::isnan()
#include <limits>

using std::cout;
using std::endl;
using std::string;

namespace {
  bool
  isLargeD0Track(const xAOD::TrackParticle& tp) {
    const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = tp.patternRecoInfo();

    if (patternReco.test(49)) {
      return true;
    }
    return false;
  }
}// namespace



InDetRttLargeD0Plots::InDetRttLargeD0Plots(InDetPlotBase* pParent, const std::string& sDir) :
  InDetPlotBase(pParent, sDir),

// =============================================================
// plots for LargeD0 performance study
// =============================================================

// //Testing TRT extension efficiency
  m_trtTesting_st_matched(this, "LargeD0/TRTTesting/StandardTracks/MatchedTracks/"),
  m_trtTesting_st_matched_signal(this, "LargeD0/TRTTesting/StandardTracks/MatchedTracks/Signal"),
  m_trtTesting_st_matched_primary(this, "LargeD0/TRTTesting/StandardTracks/MatchedTracks/Primary"),
  m_trtTesting_st_matched_secondary(this, "LargeD0/TRTTesting/StandardTracks/MatchedTracks/Secondary/"),
  m_trtTesting_st_matched_noTruthLink(this, "LargeD0/TRTTesting/StandardTracks/MatchedTracks/NoTruthLink/"),
  m_trtTesting_st_fake(this, "LargeD0/TRTTesting/StandardTracks/FakeTracks/"),
  m_trtTesting_st_fake_primary(this, "LargeD0/TRTTesting/StandardTracks/FakeTracks/Primary"),
  m_trtTesting_st_fake_secondary(this, "LargeD0/TRTTesting/StandardTracks/FakeTracks/Secondary"),
  m_trtTesting_st_fake_noTruthLink(this, "LargeD0/TRTTesting/StandardTracks/FakeTracks/NoTruthLink"),

  m_trtTesting_ld0_matched(this, "LargeD0/TRTTesting/LargeD0Tracks/MatchedTracks/"),
  m_trtTesting_ld0_matched_signal(this, "LargeD0/TRTTesting/LargeD0Tracks/MatchedTracks/Signal"),
  m_trtTesting_ld0_matched_primary(this, "LargeD0/TRTTesting/LargeD0Tracks/MatchedTracks/Primary"),
  m_trtTesting_ld0_matched_secondary(this, "LargeD0/TRTTesting/LargeD0Tracks/MatchedTracks/Secondary/"),
  m_trtTesting_ld0_matched_noTruthLink(this, "LargeD0/TRTTesting/LargeD0Tracks/MatchedTracks/NoTruthLink/"),
  m_trtTesting_ld0_fake(this, "LargeD0/TRTTesting/LargeD0Tracks/FakeTracks/"),
  m_trtTesting_ld0_fake_primary(this, "LargeD0/TRTTesting/LargeD0Tracks/FakeTracks/Primary"),
  m_trtTesting_ld0_fake_secondary(this, "LargeD0/TRTTesting/LargeD0Tracks/FakeTracks/Secondary"),
  m_trtTesting_ld0_fake_noTruthLink(this, "LargeD0/TRTTesting/LargeD0Tracks/FakeTracks/NoTruthLink"),

// hits plots
  m_hitsPlots_st_matched(this, "LargeD0/HitsPlots/StandardTracks/MatchedTracks"),
  m_hitsPlots_st_matched_signal(this, "LargeD0/HitsPlots/StandardTracks/MatchedTracks/Signal"),
  m_hitsPlots_st_matched_primary(this, "LargeD0/HitsPlots/StandardTracks/MatchedTracks/Primary"),
  m_hitsPlots_st_matched_secondary(this, "LargeD0/HitsPlots/StandardTracks/MatchedTracks/Secondary"),
  m_hitsPlots_st_matched_noTruthLink(this, "LargeD0/HitsPlots/StandardTracks/MatchedTracks/NoTruthLink"),
  m_hitsPlots_st_fake(this, "LargeD0/HitsPlots/StandardTracks/FakeTracks"),
  m_hitsPlots_st_fake_primary(this, "LargeD0/HitsPlots/StandardTracks/FakeTracks/Primary"),
  m_hitsPlots_st_fake_secondary(this, "LargeD0/HitsPlots/StandardTracks/FakeTracks/Secondary"),
  m_hitsPlots_st_fake_noTruthLink(this, "LargeD0/HitsPlots/StandardTracks/FakeTracks/NoTruthLink"),

  m_hitsPlots_ld0_matched(this, "LargeD0/HitsPlots/LargeD0Tracks/MatchedTracks"),
  m_hitsPlots_ld0_matched_signal(this, "LargeD0/HitsPlots/LargeD0Tracks/MatchedTracks/Signal"),
  m_hitsPlots_ld0_matched_primary(this, "LargeD0/HitsPlots/LargeD0Tracks/MatchedTracks/Primary"),
  m_hitsPlots_ld0_matched_secondary(this, "LargeD0/HitsPlots/LargeD0Tracks/MatchedTracks/Secondary"),
  m_hitsPlots_ld0_matched_noTruthLink(this, "LargeD0/HitsPlots/LargeD0Tracks/MatchedTracks/NoTruthLink"),
  m_hitsPlots_ld0_fake(this, "LargeD0/HitsPlots/LargeD0Tracks/FakeTracks"),
  m_hitsPlots_ld0_fake_primary(this, "LargeD0/HitsPlots/LargeD0Tracks/FakeTracks/Primary"),
  m_hitsPlots_ld0_fake_secondary(this, "LargeD0/HitsPlots/LargeD0Tracks/FakeTracks/Secondary"),
  m_hitsPlots_ld0_fake_noTruthLink(this, "LargeD0/HitsPlots/LargeD0Tracks/FakeTracks/NoTruthLink"),

// basic plots
  m_basicPlot_st_matched(this, "LargeD0/basicPlot/StandardTracks/MatchedTracks"),
  m_basicPlot_st_matched_primary(this, "LargeD0/basicPlot/StandardTracks/MatchedTracks/Primary"),
  m_basicPlot_st_matched_secondary(this, "LargeD0/basicPlot/StandardTracks/MatchedTracks/Secondary"),
  m_basicPlot_st_matched_noTruthLink(this, "LargeD0/basicPlot/StandardTracks/MatchedTracks/NoTruthLink"),
  m_basicPlot_st_fake(this, "LargeD0/basicPlot/StandardTracks/FakeTracks"),
  m_basicPlot_st_fake_primary(this, "LargeD0/basicPlot/StandardTracks/FakeTracks/Primary"),
  m_basicPlot_st_fake_secondary(this, "LargeD0/basicPlot/StandardTracks/FakeTracks/Secondary"),
  m_basicPlot_st_fake_noTruthLink(this, "LargeD0/basicPlot/StandardTracks/FakeTracks/NoTruthLink"),

  m_basicPlot_ld0_matched(this, "LargeD0/basicPlot/LargeD0Tracks/MatchedTracks"),
  m_basicPlot_ld0_matched_primary(this, "LargeD0/basicPlot/LargeD0Tracks/MatchedTracks/Primary"),
  m_basicPlot_ld0_matched_secondary(this, "LargeD0/basicPlot/LargeD0Tracks/MatchedTracks/Secondary"),
  m_basicPlot_ld0_matched_noTruthLink(this, "LargeD0/basicPlot/LargeD0Tracks/MatchedTracks/NoTruthLink"),
  m_basicPlot_ld0_fake(this, "LargeD0/basicPlot/LargeD0Tracks/FakeTracks"),
  m_basicPlot_ld0_fake_primary(this, "LargeD0/basicPlot/LargeD0Tracks/FakeTracks/Primary"),
  m_basicPlot_ld0_fake_secondary(this, "LargeD0/basicPlot/LargeD0Tracks/FakeTracks/Secondary"),
  m_basicPlot_ld0_fake_noTruthLink(this, "LargeD0/basicPlot/LargeD0Tracks/FakeTracks/NoTruthLink"),

// pt plots
  m_ptPlot_st_matched(this, "LargeD0/ptPlot/StandardTracks/MatchedTracks"),
  m_ptPlot_st_matched_primary(this, "LargeD0/ptPlot/StandardTracks/MatchedTracks/Primary"),
  m_ptPlot_st_matched_secondary(this, "LargeD0/ptPlot/StandardTracks/MatchedTracks/Secondary"),
  m_ptPlot_st_matched_noTruthLink(this, "LargeD0/ptPlot/StandardTracks/MatchedTracks/NoTruthLink"),
  m_ptPlot_st_fake(this, "LargeD0/ptPlot/StandardTracks/FakeTracks"),
  m_ptPlot_st_fake_primary(this, "LargeD0/ptPlot/StandardTracks/FakeTracks/Primary"),
  m_ptPlot_st_fake_secondary(this, "LargeD0/ptPlot/StandardTracks/FakeTracks/Secondary"),
  m_ptPlot_st_fake_noTruthLink(this, "LargeD0/ptPlot/StandardTracks/FakeTracks/NoTruthLink"),

  m_ptPlot_ld0_matched(this, "LargeD0/ptPlot/LargeD0Tracks/MatchedTracks"),
  m_ptPlot_ld0_matched_primary(this, "LargeD0/ptPlot/LargeD0Tracks/MatchedTracks/Primary"),
  m_ptPlot_ld0_matched_secondary(this, "LargeD0/ptPlot/LargeD0Tracks/MatchedTracks/Secondary"),
  m_ptPlot_ld0_matched_noTruthLink(this, "LargeD0/ptPlot/LargeD0Tracks/MatchedTracks/NoTruthLink"),
  m_ptPlot_ld0_fake(this, "LargeD0/ptPlot/LargeD0Tracks/FakeTracks"),
  m_ptPlot_ld0_fake_primary(this, "LargeD0/ptPlot/LargeD0Tracks/FakeTracks/Primary"),
  m_ptPlot_ld0_fake_secondary(this, "LargeD0/ptPlot/LargeD0Tracks/FakeTracks/Secondary"),
  m_ptPlot_ld0_fake_noTruthLink(this, "LargeD0/ptPlot/LargeD0Tracks/FakeTracks/NoTruthLink"),

  m_ptPlot_truth_primary(this, "LargeD0/ptPlot/TruthTracks/Primary"),
  m_ptPlot_truth_secondary(this, "LargeD0/ptPlot/TruthTracks/Secondary"),
  m_ptPlot_pileup_primary(this, "LargeD0/ptPlot/Pileup/TruthTracks/Primary"),
  m_ptPlot_pileup_secondary(this, "LargeD0/ptPlot/Pileup/TruthTracks/Secondary"),

// Reco info plots
  m_TrackRecoInfoPlots_st_matched(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/MatchedTracks"),
  m_TrackRecoInfoPlots_st_matched_primary(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/MatchedTracks/Primary"),
  m_TrackRecoInfoPlots_st_matched_secondary(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/MatchedTracks/Secondary"),
  m_TrackRecoInfoPlots_st_matched_noTruthLink(this,
                                              "LargeD0/TrackRecoInfoPlots/StandardTracks/MatchedTracks/NoTruthLink"),
  m_TrackRecoInfoPlots_st_fake(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks"),
  m_TrackRecoInfoPlots_st_fake_primary(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks/Primary"),
  m_TrackRecoInfoPlots_st_fake_secondary(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks/Secondary"),
  m_TrackRecoInfoPlots_st_fake_noTruthLink(this, "LargeD0/TrackRecoInfoPlots/StandardTracks/FakeTracks/NoTruthLink"),

  m_TrackRecoInfoPlots_ld0_matched(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/MatchedTracks"),
  m_TrackRecoInfoPlots_ld0_matched_primary(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/MatchedTracks/Primary"),
  m_TrackRecoInfoPlots_ld0_matched_secondary(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/MatchedTracks/Secondary"),
  m_TrackRecoInfoPlots_ld0_matched_noTruthLink(this,
                                               "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/MatchedTracks/NoTruthLink"),
  m_TrackRecoInfoPlots_ld0_fake(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks"),
  m_TrackRecoInfoPlots_ld0_fake_primary(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks/Primary"),
  m_TrackRecoInfoPlots_ld0_fake_secondary(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks/Secondary"),
  m_TrackRecoInfoPlots_ld0_fake_noTruthLink(this, "LargeD0/TrackRecoInfoPlots/LargeD0Tracks/FakeTracks/NoTruthLink"),

// Fake rate plots
  m_fakePlots_st(this, "LargeD0/FakeRatePlots/StandardTracks"),
  m_fakePlots_st_trtHit(this, "LargeD0/FakeRatePlots/StandardTracks/TRT"),
  m_fakePlots_st_trtOut(this, "LargeD0/FakeRatePlots/StandardTracks/TRTOut"),
  m_fakePlots_st_noTrt(this, "LargeD0/FakeRatePlots/StandardTracks/NoTRT"),
  m_fakePlots_st_tubeFrac051(this, "LargeD0/FakeRatePlots/StandardTracks/TubeFrac05_1"),
  m_fakePlots_st_tubeFrac0405(this, "LargeD0/FakeRatePlots/StandardTracks/TubeFrac04_05"),
  m_fakePlots_st_tubeFrac004(this, "LargeD0/FakeRatePlots/StandardTracks/TubeFrac0_04"),
  m_fakePlots_st_primary(this, "LargeD0/FakeRatePlots/StandardTracks/Primary"),
  m_fakePlots_st_primary_trtHit(this, "LargeD0/FakeRatePlots/StandardTracks/Primary/TRT"),
  m_fakePlots_st_primary_trtOut(this, "LargeD0/FakeRatePlots/StandardTracks/Primary/TRTOut"),
  m_fakePlots_st_primary_noTrt(this, "LargeD0/FakeRatePlots/StandardTracks/Primary/NoTRT"),
  m_fakePlots_st_primary_tubeFrac051(this, "LargeD0/FakeRatePlots/StandardTracks/Primary/TubeFrac05_1"),
  m_fakePlots_st_primary_tubeFrac0405(this, "LargeD0/FakeRatePlots/StandardTracks/Primary/TubeFrac04_05"),
  m_fakePlots_st_primary_tubeFrac004(this, "LargeD0/FakeRatePlots/StandardTracks/Primary/TubeFrac0_04"),
  m_fakePlots_st_secondary(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary"),
  m_fakePlots_st_secondary_trtHit(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary/TRT"),
  m_fakePlots_st_secondary_trtOut(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary/TRTOut"),
  m_fakePlots_st_secondary_noTrt(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary/NoTRT"),
  m_fakePlots_st_secondary_tubeFrac051(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary/TubeFrac05_1"),
  m_fakePlots_st_secondary_tubeFrac0405(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary/TubeFrac04_05"),
  m_fakePlots_st_secondary_tubeFrac004(this, "LargeD0/FakeRatePlots/StandardTracks/Secondary/TubeFrac0_04"),
  m_fakePlots_st_linked(this, "LargeD0/FakeRatePlots/StandardTracks/Linked"),
  m_fakePlots_st_linked_trtHit(this, "LargeD0/FakeRatePlots/StandardTracks/Linked/TRT"),
  m_fakePlots_st_linked_trtOut(this, "LargeD0/FakeRatePlots/StandardTracks/Linked/TRTOut"),
  m_fakePlots_st_linked_noTrt(this, "LargeD0/FakeRatePlots/StandardTracks/Linked/NoTRT"),
  m_fakePlots_st_linked_tubeFrac051(this, "LargeD0/FakeRatePlots/StandardTracks/Linked/TubeFrac05_1"),
  m_fakePlots_st_linked_tubeFrac0405(this, "LargeD0/FakeRatePlots/StandardTracks/Linked/TubeFrac04_05"),
  m_fakePlots_st_linked_tubeFrac004(this, "LargeD0/FakeRatePlots/StandardTracks/Linked/TubeFrac0_04"),
  m_fakePlots_st_notruth(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth"),
  m_fakePlots_st_notruth_trtHit(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth/TRT"),
  m_fakePlots_st_notruth_trtOut(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth/TRTOut"),
  m_fakePlots_st_notruth_noTrt(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth/NoTRT"),
  m_fakePlots_st_notruth_tubeFrac051(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth/TubeFrac05_1"),
  m_fakePlots_st_notruth_tubeFrac0405(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth/TubeFrac04_05"),
  m_fakePlots_st_notruth_tubeFrac004(this, "LargeD0/FakeRatePlots/StandardTracks/NoTruth/TubeFrac0_04"),
  m_fakePlots_st_combined(this, "LargeD0/FakeRatePlots/StandardTracks/Combined"),
  m_fakePlots_st_combined_trtHit(this, "LargeD0/FakeRatePlots/StandardTracks/Combined/TRT"),
  m_fakePlots_st_combined_trtOut(this, "LargeD0/FakeRatePlots/StandardTracks/Combined/TRTOut"),
  m_fakePlots_st_combined_noTrt(this, "LargeD0/FakeRatePlots/StandardTracks/Combined/NoTRT"),
  m_fakePlots_st_combined_tubeFrac051(this, "LargeD0/FakeRatePlots/StandardTracks/Combined/TubeFrac05_1"),
  m_fakePlots_st_combined_tubeFrac0405(this, "LargeD0/FakeRatePlots/StandardTracks/Combined/TubeFrac04_05"),
  m_fakePlots_st_combined_tubeFrac004(this, "LargeD0/FakeRatePlots/StandardTracks/Combined/TubeFrac0_04"),

  m_fakePlots_ld0(this, "LargeD0/FakeRatePlots/LargeD0Tracks"),
  m_fakePlots_ld0_trtHit(this, "LargeD0/FakeRatePlots/LargeD0Tracks/TRT"),
  m_fakePlots_ld0_trtOut(this, "LargeD0/FakeRatePlots/LargeD0Tracks/TRTOut"),
  m_fakePlots_ld0_noTrt(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTRT"),
  m_fakePlots_ld0_tubeFrac051(this, "LargeD0/FakeRatePlots/LargeD0Tracks/TubeFrac05_1"),
  m_fakePlots_ld0_tubeFrac0405(this, "LargeD0/FakeRatePlots/LargeD0Tracks/TubeFrac04_05"),
  m_fakePlots_ld0_tubeFrac004(this, "LargeD0/FakeRatePlots/LargeD0Tracks/TubeFrac0_04"),
  m_fakePlots_ld0_primary(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary"),
  m_fakePlots_ld0_primary_trtHit(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary/TRT"),
  m_fakePlots_ld0_primary_trtOut(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary/TRTOut"),
  m_fakePlots_ld0_primary_noTrt(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary/NoTRT"),
  m_fakePlots_ld0_primary_tubeFrac051(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary/TubeFrac05_1"),
  m_fakePlots_ld0_primary_tubeFrac0405(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary/TubeFrac04_05"),
  m_fakePlots_ld0_primary_tubeFrac004(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Primary/TubeFrac0_04"),
  m_fakePlots_ld0_secondary(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary"),
  m_fakePlots_ld0_secondary_trtHit(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary/TRT"),
  m_fakePlots_ld0_secondary_trtOut(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary/TRTOut"),
  m_fakePlots_ld0_secondary_noTrt(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary/NoTRT"),
  m_fakePlots_ld0_secondary_tubeFrac051(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary/TubeFrac05_1"),
  m_fakePlots_ld0_secondary_tubeFrac0405(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary/TubeFrac04_05"),
  m_fakePlots_ld0_secondary_tubeFrac004(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Secondary/TubeFrac0_04"),
  m_fakePlots_ld0_linked(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked"),
  m_fakePlots_ld0_linked_trtHit(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked/TRT"),
  m_fakePlots_ld0_linked_trtOut(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked/TRTOut"),
  m_fakePlots_ld0_linked_noTrt(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked/NoTRT"),
  m_fakePlots_ld0_linked_tubeFrac051(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked/TubeFrac05_1"),
  m_fakePlots_ld0_linked_tubeFrac0405(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked/TubeFrac04_05"),
  m_fakePlots_ld0_linked_tubeFrac004(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Linked/TubeFrac0_04"),
  m_fakePlots_ld0_notruth(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth"),
  m_fakePlots_ld0_notruth_trtHit(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth/TRT"),
  m_fakePlots_ld0_notruth_trtOut(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth/TRTOut"),
  m_fakePlots_ld0_notruth_noTrt(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth/NoTRT"),
  m_fakePlots_ld0_notruth_tubeFrac051(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth/TubeFrac05_1"),
  m_fakePlots_ld0_notruth_tubeFrac0405(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth/TubeFrac04_05"),
  m_fakePlots_ld0_notruth_tubeFrac004(this, "LargeD0/FakeRatePlots/LargeD0Tracks/NoTruth/TubeFrac0_04"),
  m_fakePlots_ld0_combined(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined"),
  m_fakePlots_ld0_combined_trtHit(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined/TRT"),
  m_fakePlots_ld0_combined_trtOut(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined/TRTOut"),
  m_fakePlots_ld0_combined_noTrt(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined/NoTRT"),
  m_fakePlots_ld0_combined_tubeFrac051(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined/TubeFrac05_1"),
  m_fakePlots_ld0_combined_tubeFrac0405(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined/TubeFrac04_05"),
  m_fakePlots_ld0_combined_tubeFrac004(this, "LargeD0/FakeRatePlots/LargeD0Tracks/Combined/TubeFrac0_04"),

// Efficiency plots
  m_effPlots_all(this, "LargeD0/EffPlots/AllTracks/"),
  m_effPlots_all_trtHit(this, "LargeD0/EffPlots/AllTracks/TRT"),
  m_effPlots_all_trtOut(this, "LargeD0/EffPlots/AllTracks/TRTOut"),
  m_effPlots_all_noTrt(this, "LargeD0/EffPlots/AllTracks/noTRT"),
  m_effPlots_all_signal(this, "LargeD0/EffPlots/AllTracks/Signal"),
  m_effPlots_all_signal_trtHit(this, "LargeD0/EffPlots/AllTracks/Signal/TRT"),
  m_effPlots_all_signal_trtOut(this, "LargeD0/EffPlots/AllTracks/Signal/TRTOut"),
  m_effPlots_all_signal_noTrt(this, "LargeD0/EffPlots/AllTracks/Signal/noTRT"),
  m_effPlots_all_signal_tubeFrac051(this, "LargeD0/EffPlots/AllTracks/Signal/TubeFrac05_1"),
  m_effPlots_all_signal_tubeFrac0405(this, "LargeD0/EffPlots/AllTracks/Signal/TubeFrac04_05"),
  m_effPlots_all_signal_tubeFrac004(this, "LargeD0/EffPlots/AllTracks/Signal/TubeFrac0_04"),

  m_effPlots_st(this, "LargeD0/EffPlots/StandardTracks/"),
  m_effPlots_st_trtHit(this, "LargeD0/EffPlots/StandardTracks/TRT/"),
  m_effPlots_st_trtOut(this, "LargeD0/EffPlots/StandardTracks/TRTOut/"),
  m_effPlots_st_noTrt(this, "LargeD0/EffPlots/StandardTracks/noTRT/"),
  m_effPlots_st_signal(this, "LargeD0/EffPlots/StandardTracks/Signal"),
  m_effPlots_st_signal_trtHit(this, "LargeD0/EffPlots/StandardTracks/Signal/TRT"),
  m_effPlots_st_signal_trtOut(this, "LargeD0/EffPlots/StandardTracks/Signal/TRTOut"),
  m_effPlots_st_signal_noTrt(this, "LargeD0/EffPlots/StandardTracks/Signal/noTRT"),
  m_effPlots_st_signal_tubeFrac051(this, "LargeD0/EffPlots/StandardTracks/Signal/TubeFrac05_1"),
  m_effPlots_st_signal_tubeFrac0405(this, "LargeD0/EffPlots/StandardTracks/Signal/TubeFrac04_05"),
  m_effPlots_st_signal_tubeFrac004(this, "LargeD0/EffPlots/StandardTracks/Signal/TubeFrac0_04"),

  m_effPlots_ld0(this, "LargeD0/EffPlots/LargeD0Tracks"),
  m_effPlots_ld0_trtHit(this, "LargeD0/EffPlots/LargeD0Tracks/TRT"),
  m_effPlots_ld0_trtOut(this, "LargeD0/EffPlots/LargeD0Tracks/TRTOut"),
  m_effPlots_ld0_noTrt(this, "LargeD0/EffPlots/LargeD0Tracks/noTRT"),
  m_effPlots_ld0_signal(this, "LargeD0/EffPlots/LargeD0Tracks/Signal"),
  m_effPlots_ld0_signal_trtHit(this, "LargeD0/EffPlots/LargeD0Tracks/Signal/TRT"),
  m_effPlots_ld0_signal_trtOut(this, "LargeD0/EffPlots/LargeD0Tracks/Signal/TRTOut"),
  m_effPlots_ld0_signal_noTrt(this, "LargeD0/EffPlots/LargeD0Tracks/Signal/noTRT"),
  m_effPlots_ld0_signal_tubeFrac051(this, "LargeD0/EffPlots/LargeD0Tracks/Signal/TubeFrac05_1"),
  m_effPlots_ld0_signal_tubeFrac0405(this, "LargeD0/EffPlots/LargeD0Tracks/Signal/TubeFrac04_05"),
  m_effPlots_ld0_signal_tubeFrac004(this, "LargeD0/EffPlots/LargeD0Tracks/Signal/TubeFrac0_04"),

  m_effPlots_stNonLd0(this, "LargeD0/EffPlots/StandardTracksNonLRT"),
  m_effPlots_stNonLd0_trtHit(this, "LargeD0/EffPlots/StandardTracksNonLRT/TRT"),
  m_effPlots_stNonLd0_trtOut(this, "LargeD0/EffPlots/StandardTracksNonLRT/TRTOut"),
  m_effPlots_stNonLd0_noTrt(this, "LargeD0/EffPlots/StandardTracksNonLRT/noTRT"),
  m_effPlots_stNonLd0_signal(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal"),
  m_effPlots_stNonLd0_signal_trtHit(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal/TRT"),
  m_effPlots_stNonLd0_signal_trtOut(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal/TRTOut"),
  m_effPlots_stNonLd0_signal_noTrt(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal/noTRT"),
  m_effPlots_stNonLd0_signal_tubeFrac051(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal/TubeFrac05_1"),
  m_effPlots_stNonLd0_signal_tubeFrac0405(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal/TubeFrac04_05"),
  m_effPlots_stNonLd0_signal_tubeFrac004(this, "LargeD0/EffPlots/StandardTracksNonLRT/Signal/TubeFrac0_04"),

  m_effPlots_ld0NonSt(this, "LargeD0/EffPlots/LargeD0TracksNonStd"),
  m_effPlots_ld0NonSt_trtHit(this, "LargeD0/EffPlots/LargeD0TracksNonStd/TRT"),
  m_effPlots_ld0NonSt_trtOut(this, "LargeD0/EffPlots/LargeD0TracksNonStd/TRTOut"),
  m_effPlots_ld0NonSt_noTrt(this, "LargeD0/EffPlots/LargeD0TracksNonStd/noTRT"),
  m_effPlots_ld0NonSt_signal(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal"),
  m_effPlots_ld0NonSt_signal_trtHit(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal/TRT"),
  m_effPlots_ld0NonSt_signal_trtOut(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal/TRTOut"),
  m_effPlots_ld0NonSt_signal_noTrt(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal/noTRT"),
  m_effPlots_ld0NonSt_signal_tubeFrac051(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal/TubeFrac05_1"),
  m_effPlots_ld0NonSt_signal_tubeFrac0405(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal/TubeFrac04_05"),
  m_effPlots_ld0NonSt_signal_tubeFrac004(this, "LargeD0/EffPlots/LargeD0TracksNonStd/Signal/TubeFrac0_04"),

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
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle& particle) {
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
InDetRttLargeD0Plots::fill(const xAOD::TrackParticle& particle, const int barcode, const xAOD::EventInfo& ei, const xAOD::VertexContainer& pv, const bool isSignal) {
  const bool noTruthLink = ((barcode == 0)or(barcode == -1));

  if (isLargeD0Track(particle)) {
    // all ld0 tracks
    m_trtTesting_ld0_matched.fill(particle, ei, pv);
    m_hitsPlots_ld0_matched.fill(particle);
    m_basicPlot_ld0_matched.fill(particle);
    m_ptPlot_ld0_matched.fill(particle);
    m_TrackRecoInfoPlots_ld0_matched.fill(particle);
    if(isSignal) {
      m_trtTesting_ld0_matched_signal.fill(particle, ei, pv);
      m_hitsPlots_ld0_matched_signal.fill(particle);
    }
    if (noTruthLink) { // no truth link
      m_trtTesting_ld0_matched_noTruthLink.fill(particle, ei, pv);
      m_hitsPlots_ld0_matched_noTruthLink.fill(particle);
      m_basicPlot_ld0_matched_noTruthLink.fill(particle);
      m_ptPlot_ld0_matched_noTruthLink.fill(particle);
      m_TrackRecoInfoPlots_ld0_matched_noTruthLink.fill(particle);
    } else {
      if (barcode >= 200000) { // secondary tracks
        m_trtTesting_ld0_matched_secondary.fill(particle, ei, pv);
        m_hitsPlots_ld0_matched_secondary.fill(particle);
        m_basicPlot_ld0_matched_secondary.fill(particle);
        m_ptPlot_ld0_matched_secondary.fill(particle);
        m_TrackRecoInfoPlots_ld0_matched_secondary.fill(particle);
      } else if (barcode > 0) {  // primary tracks
        m_trtTesting_ld0_matched_primary.fill(particle, ei, pv);
        m_hitsPlots_ld0_matched_primary.fill(particle);
        m_basicPlot_ld0_matched_primary.fill(particle);
        m_ptPlot_ld0_matched_primary.fill(particle);
        m_TrackRecoInfoPlots_ld0_matched_primary.fill(particle);
      } else {
        std::cout << "<InDetRttLargeD0Plots::fill> Barcode " << barcode << " was not recognised." << std::endl;
      }
    }
  } else {
    // all std tracks
    m_trtTesting_st_matched.fill(particle, ei, pv);
    m_hitsPlots_st_matched.fill(particle);
    m_basicPlot_st_matched.fill(particle);
    m_ptPlot_st_matched.fill(particle);
    m_TrackRecoInfoPlots_st_matched.fill(particle);
    if(isSignal) {
      m_trtTesting_st_matched_signal.fill(particle, ei, pv);
      m_hitsPlots_st_matched_signal.fill(particle);
    }
    if (noTruthLink) { // no truth link
      m_trtTesting_st_matched_noTruthLink.fill(particle, ei, pv);
      m_hitsPlots_st_matched_noTruthLink.fill(particle);
      m_basicPlot_st_matched_noTruthLink.fill(particle);
      m_ptPlot_st_matched_noTruthLink.fill(particle);
      m_TrackRecoInfoPlots_st_matched_noTruthLink.fill(particle);
    } else {
      if (barcode >= 200000) { // secondary tracks
        m_trtTesting_st_matched_secondary.fill(particle, ei, pv);
        m_hitsPlots_st_matched_secondary.fill(particle);
        m_basicPlot_st_matched_secondary.fill(particle);
        m_ptPlot_st_matched_secondary.fill(particle);
        m_TrackRecoInfoPlots_st_matched_secondary.fill(particle);
      } else if (barcode > 0) { // primary tracks
        m_trtTesting_st_matched_primary.fill(particle, ei, pv);
        m_hitsPlots_st_matched_primary.fill(particle);
        m_basicPlot_st_matched_primary.fill(particle);
        m_ptPlot_st_matched_primary.fill(particle);
        m_TrackRecoInfoPlots_st_matched_primary.fill(particle);
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
InDetRttLargeD0Plots::fillFake(const xAOD::TrackParticle& trackParticle, const int barcode, const xAOD::EventInfo& ei,
                               const xAOD::VertexContainer& pv) {
  const bool noTruthLink = (barcode == 0)or(barcode == -1);

  if (isLargeD0Track(trackParticle)) {
    m_trtTesting_ld0_fake.fill(trackParticle, ei, pv);
    m_hitsPlots_ld0_fake.fill(trackParticle);
    m_basicPlot_ld0_fake.fill(trackParticle);
    m_ptPlot_ld0_fake.fill(trackParticle);
    m_TrackRecoInfoPlots_ld0_fake.fill(trackParticle);
    if (noTruthLink) { // no truth link
      m_trtTesting_ld0_fake_noTruthLink.fill(trackParticle, ei, pv);
      m_hitsPlots_ld0_fake_noTruthLink.fill(trackParticle);
      m_basicPlot_ld0_fake_noTruthLink.fill(trackParticle);
      m_ptPlot_ld0_fake_noTruthLink.fill(trackParticle);
      m_TrackRecoInfoPlots_ld0_fake_noTruthLink.fill(trackParticle);
    } else {
      if (barcode >= 200000) { // secondary tracks
        m_trtTesting_ld0_fake_secondary.fill(trackParticle, ei, pv);
        m_hitsPlots_ld0_fake_secondary.fill(trackParticle);
        m_basicPlot_ld0_fake_secondary.fill(trackParticle);
        m_ptPlot_ld0_fake_secondary.fill(trackParticle);
        m_TrackRecoInfoPlots_ld0_fake_secondary.fill(trackParticle);
      } else if (barcode > 0) {  // primary tracks
        m_trtTesting_ld0_fake_primary.fill(trackParticle, ei, pv);
        m_hitsPlots_ld0_fake_primary.fill(trackParticle);
        m_basicPlot_ld0_fake_primary.fill(trackParticle);
        m_ptPlot_ld0_fake_primary.fill(trackParticle);
        m_TrackRecoInfoPlots_ld0_fake_primary.fill(trackParticle);
      } else {
        std::cout << "<InDetRttLargeD0Plots::fill> Barcode " << barcode << " was not recognised." << std::endl;
      }
    }
  } else {
    m_trtTesting_st_fake.fill(trackParticle, ei, pv);
    m_hitsPlots_st_fake.fill(trackParticle);
    m_basicPlot_st_fake.fill(trackParticle);
    m_ptPlot_st_fake.fill(trackParticle);
    m_TrackRecoInfoPlots_st_fake.fill(trackParticle);
    if (noTruthLink) { // no truth link
      m_trtTesting_st_fake_noTruthLink.fill(trackParticle, ei, pv);
      m_hitsPlots_st_fake_noTruthLink.fill(trackParticle);
      m_basicPlot_st_fake_noTruthLink.fill(trackParticle);
      m_ptPlot_st_fake_noTruthLink.fill(trackParticle);
      m_TrackRecoInfoPlots_st_fake_noTruthLink.fill(trackParticle);
    } else {
      if (barcode >= 200000) { // secondary tracks
        m_trtTesting_st_fake_secondary.fill(trackParticle, ei, pv);
        m_hitsPlots_st_fake_secondary.fill(trackParticle);
        m_basicPlot_st_fake_secondary.fill(trackParticle);
        m_ptPlot_st_fake_secondary.fill(trackParticle);
        m_TrackRecoInfoPlots_st_fake_secondary.fill(trackParticle);
      } else if (barcode > 0) {  // primary tracks
        m_trtTesting_st_fake_primary.fill(trackParticle, ei, pv);
        m_hitsPlots_st_fake_primary.fill(trackParticle);
        m_basicPlot_st_fake_primary.fill(trackParticle);
        m_ptPlot_st_fake_primary.fill(trackParticle);
        m_TrackRecoInfoPlots_st_fake_primary.fill(trackParticle);
      } else {
        std::cout << "<InDetRttLargeD0Plots::fill> Barcode " << barcode << " was not recognised." << std::endl;
      }
    }
  }
}

// ===================================================================================
// In the loop over all truth particles
// filling all truth particle, no selection
// ===================================================================================

void
InDetRttLargeD0Plots::fillTruth(const xAOD::TruthParticle& truth) {
  // m_basicPlot_truth.fill(truth);

  if ((truth.barcode() > 0)and(truth.barcode() < 200000)) {
    // m_basicPlot_primary_truth.fill(truth);
    m_ptPlot_truth_primary.fill(truth);
  } else {
    // m_basicPlot_secondary_truth.fill(truth);
    m_ptPlot_truth_secondary.fill(truth);
  }
}

void
InDetRttLargeD0Plots::fillEfficiency(const xAOD::TruthParticle& truth, const bool isGood) {
  m_effPlots.fill(truth, isGood);
}

void
InDetRttLargeD0Plots::fillEfficiency(const xAOD::TruthParticle& truth, \
                                     const bool isReconstructed,       \
                                     const bool isLargeD0Track,        \
                                     const bool hasTRTHit,         \
                                     const bool hasTRTOut,         \
                                     const int nTrtHits,          \
                                     const int nTrtTubeHits,          \
                                     const bool isSignal) {
  //unsigned int rec = (unsigned int) isReconstructed;
  const bool LRT = (isLargeD0Track and isReconstructed);
  //unsigned int TRT = (unsigned int) hasTRTHit;
  //unsigned int TRTout = (unsigned int) hasTRTOut;
  bool isStandardTrack = false;
  double tubeFrac = -1.0;

  if (nTrtHits > 0) {
    tubeFrac = ((double) nTrtTubeHits) / ((double) nTrtHits);
  } // determine the fraction of TRT hits which are TRT tube hits (rather than precision hits)
  bool tubeFrac05 = false;
  if (tubeFrac < 0.5) {
    tubeFrac05 = true;
  }
  bool tubeFrac04 = false;
  if (tubeFrac < 0.4) {
    tubeFrac04 = true;
  }
  //unsigned int tubeFrac05 = (unsigned int) tubeFrac05;
  //unsigned int tubeFrac04 = (unsigned int) tubeFrac04;
  if (!isLargeD0Track) {
    isStandardTrack = true;
  }
  bool STD = (isStandardTrack and isReconstructed);

  m_effPlots_all.fill(truth, isReconstructed); // efficiency of all types of tracks - all matched reco tracks/all truth tracks
  m_effPlots_all_trtHit.fill(truth, isReconstructed and hasTRTHit); // how much of efficiency is from tracks with TRT hits?
  m_effPlots_all_trtOut.fill(truth, isReconstructed and hasTRTOut and not hasTRTHit); // how much from tracks with TRT outliers but no
                                                                     // TRT hits?
  m_effPlots_all_noTrt.fill(truth, isReconstructed and not hasTRTHit and not hasTRTOut); // how much from tracks without any TRT
                                                                        // contribution?
  m_effPlots_st.fill(truth, STD); // all matched reco tracks from standard tracking/all truth tracks
  m_effPlots_st_trtHit.fill(truth, STD and hasTRTHit);
  m_effPlots_st_trtOut.fill(truth, STD and hasTRTOut and not hasTRTHit);
  m_effPlots_st_noTrt.fill(truth, STD and not hasTRTHit and not hasTRTOut);
  m_effPlots_ld0.fill(truth, LRT); // all matched reco tracks from large d0 tracking/all truth tracks
  m_effPlots_ld0_trtHit.fill(truth, LRT and hasTRTHit);
  m_effPlots_ld0_trtOut.fill(truth, LRT and hasTRTOut and not hasTRTHit);
  m_effPlots_ld0_noTrt.fill(truth, LRT and not hasTRTHit and not hasTRTOut);

  if (isStandardTrack) { // the point of these plots is that the denominator is only std tracks, so we can see the
                         // peformance of the std tracking itself as comparison to lrt
    m_effPlots_stNonLd0.fill(truth, STD);
    m_effPlots_stNonLd0_trtHit.fill(truth, STD and hasTRTHit);
    m_effPlots_stNonLd0_trtOut.fill(truth, STD and hasTRTOut and not hasTRTHit);
    m_effPlots_stNonLd0_noTrt.fill(truth, STD and not hasTRTHit and not hasTRTOut);
  }

  if (isLargeD0Track) { // the point of these plots is that the denominator is only ld0 tracks, so we can see the
                        // peformance of the ld0 tracking itself
    m_effPlots_ld0NonSt.fill(truth, LRT);
    m_effPlots_ld0NonSt_trtHit.fill(truth, LRT and hasTRTHit);
    m_effPlots_ld0NonSt_trtOut.fill(truth, isReconstructed and LRT and hasTRTOut and not hasTRTHit);
    m_effPlots_ld0NonSt_noTrt.fill(truth, isReconstructed and LRT and not hasTRTHit and not hasTRTOut);
  }

  if (isSignal) { // reco signal tracks/tracks from all signal
    m_effPlots_all_signal.fill(truth, isReconstructed);
    m_effPlots_all_signal_trtHit.fill(truth, isReconstructed and hasTRTHit);
    m_effPlots_all_signal_trtOut.fill(truth, isReconstructed and hasTRTOut and not hasTRTHit);
    m_effPlots_all_signal_noTrt.fill(truth, isReconstructed and not hasTRTHit and not hasTRTOut);
    m_effPlots_all_signal_tubeFrac051.fill(truth, isReconstructed and hasTRTHit and not tubeFrac05); // how much of eff comes from
                                                                                           // tracks with tube frac >
                                                                                           // 0.5?
    m_effPlots_all_signal_tubeFrac0405.fill(truth, isReconstructed and hasTRTHit and tubeFrac05 and not tubeFrac04); // how
                                                                                                               // much
                                                                                                               // from
                                                                                                               // tracks
                                                                                                               // with
                                                                                                               // tube
                                                                                                               // frac
                                                                                                               // (0.4,0.5)
    m_effPlots_all_signal_tubeFrac004.fill(truth, isReconstructed and hasTRTHit and tubeFrac04); // how much from tracks with tube
                                                                                       // frac < 0.4?
    m_effPlots_st_signal.fill(truth, STD);
    m_effPlots_st_signal_trtHit.fill(truth, STD and hasTRTHit);
    m_effPlots_st_signal_trtOut.fill(truth, STD and hasTRTOut and not hasTRTHit);
    m_effPlots_st_signal_noTrt.fill(truth, STD and not hasTRTHit and not hasTRTOut);
    m_effPlots_st_signal_tubeFrac051.fill(truth, STD and hasTRTHit and not tubeFrac05);
    m_effPlots_st_signal_tubeFrac0405.fill(truth, STD and hasTRTHit and tubeFrac05 and not tubeFrac04);
    m_effPlots_st_signal_tubeFrac004.fill(truth, STD and hasTRTHit and tubeFrac04);
    m_effPlots_ld0_signal.fill(truth, LRT);
    m_effPlots_ld0_signal_trtHit.fill(truth, LRT and hasTRTHit);
    m_effPlots_ld0_signal_trtOut.fill(truth, LRT and hasTRTOut and not hasTRTHit);
    m_effPlots_ld0_signal_noTrt.fill(truth, LRT and not hasTRTHit and not hasTRTOut);
    m_effPlots_ld0_signal_tubeFrac051.fill(truth, LRT and hasTRTHit and not tubeFrac05);
    m_effPlots_ld0_signal_tubeFrac0405.fill(truth, LRT and hasTRTHit and tubeFrac05 and not tubeFrac04);
    m_effPlots_ld0_signal_tubeFrac004.fill(truth, LRT and hasTRTHit and tubeFrac04);
    if (isStandardTrack) {
      m_effPlots_stNonLd0_signal.fill(truth, STD);
      m_effPlots_stNonLd0_signal_trtHit.fill(truth, STD and hasTRTHit);
      m_effPlots_stNonLd0_signal_trtOut.fill(truth, STD and hasTRTOut and not hasTRTHit);
      m_effPlots_stNonLd0_signal_noTrt.fill(truth, STD and not hasTRTHit and not hasTRTOut);
      m_effPlots_stNonLd0_signal_tubeFrac051.fill(truth, STD and hasTRTHit and not tubeFrac05);
      m_effPlots_stNonLd0_signal_tubeFrac0405.fill(truth, STD and hasTRTHit and tubeFrac05 and not tubeFrac04);
      m_effPlots_stNonLd0_signal_tubeFrac004.fill(truth, STD and hasTRTHit and tubeFrac04);
    }
    if (isLargeD0Track) {
      m_effPlots_ld0NonSt_signal.fill(truth, LRT);
      m_effPlots_ld0NonSt_signal_trtHit.fill(truth, LRT and hasTRTHit);
      m_effPlots_ld0NonSt_signal_trtOut.fill(truth, LRT and hasTRTOut and not hasTRTHit);
      m_effPlots_ld0NonSt_signal_noTrt.fill(truth, LRT and not hasTRTHit and not hasTRTOut);
      m_effPlots_ld0NonSt_signal_tubeFrac051.fill(truth, LRT and hasTRTHit and not tubeFrac05);
      m_effPlots_ld0NonSt_signal_tubeFrac0405.fill(truth, LRT and hasTRTHit and tubeFrac05 and not tubeFrac04);
      m_effPlots_ld0NonSt_signal_tubeFrac004.fill(truth, LRT and hasTRTHit and tubeFrac04);
    }
  }


  return;
}

// ===================================================================================
// In the loop over truth particles
// filling truth particle with truthMatching > minProbEffHigh (Truth-matched truth tracks)
// ===================================================================================

void
InDetRttLargeD0Plots::fill(const xAOD::TruthParticle& truthParticle) {
  // fill truth plots
  m_TrackTruthInfoPlots.fill(truthParticle);
  // this gets called for the truth particles associated to tracks - exactly what we need!
  // m_effPlots.fillNumerator(truthParticle); // @asogaard
}

void
InDetRttLargeD0Plots::fillBMR(const xAOD::TrackParticle& track, float weight) {
  // fill the plot requiring truth matching probability less than 80%
  m_BadMatchRate.fillBMR(track, weight);
}

void
InDetRttLargeD0Plots::fillRF(const xAOD::TrackParticle& track, float weight) {
  // fill the plots requiring truth matching probability less than 20%
  m_BadMatchRate.fillRF(track, weight);
}

void
InDetRttLargeD0Plots::fill(const xAOD::VertexContainer& vertexContainer) {
  // fill vertex container general properties
  // m_verticesPlots.fill(vertexContainer); //if ever needed
  // fill vertex-specific properties, for all vertices and for hard-scattering vertex
  for (const auto& vtx : vertexContainer.stdcont()) {
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
InDetRttLargeD0Plots::fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei) {
  m_verticesPlots.fill(vertexContainer, ei);
}

void
InDetRttLargeD0Plots::fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter) {
  m_nTracks.fill(freq, counter);
}

void
InDetRttLargeD0Plots::fillFakeRate(const xAOD::TrackParticle& particle, \
                                   const bool fake,         \
                                   const int barcode,           \
                                   const bool trtHit,           \
                                   const bool trtOut,           \
                                   const int nTRTHit,           \
                                   const int nTRTTube) {
  // Declare and get necessary variables.
  const bool isLargeD0 = isLargeD0Track(particle);
  const bool primary = (barcode < 200000 and barcode > 0); // match and ...
  const bool secondary = (barcode >= 200000); // match and ...
  const bool notruthlink = ((barcode == 0)or(barcode == -1));
  double tubeFrac = -1.0;

  if (nTRTHit > 0) {
    tubeFrac = ((double) nTRTTube) / ((double) nTRTHit);
  }
  bool tubeFrac05 = false;
  if (tubeFrac < 0.5) {
    tubeFrac05 = true;
  }
  bool tubeFrac04 = false;
  if (tubeFrac < 0.4) {
    tubeFrac04 = true;
  }

  // Fill fakerate distributions.
  /* "New" definition, cf.
     [https://indico.cern.ch/event/559023/contributions/2255673/attachments/1314623/1968909/2016.07.25.pdf] */
  if (isLargeD0) { // ld0 fakes (matching prob < 0.5) or not associated to a particle with finite barcode/all ld0 tracks
    m_fakePlots_ld0.fill(particle, fake); // numerator is tracks that are fake, regardless of barcode status
    m_fakePlots_ld0_trtHit.fill(particle, fake and trtHit); // for tracks with any number of trt hits
    m_fakePlots_ld0_trtOut.fill(particle, fake and trtOut and not trtHit); // for tracks with some trt outliers but no
                                                                           // trt hits
    m_fakePlots_ld0_noTrt.fill(particle, fake and not trtHit and not trtOut); // tracks with no trt hits/outliers of any
                                                                              // kind
    m_fakePlots_ld0_tubeFrac051.fill(particle, fake and trtHit and not tubeFrac05); // tracks with trt hits with tube
                                                                                    // fraction > 0.5
    m_fakePlots_ld0_tubeFrac0405.fill(particle, fake and trtHit and tubeFrac05 and not tubeFrac04); // tracks with trt
                                                                                                    // hits and tube
                                                                                                    // fraction (.4,.5);
    m_fakePlots_ld0_tubeFrac004.fill(particle, fake and trtHit and tubeFrac04); // tracks with trt hits and tube
                                                                                // fraction < 0.4
    m_fakePlots_ld0_primary.fill(particle, fake and primary); // numeator is tracks that are fake and have barcode > 0
                                                              // and < 200 000
    m_fakePlots_ld0_primary_trtHit.fill(particle, fake and primary and trtHit);
    m_fakePlots_ld0_primary_trtOut.fill(particle, fake and primary and trtOut and not trtHit);
    m_fakePlots_ld0_primary_noTrt.fill(particle, fake and primary and not trtHit and not trtOut);
    m_fakePlots_ld0_primary_tubeFrac051.fill(particle, fake and primary and trtHit and not tubeFrac05);
    m_fakePlots_ld0_primary_tubeFrac0405.fill(particle, fake and primary and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_ld0_primary_tubeFrac004.fill(particle, fake and primary and trtHit and tubeFrac04);
    m_fakePlots_ld0_secondary.fill(particle, fake and secondary); // numerator is tracks that are fake and have barcode
                                                                  // > 200 000
    m_fakePlots_ld0_secondary_trtHit.fill(particle, fake and secondary and trtHit);
    m_fakePlots_ld0_secondary_trtOut.fill(particle, fake and secondary and trtOut and not trtHit);
    m_fakePlots_ld0_secondary_noTrt.fill(particle, fake and secondary and not trtHit and not trtOut);
    m_fakePlots_ld0_secondary_tubeFrac051.fill(particle, fake and secondary and trtHit and not tubeFrac05);
    m_fakePlots_ld0_secondary_tubeFrac0405.fill(particle,
                                                fake and secondary and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_ld0_secondary_tubeFrac004.fill(particle, fake and secondary and trtHit and tubeFrac04);
    m_fakePlots_ld0_linked.fill(particle, fake and(primary or secondary)); // numerator is tracks that are fake and have
                                                                           // any finite barcode
    m_fakePlots_ld0_linked_trtHit.fill(particle, fake and(primary or secondary) and trtHit);
    m_fakePlots_ld0_linked_trtOut.fill(particle, fake and(primary or secondary) and trtOut and not trtHit);
    m_fakePlots_ld0_linked_noTrt.fill(particle, fake and(primary or secondary) and not trtHit and not trtOut);
    m_fakePlots_ld0_linked_tubeFrac051.fill(particle, fake and(primary or secondary) and trtHit and not tubeFrac05);
    m_fakePlots_ld0_linked_tubeFrac0405.fill(particle, fake and(
                                               primary or secondary) and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_ld0_linked_tubeFrac004.fill(particle, fake and(primary or secondary) and trtHit and tubeFrac04);
    m_fakePlots_ld0_notruth.fill(particle, fake and notruthlink); // numerator is tracks that are fake and have barcode
                                                                  // 0/no barcode
    m_fakePlots_ld0_notruth_trtHit.fill(particle, fake and notruthlink and trtHit);
    m_fakePlots_ld0_notruth_trtOut.fill(particle, fake and notruthlink and trtOut and not trtHit);
    m_fakePlots_ld0_notruth_noTrt.fill(particle, fake and notruthlink and not trtHit and not trtOut);
    m_fakePlots_ld0_notruth_tubeFrac051.fill(particle, fake and notruthlink and trtHit and not tubeFrac05);
    m_fakePlots_ld0_notruth_tubeFrac0405.fill(particle,
                                              fake and notruthlink and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_ld0_notruth_tubeFrac004.fill(particle, fake and notruthlink and trtHit and tubeFrac04);
    m_fakePlots_ld0_combined.fill(particle, (fake or notruthlink)); // numerator tracks that are either fake or barcode
                                                                    // 0/no barcode
    m_fakePlots_ld0_combined_trtHit.fill(particle, (fake or notruthlink) and trtHit);
    m_fakePlots_ld0_combined_trtOut.fill(particle, (fake or notruthlink) and trtOut and not trtHit);
    m_fakePlots_ld0_combined_noTrt.fill(particle, (fake or notruthlink) and not trtHit and not trtOut);
    m_fakePlots_ld0_combined_tubeFrac051.fill(particle, (fake or notruthlink) and trtHit and not tubeFrac05);
    m_fakePlots_ld0_combined_tubeFrac0405.fill(particle,
                                               (fake or notruthlink) and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_ld0_combined_tubeFrac004.fill(particle, (fake or notruthlink) and trtHit and tubeFrac04);
  } else {// st fakes (matching prob < 0.5) or not associated to a particle with finite barcode/all std tracks
    m_fakePlots_st.fill(particle, fake);
    m_fakePlots_st_trtHit.fill(particle, fake and trtHit);
    m_fakePlots_st_trtOut.fill(particle, fake and trtOut and not trtHit);
    m_fakePlots_st_noTrt.fill(particle, fake and not trtHit and not trtOut);
    m_fakePlots_st_primary.fill(particle, fake and primary);
    m_fakePlots_st_tubeFrac051.fill(particle, fake and trtHit and not tubeFrac05);
    m_fakePlots_st_tubeFrac0405.fill(particle, fake and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_st_tubeFrac004.fill(particle, fake and trtHit and tubeFrac04);
    m_fakePlots_st_primary_trtHit.fill(particle, fake and primary and trtHit);
    m_fakePlots_st_primary_trtOut.fill(particle, fake and primary and trtOut and not trtHit);
    m_fakePlots_st_primary_noTrt.fill(particle, fake and primary and not trtHit and not trtOut);
    m_fakePlots_st_primary_tubeFrac051.fill(particle, fake and primary and trtHit and not tubeFrac05);
    m_fakePlots_st_primary_tubeFrac0405.fill(particle, fake and primary and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_st_primary_tubeFrac004.fill(particle, fake and primary and trtHit and tubeFrac04);
    m_fakePlots_st_secondary.fill(particle, fake and secondary);
    m_fakePlots_st_secondary_trtHit.fill(particle, fake and secondary and trtHit);
    m_fakePlots_st_secondary_trtOut.fill(particle, fake and secondary and trtOut and not trtHit);
    m_fakePlots_st_secondary_noTrt.fill(particle, fake and secondary and not trtHit and not trtOut);
    m_fakePlots_st_secondary_tubeFrac051.fill(particle, fake and secondary and trtHit and not tubeFrac05);
    m_fakePlots_st_secondary_tubeFrac0405.fill(particle,
                                               fake and secondary and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_st_secondary_tubeFrac004.fill(particle, fake and secondary and trtHit and tubeFrac04);
    m_fakePlots_st_linked_trtHit.fill(particle, fake and(primary or secondary) and trtHit);
    m_fakePlots_st_linked_trtOut.fill(particle, fake and(primary or secondary) and trtOut and not trtHit);
    m_fakePlots_st_linked_noTrt.fill(particle, fake and(primary or secondary) and not trtHit and not trtOut);
    m_fakePlots_st_linked_tubeFrac051.fill(particle, fake and(primary or secondary) and trtHit and not tubeFrac05);
    m_fakePlots_st_linked_tubeFrac0405.fill(particle, fake and(
                                              primary or secondary) and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_st_linked_tubeFrac004.fill(particle, fake and(primary or secondary) and trtHit and tubeFrac04);
    m_fakePlots_st_linked.fill(particle, fake and(primary or secondary));
    m_fakePlots_st_notruth.fill(particle, fake and notruthlink);
    m_fakePlots_st_notruth_trtHit.fill(particle, fake and notruthlink and trtHit);
    m_fakePlots_st_notruth_trtOut.fill(particle, fake and notruthlink and trtOut and not trtHit);
    m_fakePlots_st_notruth_noTrt.fill(particle, fake and notruthlink and not trtHit and not trtOut);
    m_fakePlots_st_notruth_tubeFrac051.fill(particle, fake and notruthlink and trtHit and not tubeFrac05);
    m_fakePlots_st_notruth_tubeFrac0405.fill(particle,
                                             fake and notruthlink and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_st_notruth_tubeFrac004.fill(particle, fake and notruthlink and trtHit and tubeFrac04);
    m_fakePlots_st_combined.fill(particle, (fake or notruthlink));
    m_fakePlots_st_combined_trtHit.fill(particle, (fake or notruthlink) and trtHit);
    m_fakePlots_st_combined_trtOut.fill(particle, (fake or notruthlink) and trtOut and not trtHit);
    m_fakePlots_st_combined_noTrt.fill(particle, (fake or notruthlink) and not trtHit and not trtOut);
    m_fakePlots_st_combined_tubeFrac051.fill(particle, (fake or notruthlink) and trtHit and not tubeFrac05);
    m_fakePlots_st_combined_tubeFrac0405.fill(particle,
                                              (fake or notruthlink) and trtHit and tubeFrac05 and not tubeFrac04);
    m_fakePlots_st_combined_tubeFrac004.fill(particle, (fake or notruthlink) and trtHit and tubeFrac04);
  }
}
