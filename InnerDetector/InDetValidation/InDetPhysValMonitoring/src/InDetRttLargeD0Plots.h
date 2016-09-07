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

///class holding all plots for Inner Detector RTT Validation and implementing fill methods
class InDetRttLargeD0Plots: public InDetPlotBase {
public:
  InDetRttLargeD0Plots(InDetPlotBase *pParent, const std::string &dirName);
  ///fill for things needing truth and track only
  // void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle); // not used. SC
  ///fill for things needing track only
  void fill(const xAOD::TrackParticle &particle);
  void fill(const xAOD::TrackParticle &particle, const int barcode);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle &particle);
  ///fill for things needing all truth - not just the ones from the reco tracks
  void fillTruth(const xAOD::TruthParticle &particle);

  void pro_fill(const xAOD::TruthParticle &truth, float weight);

  void fillEfficiency(const xAOD::TruthParticle &particle,      \
                      const bool isReconstructed,               \
                      const bool isLargeD0Track = false,        \
                      const bool isTRTExtended = false,     \
                      const bool isSignal = false);


  // fill the fake and bad match rate plots
  void fillBMR(const xAOD::TrackParticle &track, float weight);
  void fillRF(const xAOD::TrackParticle &track, float weight);

  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer &vertexContainer);
  ///fill reco-vertex related plots that need EventInfo
  void fill(const xAOD::VertexContainer &vertexContainer, const xAOD::EventInfo &ei);

  virtual ~InDetRttLargeD0Plots() {/**nop**/
  };
  ///fill for Counters
  void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory counter);
  ///fill for fakes
  void fillFakeRate(const xAOD::TrackParticle &particle, const bool isFake,
                    const InDetPerfPlot_fakes::Category c = InDetPerfPlot_fakes::ALL);

  ///fill for fakes (Siinn)
  void fillFake(const xAOD::TrackParticle &particle);
private:
  // ============================================
  // Plots used for LargeD0 performance study
  // ============================================

  // Testing track categories/authors for TRT studies
  InDetPerfPlot_TrtTest m_trtTesting_nonfake;
  InDetPerfPlot_TrtTest m_trtTesting_matching_truthLink_primary;
  InDetPerfPlot_TrtTest m_trtTesting_matching_truthLink_secondary;
  InDetPerfPlot_TrtTest m_trtTesting_matching_noTruthLink;
  InDetPerfPlot_TrtTest m_trtTesting_fakes;
  //       InDetPerfPlot_TrtTest m_trtTesting_fakes_noTruthLink;

  // hit plots
  Trk::IDHitPlots m_hitsPlots_nonfake_st;
  Trk::IDHitPlots m_hitsPlots_matching_truthLink_primary_st;
  Trk::IDHitPlots m_hitsPlots_matching_truthLink_secondary_st;
  Trk::IDHitPlots m_hitsPlots_matching_NotruthLink_st;
  Trk::IDHitPlots m_hitsPlots_fake_st;

  Trk::IDHitPlots m_hitsPlots_nonfake_ld0;
  Trk::IDHitPlots m_hitsPlots_matching_truthLink_primary_ld0;
  Trk::IDHitPlots m_hitsPlots_matching_truthLink_secondary_ld0;
  Trk::IDHitPlots m_hitsPlots_matching_NotruthLink_ld0;
  Trk::IDHitPlots m_hitsPlots_fake_ld0;

  // basic plots
  InDetBasicPlot m_basicPlot_nonfake_st;
  InDetBasicPlot m_basicPlot_nonfake_ld0;
  InDetBasicPlot m_basicPlot_fake_st;
  InDetBasicPlot m_basicPlot_fake_ld0;
  InDetBasicPlot m_basicPlot_truth;

  InDetBasicPlot m_basicPlot_primary_truth;
  InDetBasicPlot m_basicPlot_secondary_truth;

  InDetBasicPlot m_basicPlot_pileup_primary_truth;
  InDetBasicPlot m_basicPlot_pileup_secondary_truth;

  // pT plots
  InDetPerfPlot_Pt m_ptPlot_nonfake_st;
  InDetPerfPlot_Pt m_ptPlot_nonfake_ld0;
  InDetPerfPlot_Pt m_ptPlot_fake_st;
  InDetPerfPlot_Pt m_ptPlot_fake_ld0;

  InDetPerfPlot_Pt m_ptPlot_primary;
  InDetPerfPlot_Pt m_ptPlot_secondary;

  InDetPerfPlot_Pt m_ptPlot_pileup_primary;
  InDetPerfPlot_Pt m_ptPlot_pileup_secondary;


  // Reco info plots
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_nonfake_st;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_nonfake_ld0;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_fake_st;
  Trk::RecoInfoPlots m_TrackRecoInfoPlots_fake_ld0;

  // Fake rate plots.
  InDetPerfPlot_ExtendedFakes m_fakePlots_primary;
  InDetPerfPlot_ExtendedFakes m_fakePlots_secondary;
  InDetPerfPlot_ExtendedFakes m_fakePlots_noTruth;
  InDetPerfPlot_ExtendedFakes m_fakePlots_rest;

  InDetPerfPlot_ExtendedFakes m_fakePlots_primary_LRT;
  InDetPerfPlot_ExtendedFakes m_fakePlots_secondary_LRT;
  InDetPerfPlot_ExtendedFakes m_fakePlots_noTruth_LRT;
  InDetPerfPlot_ExtendedFakes m_fakePlots_rest_LRT;

  // Efficiency plots
  InDetPerfPlot_Eff m_effPlotsStd;
  InDetPerfPlot_Eff m_effPlotsStdSignal;
  InDetPerfPlot_Eff m_effPlotsStdTRT;
  InDetPerfPlot_Eff m_effPlotsStdTRTSignal;
  InDetPerfPlot_Eff m_effPlotsStdnoTRT;
  InDetPerfPlot_Eff m_effPlotsStdnoTRTSignal;
  InDetPerfPlot_Eff m_effPlotsLRT;
  InDetPerfPlot_Eff m_effPlotsLRTSignal;
  InDetPerfPlot_Eff m_effPlotsLRTTRT;
  InDetPerfPlot_Eff m_effPlotsLRTTRTSignal;
  InDetPerfPlot_Eff m_effPlotsLRTnoTRT;
  InDetPerfPlot_Eff m_effPlotsLRTnoTRTSignal;
  InDetPerfPlot_Eff m_effPlotsLRTnonStd;
  InDetPerfPlot_Eff m_effPlotsLRTnonStdSignal;
  InDetPerfPlot_Eff m_effPlotsLRTnonStdTRT;
  InDetPerfPlot_Eff m_effPlotsLRTnonStdnoTRT;
  InDetPerfPlot_Eff m_effPlotsAll;
  InDetPerfPlot_Eff m_effPlotsAllSignal;
  InDetPerfPlot_Eff m_effPlotsAllTRT;
  InDetPerfPlot_Eff m_effPlotsAllTRTSignal;
  InDetPerfPlot_Eff m_effPlotsAllnoTRT;
  InDetPerfPlot_Eff m_effPlotsAllnoTRTSignal;

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
