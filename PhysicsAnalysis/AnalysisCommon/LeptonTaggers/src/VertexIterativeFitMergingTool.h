// This is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_VERTEXITERATIVEFITMERGINGTOOL_H
#define PROMPT_VERTEXITERATIVEFITMERGINGTOOL_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : VertexIterativeFitMergingTool
 * @Author : Fudong He
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Merge the input vertices and output merged vertices.
 * 
 **********************************************************************************/

// Local
#include "IVertexMergingTool.h"
#include "IVertexFittingSvc.h"
#include "PromptUtils.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// xAOD
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

// ROOT
#include "TH1.h"


namespace Prompt
{
  //======================================================================================================
  // Temporary
  //
  struct TmpVtx
  {
    TmpVtx():vtx(0) {}

    xAOD::Vertex *vtx;
  };

  //======================================================================================================
  // Two track vertex
  //
  struct TwoTrackVtx
  {
    TwoTrackVtx():vertex(0), track_id0(0), track_id1(0), vertex_fit_prob(1000.0), sum_track_pt(0.0) {}

    xAOD::Vertex                              *vertex;
    const xAOD::TrackParticle                 *track_id0;
    const xAOD::TrackParticle                 *track_id1; 

    double                                     vertex_fit_prob;
    double                                     sum_track_pt;
  };

  //=============================================================================
  struct SortTwoTrackVtxBySumTrackPt
  {
    bool operator()(const TwoTrackVtx &lhs, const TwoTrackVtx &rhs) { return lhs.sum_track_pt > rhs.sum_track_pt; }
  };

  //=============================================================================
  struct SortTwoTrackVtxByFitProb
  {
    bool operator()(const TwoTrackVtx &lhs, const TwoTrackVtx &rhs) { return lhs.vertex_fit_prob > rhs.vertex_fit_prob; }
  };

  //=============================================================================
  struct SortTracksByPt
  {
    bool operator()(const xAOD::TrackParticle *lhs, const xAOD::TrackParticle *rhs) { return lhs->pt() > rhs->pt(); }
  };


  //=============================================================================
  struct SortTwoTrackVtxByDistToSeed
  {
    explicit SortTwoTrackVtxByDistToSeed(const xAOD::Vertex *seed_):seed(seed_) {}

    bool operator()(const TwoTrackVtx &lhs, const TwoTrackVtx &rhs)
    { 
      return Prompt::GetDistance(seed, lhs.vertex) < Prompt::GetDistance(seed, rhs.vertex);
    }

    const xAOD::Vertex *seed;
  };

  //======================================================================================================
  // Vertex Merging Tool
  //
  class VertexIterativeFitMergingTool: public AthAlgTool, virtual public IVertexMergingTool
  {
  public:

    VertexIterativeFitMergingTool(const std::string &name,
			          const std::string &type,
			          const IInterface  *parent);
    
    virtual ~VertexIterativeFitMergingTool() {}

    virtual StatusCode initialize(void);

    virtual MergeResult mergeInitVertices(const FittingInput &input,
					  const xAOD::TrackParticle *tracklep,
					  const std::vector<xAOD::Vertex *> &init_vtxs,
					  const std::vector<const xAOD::TrackParticle *> &selected_tracks);
 
  private: 

    bool mergeIteratively2TrackVtxs(const FittingInput &input,
				    MergeResult &result, 
				    const VtxType vtxType);

    xAOD::Vertex* fitSeedVertexCluster(const FittingInput &input,
				       xAOD::Vertex *vtx, 
				       const VtxType vtxType,
				       std::vector<TwoTrackVtx> &others);

    xAOD::Vertex* fitSeedPlusOtherVertex(const FittingInput &input,
					 const xAOD::Vertex *seed, 
					 const xAOD::Vertex *other_vtx,
					 const VtxType vtxType);

    bool passVertexSelection(const xAOD::Vertex *vtx) const;

    unsigned removeMerged2TrackVertexes(const xAOD::Vertex *merged_vtx, 
					std::vector<TwoTrackVtx> &vtxs);

    void plotVertexDistances(const std::vector<TwoTrackVtx> &others);

    std::vector<const xAOD::TrackParticle *> getTracksWithoutVertex(const std::vector<xAOD::Vertex *> &pass_vtxs, 
								    const std::vector<const xAOD::TrackParticle *> &selected_tracks);
    
    std::vector<xAOD::Vertex *> fit2TrackVertexes(const FittingInput &input,
						  std::vector<const xAOD::TrackParticle *> &selected_tracks,
						  const VtxType vtxType);
    
    StatusCode makeHist(TH1 *&h, const std::string &key, int nbin, double xmin, double xmax);

  private:

    //
    // Properties:
    //
    ServiceHandle<Prompt::IVertexFittingSvc>               m_vertexFitterSvc;
    ServiceHandle<ITHistSvc>                               m_histSvc;
    
    bool                                                   m_debug;
    bool                                                   m_runInDevelopmentMode;

    double                                                 m_minFitProb;
    double                                                 m_minCandOverSeedFitProbRatio;
    unsigned                                               m_maxExtraTracks;

    std::string                                            m_outputStream;

    //
    // Development histograms
    //
    TH1                                                   *h_nvtx_2trk_init;
    TH1                                                   *h_nvtx_2trk_pass;
    TH1                                                   *h_nvtx_2trk_unmerged;
    TH1                                                   *h_nvtx_merged;

    TH1                                                   *h_newVtxFit_chi2;
    TH1                                                   *h_newVtxFit_prob;

    TH1                                                   *h_newVtxFit_dist_toCurr;
    TH1                                                   *h_newVtxFit_dist_toSeed;
    TH1                                                   *h_newVtxFit_dist_toSeed_pass;
    TH1                                                   *h_newVtxFit_dist_toSeed_fail;

    TH1                                                   *h_newVtxFit_prob_candOverSeed;
    TH1                                                   *h_newVtxFit_prob_candOverSeed_pass;
    TH1                                                   *h_newVtxFit_prob_candOverSeed_fail;
    TH1                                                   *h_newVtxFit_prob_candOverSeed_3trk;
    TH1                                                   *h_newVtxFit_prob_candOverSeed_3trk_pass;

    TH1                                                   *h_Vtx2trkPair_dist;
    TH1                                                   *h_Vtx2trkPair_dist_zoom;
    TH1                                                   *h_Vtx2trkPair_sig1;
    TH1                                                   *h_Vtx2trkPair_sig2;

    TH1                                                   *h_selectedTrack_CountAll;
    TH1                                                   *h_selectedTrack_CountMatch2Vtx;
    TH1                                                   *h_selectedTrack_CountWithout2Vtx;

    TH1                                                   *h_vtxWithoutLepton2trk_NTrack;
    TH1                                                   *h_vtxWithoutLepton2trk_NPass;
    TH1                                                   *h_vtxWithoutLepton2trk_NPassUnmerged;
    TH1                                                   *h_vtxWithoutLepton2trk_NMerged;
  };
}

#endif 
