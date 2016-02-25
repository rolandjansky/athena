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


//std includes
#include <string>

//local includes

#include "InDetPlotBase.h"
#include "InDetPerfPlot_Pt.h"
#include "InDetBasicPlot.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "InDetPerfPlot_nTracks.h"
#include "InDetPerfPlot_res.h"
//#include "InDetPerfPlot_pull.h"
#include "InDetPerfPlot_fakes.h"
#include "InDetPerfPlot_Eff.h"
#include "InDetPerfPlot_hitResidual.h"

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
class InDetRttLargeD0Plots:public InDetPlotBase {
public:
  InDetRttLargeD0Plots(InDetPlotBase * pParent, const std::string & dirName);
  void SetPrimaryEtaCut( float eta );
  ///fill for things needing truth and track only
  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthParticle);
  ///fill for things needing track only
  void fill(const xAOD::TrackParticle& particle);
  ///fill for things needing truth only
  void fill(const xAOD::TruthParticle& particle);
  ///fill for things needing all truth - not just the ones from the reco tracks
  void fillTruth(const xAOD::TruthParticle& particle);
  void fillEfficiency(const xAOD::TruthParticle& particle, const bool isReconstructed, const bool isStandardTrack);

  //fill the duplicate plots
  //void fillDupTrack(const xAOD::TrackParticle& particle);

  void fillBMR_Denom(const xAOD::TrackParticle& particle);
  void fillBMR_Num(const xAOD::TrackParticle& particle);
  void fillRF_Num(const xAOD::TrackParticle& particle);

  ///fill reco-vertex related plots
  void fill(const xAOD::VertexContainer& vertexContainer);
  ///fill reco-vertex related plots that need EventInfo
  void fill(const xAOD::VertexContainer& vertexContainer, const xAOD::EventInfo& ei);


  virtual ~InDetRttLargeD0Plots(){/**nop**/};
  ///fill for Counters
  void fillCounter(const unsigned int freq, const InDetPerfPlot_nTracks::CounterCategory  counter);
  ///fill for fakes
  void fillFakeRate(const xAOD::TrackParticle& particle, const bool match, const InDetPerfPlot_fakes::Category c = InDetPerfPlot_fakes::ALL);

  ///fill for fakes (Siinn)
  void fillFake(const xAOD::TrackParticle& particle);


private:

       //============================================
       // Plots used for LargeD0 performance study
       //============================================

       // hit plots
       Trk::IDHitPlots m_hitsPlots_nonfake_st;
       Trk::IDHitPlots m_hitsPlots_nonfake_ld0;
       Trk::IDHitPlots m_hitsPlots_fake_st;
       Trk::IDHitPlots m_hitsPlots_fake_ld0;

       // basic plots
       InDetBasicPlot m_basicPlot_nonfake_st;
       InDetBasicPlot m_basicPlot_nonfake_ld0;
       InDetBasicPlot m_basicPlot_fake_st;
       InDetBasicPlot m_basicPlot_fake_ld0;
       InDetBasicPlot m_basicPlot_truth;

       // pT plots
       InDetPerfPlot_Pt m_ptPlot_nonfake_st;
       InDetPerfPlot_Pt m_ptPlot_nonfake_ld0;
       InDetPerfPlot_Pt m_ptPlot_fake_st;
       InDetPerfPlot_Pt m_ptPlot_fake_ld0;

       // Reco info plots
       Trk::RecoInfoPlots m_TrackRecoInfoPlots_nonfake_st;
       Trk::RecoInfoPlots m_TrackRecoInfoPlots_nonfake_ld0;
       Trk::RecoInfoPlots m_TrackRecoInfoPlots_fake_st;
       Trk::RecoInfoPlots m_TrackRecoInfoPlots_fake_ld0;

       // Fake rate (rather, fraction) plots.
       InDetPerfPlot_fakes m_fakePlots;
       InDetPerfPlot_fakes m_fakePlotsLRT;

        // The plots below are not used for the moment
       InDetPerfPlot_Pt  m_ptPlot;
       InDetBasicPlot m_basicPlot;
       Trk::ParamPlots m_PtEtaPlots;
       Trk::ImpactPlots m_IPPlots;
       Trk::RecoInfoPlots m_TrackRecoInfoPlots;
       Trk::TruthInfoPlots m_TrackTruthInfoPlots;
       InDetPerfPlot_nTracks m_nTracks;
       InDetPerfPlot_res m_resPlots;
       InDetPerfPlot_hitResidual m_hitResidualPlot;
       InDetPerfPlot_HitDetailed m_hitsDetailedPlots;
       InDetPerfPlot_Eff m_effPlots;
       InDet_BadMatchRate m_BadMatchRate;
       InDetPerfPlot_VertexContainer m_verticesPlots;
       InDetPerfPlot_Vertex m_vertexPlots;
       InDetPerfPlot_Vertex m_hardScatterVertexPlots;

       // not useful 
       Trk::IDHitPlots m_hitsPlots;
       Trk::IDHitPlots m_hitsMatchedTracksPlots;
       Trk::IDHitPlots m_hitsFakeTracksPlots;

       //Set scripts to analyze the duplicates that appear

       std::string m_trackParticleTruthProbKey;
       float m_truthProbThreshold;
       float m_truthProbLowThreshold;
       float m_truthPrimaryEtaCut;
};




#endif
