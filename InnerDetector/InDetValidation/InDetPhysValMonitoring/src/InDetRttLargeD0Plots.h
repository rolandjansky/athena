/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETPHYSVALMONITORING_InDetRttLargeD0Plots
#define INDETPHYSVALMONITORING_InDetRttLargeD0Plots
/**
 * @file InDetRttLargeD0Plots.h
 * @author shaun roe
 * @date 29/3/2014
 **/


// std includes
#include <string>
#include <map> // @asogaard

// local includes

#include "InDetPlotBase.h"
#include "InDetPerfPlot_Pt.h"
#include "InDetBasicPlot.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "InDetPerfPlot_res.h"
#include "InDetPerfPlot_fakes.h"
#include "InDetPerfPlot_ExtendedFakes.h"
#include "InDetPerfPlot_Eff.h"
#include "InDetPerfPlot_hitResidual.h"
#include "InDetPerfPlot_TrtTest.h"

#include "InDet_BadMatchRate.h"

#include "TrkValHistUtils/IDHitPlots.h"
#include "InDetPerfPlot_HitDetailed.h"
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_VertexContainer.h"


#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttLargeD0Plots: public InDetPlotBase {
public:
  InDetRttLargeD0Plots(InDetPlotBase* pParent, const std::string& dirName);
  ///fill for things needing truth and track only
  // void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle); // not used. SC
  ///fill for things needing track only
  void fill(const xAOD::TrackParticle& particle);
  void fill(const xAOD::TrackParticle& particle,		\
	    const int barcode,					\
	    const xAOD::EventInfo& ei,				\
            const xAOD::VertexContainer& pv,			\
	    const bool isSignal = false);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle& particle);
  ///fill for things needing all truth - not just the ones from the reco tracks
  void fillTruth(const xAOD::TruthParticle& particle);

  void fillEfficiency(const xAOD::TruthParticle& truth, const bool isGood);

  void fillEfficiency(const xAOD::TruthParticle& particle,      \
                      const bool isReconstructed,               \
                      const bool isLargeD0Track,     \
                      const bool hasTRTHit = false,     \
                      const bool hasTRTOut = false,     \
                      const int nTrtHits = 0,          \
                      const int nTrtTubeHits = 0,      \
                      const bool isSignal = false);


  // fill the fake and bad match rate plots
  void fillBMR(const xAOD::TrackParticle& track, float weight);
  void fillRF(const xAOD::TrackParticle& track, float weight);

  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer& vertexContainer);
  ///fill reco-vertex related plots that need EventInfo
  void fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei);

  virtual ~InDetRttLargeD0Plots() {/**nop**/
  };
  ///fill for Counters
  void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter);
  ///fill for fakes
  void fillFakeRate(const xAOD::TrackParticle& particle,        \
                    const bool isFake,                  \
                    const int barcode = -1,             \
                    const bool trtHit = false,              \
                    const bool trtOut = false,              \
                    const int nTRTHit = 0,              \
                    const int nTRTTube = 0);

  ///fill for fakes (Siinn)
  void fillFake(const xAOD::TrackParticle& particle, const int barcode, const xAOD::EventInfo& ei,
                const xAOD::VertexContainer& pv);
private:
  // ============================================
  // Plots used for LargeD0 performance study
  // ============================================

  // Testing track categories/authors for TRT studies
  InDetPerfPlot_TrtTest m_trtTesting_st_matched;
  InDetPerfPlot_TrtTest m_trtTesting_st_matched_signal;
  InDetPerfPlot_TrtTest m_trtTesting_st_matched_primary;
  InDetPerfPlot_TrtTest m_trtTesting_st_matched_secondary;
  InDetPerfPlot_TrtTest m_trtTesting_st_matched_noTruthLink;
  InDetPerfPlot_TrtTest m_trtTesting_st_fake;
  InDetPerfPlot_TrtTest m_trtTesting_st_fake_primary;
  InDetPerfPlot_TrtTest m_trtTesting_st_fake_secondary;
  InDetPerfPlot_TrtTest m_trtTesting_st_fake_noTruthLink;

  InDetPerfPlot_TrtTest m_trtTesting_ld0_matched;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_matched_signal;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_matched_primary;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_matched_secondary;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_matched_noTruthLink;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_fake;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_fake_primary;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_fake_secondary;
  InDetPerfPlot_TrtTest m_trtTesting_ld0_fake_noTruthLink;

  // hit plots
  Trk::IDHitPlots m_hitsPlots_st_matched;
  Trk::IDHitPlots m_hitsPlots_st_matched_signal;
  Trk::IDHitPlots m_hitsPlots_st_matched_primary;
  Trk::IDHitPlots m_hitsPlots_st_matched_secondary;
  Trk::IDHitPlots m_hitsPlots_st_matched_noTruthLink;
  Trk::IDHitPlots m_hitsPlots_st_fake;
  Trk::IDHitPlots m_hitsPlots_st_fake_primary;
  Trk::IDHitPlots m_hitsPlots_st_fake_secondary;
  Trk::IDHitPlots m_hitsPlots_st_fake_noTruthLink;

  Trk::IDHitPlots m_hitsPlots_ld0_matched;
  Trk::IDHitPlots m_hitsPlots_ld0_matched_signal;
  Trk::IDHitPlots m_hitsPlots_ld0_matched_primary;
  Trk::IDHitPlots m_hitsPlots_ld0_matched_secondary;
  Trk::IDHitPlots m_hitsPlots_ld0_matched_noTruthLink;
  Trk::IDHitPlots m_hitsPlots_ld0_fake;
  Trk::IDHitPlots m_hitsPlots_ld0_fake_primary;
  Trk::IDHitPlots m_hitsPlots_ld0_fake_secondary;
  Trk::IDHitPlots m_hitsPlots_ld0_fake_noTruthLink;

  // basic plots
  InDetBasicPlot m_basicPlot_st_matched;
  InDetBasicPlot m_basicPlot_st_matched_primary;
  InDetBasicPlot m_basicPlot_st_matched_secondary;
  InDetBasicPlot m_basicPlot_st_matched_noTruthLink;
  InDetBasicPlot m_basicPlot_st_fake;
  InDetBasicPlot m_basicPlot_st_fake_primary;
  InDetBasicPlot m_basicPlot_st_fake_secondary;
  InDetBasicPlot m_basicPlot_st_fake_noTruthLink;

  InDetBasicPlot m_basicPlot_ld0_matched;
  InDetBasicPlot m_basicPlot_ld0_matched_primary;
  InDetBasicPlot m_basicPlot_ld0_matched_secondary;
  InDetBasicPlot m_basicPlot_ld0_matched_noTruthLink;
  InDetBasicPlot m_basicPlot_ld0_fake;
  InDetBasicPlot m_basicPlot_ld0_fake_primary;
  InDetBasicPlot m_basicPlot_ld0_fake_secondary;
  InDetBasicPlot m_basicPlot_ld0_fake_noTruthLink;

  // pT plots
  InDetPerfPlot_Pt m_ptPlot_st_matched;
  InDetPerfPlot_Pt m_ptPlot_st_matched_primary;
  InDetPerfPlot_Pt m_ptPlot_st_matched_secondary;
  InDetPerfPlot_Pt m_ptPlot_st_matched_noTruthLink;
  InDetPerfPlot_Pt m_ptPlot_st_fake;
  InDetPerfPlot_Pt m_ptPlot_st_fake_primary;
  InDetPerfPlot_Pt m_ptPlot_st_fake_secondary;
  InDetPerfPlot_Pt m_ptPlot_st_fake_noTruthLink;

  InDetPerfPlot_Pt m_ptPlot_ld0_matched;
  InDetPerfPlot_Pt m_ptPlot_ld0_matched_primary;
  InDetPerfPlot_Pt m_ptPlot_ld0_matched_secondary;
  InDetPerfPlot_Pt m_ptPlot_ld0_matched_noTruthLink;
  InDetPerfPlot_Pt m_ptPlot_ld0_fake;
  InDetPerfPlot_Pt m_ptPlot_ld0_fake_primary;
  InDetPerfPlot_Pt m_ptPlot_ld0_fake_secondary;
  InDetPerfPlot_Pt m_ptPlot_ld0_fake_noTruthLink;

  InDetPerfPlot_Pt m_ptPlot_truth_primary;
  InDetPerfPlot_Pt m_ptPlot_truth_secondary;

  InDetPerfPlot_Pt m_ptPlot_pileup_primary;
  InDetPerfPlot_Pt m_ptPlot_pileup_secondary;


  // Reco info plots
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_matched;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_matched_primary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_matched_secondary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_matched_noTruthLink;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_fake;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_fake_primary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_fake_secondary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_st_fake_noTruthLink;

  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_matched;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_matched_primary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_matched_secondary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_matched_noTruthLink;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_fake;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_fake_primary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_fake_secondary;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_ld0_fake_noTruthLink;


  // Fake rate plots.
  InDetPerfPlot_ExtendedFakes m_fakePlots_st;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_primary_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_secondary_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_linked_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_notruth_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_st_combined_tubeFrac004;

  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_primary_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_secondary_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_linked_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_notruth_tubeFrac004;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined_trtHit;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined_trtOut;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined_noTrt;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined_tubeFrac051;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined_tubeFrac0405;
  InDetPerfPlot_ExtendedFakes m_fakePlots_ld0_combined_tubeFrac004;


  // Efficiency plots
  InDetPerfPlot_Eff m_effPlots_all;
  InDetPerfPlot_Eff m_effPlots_all_trtHit;
  InDetPerfPlot_Eff m_effPlots_all_trtOut;
  InDetPerfPlot_Eff m_effPlots_all_noTrt;
  InDetPerfPlot_Eff m_effPlots_all_signal;
  InDetPerfPlot_Eff m_effPlots_all_signal_trtHit;
  InDetPerfPlot_Eff m_effPlots_all_signal_trtOut;
  InDetPerfPlot_Eff m_effPlots_all_signal_noTrt;
  InDetPerfPlot_Eff m_effPlots_all_signal_tubeFrac051;
  InDetPerfPlot_Eff m_effPlots_all_signal_tubeFrac0405;
  InDetPerfPlot_Eff m_effPlots_all_signal_tubeFrac004;

  InDetPerfPlot_Eff m_effPlots_st;
  InDetPerfPlot_Eff m_effPlots_st_trtHit;
  InDetPerfPlot_Eff m_effPlots_st_trtOut;
  InDetPerfPlot_Eff m_effPlots_st_noTrt;
  InDetPerfPlot_Eff m_effPlots_st_signal;
  InDetPerfPlot_Eff m_effPlots_st_signal_trtHit;
  InDetPerfPlot_Eff m_effPlots_st_signal_trtOut;
  InDetPerfPlot_Eff m_effPlots_st_signal_noTrt;
  InDetPerfPlot_Eff m_effPlots_st_signal_tubeFrac051;
  InDetPerfPlot_Eff m_effPlots_st_signal_tubeFrac0405;
  InDetPerfPlot_Eff m_effPlots_st_signal_tubeFrac004;

  InDetPerfPlot_Eff m_effPlots_ld0;
  InDetPerfPlot_Eff m_effPlots_ld0_trtHit;
  InDetPerfPlot_Eff m_effPlots_ld0_trtOut;
  InDetPerfPlot_Eff m_effPlots_ld0_noTrt;
  InDetPerfPlot_Eff m_effPlots_ld0_signal;
  InDetPerfPlot_Eff m_effPlots_ld0_signal_trtHit;
  InDetPerfPlot_Eff m_effPlots_ld0_signal_trtOut;
  InDetPerfPlot_Eff m_effPlots_ld0_signal_noTrt;
  InDetPerfPlot_Eff m_effPlots_ld0_signal_tubeFrac051;
  InDetPerfPlot_Eff m_effPlots_ld0_signal_tubeFrac0405;
  InDetPerfPlot_Eff m_effPlots_ld0_signal_tubeFrac004;

  InDetPerfPlot_Eff m_effPlots_stNonLd0;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_trtHit;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_trtOut;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_noTrt;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal_trtHit;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal_trtOut;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal_noTrt;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal_tubeFrac051;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal_tubeFrac0405;
  InDetPerfPlot_Eff m_effPlots_stNonLd0_signal_tubeFrac004;

  InDetPerfPlot_Eff m_effPlots_ld0NonSt;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_trtHit;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_trtOut;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_noTrt;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal_trtHit;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal_trtOut;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal_noTrt;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal_tubeFrac051;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal_tubeFrac0405;
  InDetPerfPlot_Eff m_effPlots_ld0NonSt_signal_tubeFrac004;

  InDetPerfPlot_Eff m_effPlots;

  // The plots below are not used for the moment
  InDetPerfPlot_Pt m_ptPlot;
  InDetBasicPlot m_basicPlot;
  Trk::ParamPlots m_PtEtaPlots;
  Trk::ImpactPlots m_IPPlots;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots;
  Trk::TruthInfoPlots m_TrackTruthInfoPlots;
  InDetPerfPlot_nTracks m_nTracks;
  InDetPerfPlot_res m_resPlots;
  InDetPerfPlot_hitResidual m_hitResidualPlot;
  InDetPerfPlot_HitDetailed m_hitsDetailedPlots;

  InDet_BadMatchRate m_BadMatchRate;
  InDetPerfPlot_VertexContainer m_verticesPlots;
  InDetPerfPlot_Vertex m_vertexPlots;
  InDetPerfPlot_Vertex m_hardScatterVertexPlots;

  // not useful
  Trk::IDHitPlots m_hitsPlots;
  Trk::IDHitPlots m_hitsMatchedTracksPlots;
  Trk::IDHitPlots m_hitsFakeTracksPlots;

  std::string m_trackParticleTruthProbKey;
  float m_truthProbLowThreshold;
};




#endif
