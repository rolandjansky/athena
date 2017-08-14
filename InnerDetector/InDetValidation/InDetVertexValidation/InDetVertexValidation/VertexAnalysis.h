/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VERTEXANALYSIS_H
#define VERTEXANALYSIS_H

// STL includes
#include <string>
#include <vector>

#include <TF1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TRandom.h>

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthPileupEvent.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthParticle.h"

#include "xAODTracking/TrackParticleContainerFwd.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

// Tools
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
//#include "MCTruthClassifier/MCTruthClassifierDefs.h"
//#include "MCTruthClassifier/IMCTruthClassifier.h"

class VertexAnalysis {
 public:
  VertexAnalysis();
  ~VertexAnalysis();

  std::vector< std::pair<TH1*,std::string> > bookHistograms(float m_MaxLayoutEta);

  int procHistograms();

  int execute(const xAOD::VertexContainer &vertexContainer,
        const xAOD::TruthVertexContainer &truthvertexContainer,
        const xAOD::EventInfo &eventInfo,
        double m_muCorrected,
        double m_muWeight,
        const xAOD::TrackParticleContainer &trackContainer,
        const xAOD::TruthEventContainer &truthEventContainer,
        const xAOD::TruthPileupEventContainer &truthPileupEventContainer,
        xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_loose,
        xAOD::TrackVertexAssociationMap trkvxassoUniqueMap_tight,
        bool Truthevents_available,
        bool TruthPUevents_available);

  int CalcEff(TH1D *Reco, TH1D *Truth, TH1D *Eff);

  int CalcResolution(TH2D *Bias, TH1D *Resolution);

  int CalcResolution_twoD(TH3D *Bias, TH2D *Resolution);

 private:
  // Event count.
  int m_entryNumber;
  TH1D *h_events = NULL;
  TH1D *h_run_number = NULL;
  TH1D *h_event_number = NULL;

  //internal pointers to all the histograms to use

  TH1D *h_all_track_pt = NULL;
  TH1D *h_all_track_eta = NULL;
  TH1D *h_all_track_d0 = NULL;
    TH2D *h_all_track_d0_eta = NULL;
  TH1D *h_all_track_err_d0 = NULL;
    TH2D *h_all_track_err_d0_eta = NULL;
  TH1D *h_all_track_d0_over_err_d0 = NULL;
    TH2D *h_all_track_d0_over_err_d0_eta = NULL;
  TH1D *h_all_track_z0 = NULL;
    TH2D *h_all_track_z0_eta = NULL;
  TH1D *h_all_track_err_z0 = NULL;
    TH2D *h_all_track_err_z0_eta = NULL;
  TH1D *h_all_track_z0_over_err_z0 = NULL;
    TH2D *h_all_track_z0_over_err_z0_eta = NULL;

  TH1D *h_mu_UnCorrected = NULL;
  TH1D *h_mu_Corrected = NULL; 
  TH1D *h_nVtx = NULL;
    TH2D *h_nVtx_Mu = NULL;
  TH1D *h_x = NULL; 
  TH1D *h_y = NULL; 
  TH1D *h_z = NULL; 
  TH1D *h_x_err = NULL; 
  TH1D *h_y_err = NULL; 
  TH1D *h_z_err = NULL;
  TH1D *h_lPUdz = NULL;
  TH1D *h_Dz = NULL;
    TH1D *h_highest_pt_Dz = NULL;
    TH2D *h_Dz_Mu = NULL;
  TH1D *h_type = NULL; 
  TH1D *h_Vtx_sumpt_squared = NULL; 
  TH1D *h_Vtx_lPUdz = NULL;
  TH1D *h_Vtx_ntracks = NULL; 
  TH1D *h_Vtx_zPosition = NULL;
  TH1D *h_chi2_over_ndf = NULL;
    TH2D *h_chi2_over_ndf_Mu = NULL; 
    TH2D *h_chi2_over_ndf_sumpt_squared = NULL;
    TH2D *h_chi2_over_ndf_lPUdz = NULL; 
    TH2D *h_chi2_over_ndf_ntracks = NULL; 
    TH2D *h_chi2_over_ndf_zPosition = NULL; 

  TH1D *h_ntracks = NULL;
    TH2D *h_ntracks_Mu = NULL; 
    TH2D *h_ntracks_sumpt_squared = NULL;
    TH2D *h_ntracks_lPUdz = NULL; 
    TH2D *h_ntracks_zPosition = NULL; 
  TH1D *h_track_weights = NULL;
  TH1D *h_track_pt = NULL; 
  TH1D *h_track_eta = NULL;
  TH1D *h_track_t = NULL;
  TH1D *h_track_t_with_cut = NULL;
  TH1D *h_track_nSiHits = NULL;
  TH1D *h_track_nSiHoles = NULL;
  TH1D *h_track_d0 = NULL;
  TH1D *h_track_err_d0 = NULL;
  TH1D *h_track_d0_over_err_d0 = NULL;
    TH2D *h_track_d0_over_err_d0_Mu = NULL;
  TH1D *h_track_z0 = NULL; 
  TH1D *h_track_err_z0 = NULL; 
    TH2D *h_track_err_z0_Mu = NULL;
    TH2D *h_track_err_z0_eta = NULL; 
  TH1D *h_track_z0_over_err_z0 = NULL;
  TH1D *h_track_z0_Vtx_z0 = NULL;
  TH1D *h_track_bias_d0 = NULL; 
  TH1D *h_track_bias_z0 = NULL;
  TH1D *h_track_bias_phi = NULL; 
  TH1D *h_track_pull_d0 = NULL; 
  TH1D *h_track_pull_z0 = NULL;
  TH1D *h_track_pull_phi = NULL;
  TH1D *h_eff_track_pt = NULL;
  TH1D *h_eff_track_eta = NULL;

  TH1D *h_nClass = NULL;
  TH1D *h_nReconstruct = NULL;
    TH2D *h_nReconstruct_mu = NULL;
  TH1D *h_Reconstruct_sumpt_squared = NULL; 
  TH1D *h_Reconstruct_lPUdz = NULL; 
  TH1D *h_Reconstruct_ntracks = NULL;
  TH1D *h_Reconstruct_zPosition = NULL;
  TH1D *h_nMatch = NULL;
    TH2D *h_nMatch_mu = NULL;
  TH1D *h_Match_sumpt_squared = NULL; 
  TH1D *h_Match_lPUdz = NULL; 
  TH1D *h_Match_ntracks = NULL;
  TH1D *h_Match_zPosition = NULL;
  TH1D *h_nMerge = NULL;
    TH2D *h_nMerge_mu = NULL; 
  TH1D *h_Merge_sumpt_squared = NULL; 
  TH1D *h_Merge_lPUdz = NULL;
  TH1D *h_Merge_ntracks = NULL; 
  TH1D *h_Merge_zPosition = NULL; 
  TH1D *h_nSplit = NULL; 
    TH2D *h_nSplit_mu = NULL; 
  TH1D *h_Split_sumpt_squared = NULL; 
  TH1D *h_Split_lPUdz = NULL; 
  TH1D *h_Split_ntracks = NULL; 
  TH1D *h_Split_zPosition = NULL; 
  TH1D *h_nFake = NULL; 
    TH2D *h_nFake_mu = NULL; 
  TH1D *h_Fake_sumpt_squared = NULL; 
  TH1D *h_Fake_lPUdz = NULL; 
  TH1D *h_Fake_ntracks = NULL; 
  TH1D *h_Fake_zPosition = NULL; 
  TH1D *h_DzSplits = NULL; 
  TH1D *h_HSCategory = NULL; 
    TH2D *h_HSCategory_Mu = NULL;

  TH1D *h_HS_nClass = NULL;
  TH1D *h_nHS_best = NULL;
    TH2D *h_nHS_best_mu = NULL;
  TH1D *h_HS_best_track_eta = NULL;
  TH1D *h_HS_best_track_pt = NULL;
  TH1D *h_HS_best_track_association_eta = NULL;
  TH1D *h_HS_best_truth_association_eta = NULL;
  TH1D *h_HS_best_sqrt_sumpt_squared = NULL;
  TH1D *h_HS_best_sumpt_squared = NULL; 
  TH1D *h_HS_best_lPUdz = NULL; 
  TH1D *h_HS_best_ntracks = NULL;
  TH1D *h_HS_best_zPosition = NULL;
  TH1D *h_HS_best_sumpt_squared_highest = NULL;
  TH1D *h_HS_best_sumpt_squared_highest_eta_bin_000_120 = NULL; 
  TH1D *h_HS_best_sumpt_squared_highest_sumpt_squared = NULL; 
  TH1D *h_HS_best_sumpt_squared_highest_lPUdz = NULL; 
  TH1D *h_HS_best_sumpt_squared_highest_ntracks = NULL;
  TH1D *h_HS_best_sumpt_squared_highest_zPosition = NULL;
  TH1D *h_HS_best_not_sumpt_squared_highest_sumpt_squared = NULL; 
  TH1D *h_HS_best_not_sumpt_squared_highest_lPUdz = NULL; 
  TH1D *h_HS_best_not_sumpt_squared_highest_ntracks = NULL;
  TH1D *h_HS_best_not_sumpt_squared_highest_zPosition = NULL;
  TH1D *h_nHS_best_Reconstruct = NULL;
    TH2D *h_nHS_best_Reconstruct_mu = NULL;
  TH1D *h_HS_best_Reconstruct_sumpt_squared = NULL; 
  TH1D *h_HS_best_Reconstruct_lPUdz = NULL; 
  TH1D *h_HS_best_Reconstruct_ntracks = NULL;
  TH1D *h_HS_best_Reconstruct_zPosition = NULL;
  TH1D *h_HS_best_nMatch = NULL;
    TH2D *h_HS_best_nMatch_mu = NULL;
  TH1D *h_HS_best_Match_sumpt_squared = NULL; 
  TH1D *h_HS_best_Match_lPUdz = NULL; 
  TH1D *h_HS_best_Match_ntracks = NULL;
  TH1D *h_HS_best_Match_zPosition = NULL;
  TH1D *h_HS_best_nMerge = NULL;
    TH2D *h_HS_best_nMerge_mu = NULL; 
  TH1D *h_HS_best_Merge_sumpt_squared = NULL; 
  TH1D *h_HS_best_Merge_lPUdz = NULL;
  TH1D *h_HS_best_Merge_ntracks = NULL; 
  TH1D *h_HS_best_Merge_zPosition = NULL; 
  TH1D *h_HS_best_nSplit = NULL; 
    TH2D *h_HS_best_nSplit_mu = NULL; 
  TH1D *h_HS_best_Split_sumpt_squared = NULL; 
  TH1D *h_HS_best_Split_lPUdz = NULL; 
  TH1D *h_HS_best_Split_ntracks = NULL; 
  TH1D *h_HS_best_Split_zPosition = NULL; 
  TH1D *h_HS_best_nFake = NULL; 
    TH2D *h_HS_best_nFake_mu = NULL; 
  TH1D *h_HS_best_Fake_sumpt_squared = NULL; 
  TH1D *h_HS_best_Fake_lPUdz = NULL; 
  TH1D *h_HS_best_Fake_ntracks = NULL; 
  TH1D *h_HS_best_Fake_zPosition = NULL;

  TH1D *h_PU_nClass = NULL;
  TH1D *h_nPU_Reconstruct = NULL;
    TH2D *h_nPU_Reconstruct_mu = NULL;
  TH1D *h_PU_Reconstruct_track_eta = NULL;
  TH1D *h_PU_Reconstruct_track_pt = NULL; 
  TH1D *h_PU_Reconstruct_sqrt_sumpt_squared = NULL; 
  TH1D *h_PU_Reconstruct_sumpt_squared = NULL; 
  TH1D *h_PU_Reconstruct_lPUdz = NULL;
  TH1D *h_PU_Reconstruct_ntracks = NULL;
  TH1D *h_PU_Reconstruct_zPosition = NULL;
  TH1D *h_PU_nMatch = NULL;
    TH2D *h_PU_nMatch_mu = NULL;
  TH1D *h_PU_Match_sumpt_squared = NULL; 
  TH1D *h_PU_Match_lPUdz = NULL;
  TH1D *h_PU_Match_ntracks = NULL;
  TH1D *h_PU_Match_zPosition = NULL;
  TH1D *h_PU_nMerge = NULL;
    TH2D *h_PU_nMerge_mu = NULL; 
  TH1D *h_PU_Merge_sumpt_squared = NULL; 
  TH1D *h_PU_Merge_lPUdz = NULL;
  TH1D *h_PU_Merge_ntracks = NULL; 
  TH1D *h_PU_Merge_zPosition = NULL; 
  TH1D *h_PU_nSplit = NULL; 
    TH2D *h_PU_nSplit_mu = NULL; 
  TH1D *h_PU_Split_sumpt_squared = NULL; 
  TH1D *h_PU_Split_lPUdz = NULL; 
  TH1D *h_PU_Split_ntracks = NULL; 
  TH1D *h_PU_Split_zPosition = NULL; 
  TH1D *h_PU_nFake = NULL; 
    TH2D *h_PU_nFake_mu = NULL; 
  TH1D *h_PU_Fake_sumpt_squared = NULL; 
  TH1D *h_PU_Fake_lPUdz = NULL; 
  TH1D *h_PU_Fake_ntracks = NULL; 
  TH1D *h_PU_Fake_zPosition = NULL;

  TH1D *h_truth_nVtx = NULL; 
    TH2D *h_truth_nVtx_Mu = NULL;
  TH1D *h_truth_Vtx_sumpt_squared = NULL; 
  TH1D *h_truth_Vtx_lPUdz = NULL; 
  TH1D *h_truth_Vtx_ntracks = NULL; 
  TH1D *h_truth_Vtx_zPosition = NULL;

  TH1D *h_truth_HS_nVtx = NULL; 
    TH2D *h_truth_HS_nVtx_Mu = NULL; 
  TH1D *h_truth_HS_Vtx_sumpt_squared = NULL;
  TH1D *h_truth_HS_nVtx_accepted_lower_coverage = NULL; 
  TH1D *h_truth_HS_nVtx_accepted = NULL; 
  TH1D *h_truth_HS_Vtx_lPUdz = NULL; 
  TH1D *h_truth_HS_Vtx_ntracks = NULL; 
  TH1D *h_truth_HS_Vtx_zPosition = NULL;

  TH1D *h_truth_PU_nVtx = NULL; 
    TH2D *h_truth_PU_nVtx_Mu = NULL; 
  TH1D *h_truth_PU_Vtx_sumpt_squared = NULL;
  TH1D *h_truth_PU_nVtx_accepted_lower_coverage = NULL; 
  TH1D *h_truth_PU_nVtx_accepted = NULL; 
  TH1D *h_truth_PU_Vtx_lPUdz = NULL;
  TH1D *h_truth_PU_Vtx_ntracks = NULL; 
  TH1D *h_truth_PU_Vtx_zPosition = NULL; 

  TH1D *h_truth_PU_track_pt = NULL;

  TH1D *h_eff_Vtx_sumpt_squared = NULL; 
  TH1D *h_eff_Vtx_lPUdz = NULL; 
  TH1D *h_eff_Vtx_ntracks = NULL; 
  TH1D *h_eff_Vtx_zPosition = NULL; 
  TH1D *h_eff_Reconstruct_sumpt_squared = NULL; 
  TH1D *h_eff_Reconstruct_lPUdz = NULL; 
  TH1D *h_eff_Reconstruct_ntracks = NULL; 
  TH1D *h_eff_Reconstruct_zPosition = NULL;
  TH1D *h_bias_Vtx_x = NULL; 
    TH2D *h_bias_Vtx_x_mu = NULL; 
    TH2D *h_bias_Vtx_x_sumpt_squared = NULL; 
    TH2D *h_bias_Vtx_x_lPUdz = NULL; 
    TH2D *h_bias_Vtx_x_ntracks = NULL; 
    TH2D *h_bias_Vtx_x_zPosition = NULL;
  TH1D *h_bias_Vtx_y = NULL; 
    TH2D *h_bias_Vtx_y_mu = NULL; 
    TH2D *h_bias_Vtx_y_sumpt_squared = NULL; 
    TH2D *h_bias_Vtx_y_lPUdz = NULL; 
    TH2D *h_bias_Vtx_y_ntracks = NULL; 
    TH2D *h_bias_Vtx_y_zPosition = NULL;
  TH1D *h_bias_Vtx_r = NULL; 
    TH2D *h_bias_Vtx_r_mu = NULL; 
    TH2D *h_bias_Vtx_r_sumpt_squared = NULL; 
    TH2D *h_bias_Vtx_r_lPUdz = NULL; 
    TH2D *h_bias_Vtx_r_ntracks = NULL; 
    TH2D *h_bias_Vtx_r_zPosition = NULL;
  TH1D *h_bias_Vtx_z = NULL; 
    TH2D *h_bias_Vtx_z_mu = NULL; 
    TH2D *h_bias_Vtx_z_sumpt_squared = NULL; 
    TH2D *h_bias_Vtx_z_lPUdz = NULL; 
    TH2D *h_bias_Vtx_z_ntracks = NULL; 
    TH2D *h_bias_Vtx_z_zPosition = NULL;
  TH1D *h_bias_Reconstruct_x = NULL; 
    TH2D *h_bias_Reconstruct_x_mu = NULL; 
    TH2D *h_bias_Reconstruct_x_sumpt_squared = NULL; 
    TH2D *h_bias_Reconstruct_x_lPUdz = NULL; 
    TH2D *h_bias_Reconstruct_x_ntracks = NULL; 
    TH2D *h_bias_Reconstruct_x_zPosition = NULL;
  TH1D *h_bias_Reconstruct_y = NULL; 
    TH2D *h_bias_Reconstruct_y_mu = NULL; 
    TH2D *h_bias_Reconstruct_y_sumpt_squared = NULL; 
    TH2D *h_bias_Reconstruct_y_lPUdz = NULL; 
    TH2D *h_bias_Reconstruct_y_ntracks = NULL; 
    TH2D *h_bias_Reconstruct_y_zPosition = NULL;
  TH1D *h_bias_Reconstruct_z = NULL; 
    TH2D *h_bias_Reconstruct_z_mu = NULL; 
    TH2D *h_bias_Reconstruct_z_sumpt_squared = NULL; 
    TH2D *h_bias_Reconstruct_z_lPUdz = NULL; 
    TH2D *h_bias_Reconstruct_z_ntracks = NULL; 
    TH2D *h_bias_Reconstruct_z_zPosition = NULL; 
  TH1D *h_res_Vtx_x = NULL; 
    TH1D *h_res_Vtx_x_mu = NULL; 
    TH1D *h_res_Vtx_x_sumpt_squared = NULL; 
    TH1D *h_res_Vtx_x_lPUdz = NULL; 
    TH1D *h_res_Vtx_x_ntracks = NULL; 
    TH1D *h_res_Vtx_x_zPosition = NULL; 
  TH1D *h_res_Vtx_y = NULL; 
    TH1D *h_res_Vtx_y_mu = NULL;
    TH1D *h_res_Vtx_y_sumpt_squared = NULL; 
    TH1D *h_res_Vtx_y_lPUdz = NULL; 
    TH1D *h_res_Vtx_y_ntracks = NULL; 
    TH1D *h_res_Vtx_y_zPosition = NULL;
  TH1D *h_res_Vtx_r = NULL; 
    TH1D *h_res_Vtx_r_mu = NULL;
    TH1D *h_res_Vtx_r_sumpt_squared = NULL; 
    TH1D *h_res_Vtx_r_lPUdz = NULL; 
    TH1D *h_res_Vtx_r_ntracks = NULL; 
    TH1D *h_res_Vtx_r_zPosition = NULL; 
  TH1D *h_res_Vtx_z = NULL; 
    TH1D *h_res_Vtx_z_mu = NULL; 
    TH1D *h_res_Vtx_z_sumpt_squared = NULL; 
    TH1D *h_res_Vtx_z_lPUdz = NULL;
    TH1D *h_res_Vtx_z_ntracks = NULL; 
    TH1D *h_res_Vtx_z_zPosition = NULL;
  TH1D *h_res_Reconstruct_x = NULL; 
    TH1D *h_res_Reconstruct_x_mu = NULL; 
    TH1D *h_res_Reconstruct_x_sumpt_squared = NULL; 
    TH1D *h_res_Reconstruct_x_lPUdz = NULL; 
    TH1D *h_res_Reconstruct_x_ntracks = NULL; 
    TH1D *h_res_Reconstruct_x_zPosition = NULL; 
  TH1D *h_res_Reconstruct_y = NULL; 
    TH1D *h_res_Reconstruct_y_mu = NULL;
    TH1D *h_res_Reconstruct_y_sumpt_squared = NULL; 
    TH1D *h_res_Reconstruct_y_lPUdz = NULL; 
    TH1D *h_res_Reconstruct_y_ntracks = NULL; 
    TH1D *h_res_Reconstruct_y_zPosition = NULL; 
  TH1D *h_res_Reconstruct_z = NULL; 
    TH1D *h_res_Reconstruct_z_mu = NULL; 
    TH1D *h_res_Reconstruct_z_sumpt_squared = NULL; 
    TH1D *h_res_Reconstruct_z_lPUdz = NULL;
    TH1D *h_res_Reconstruct_z_ntracks = NULL; 
    TH1D *h_res_Reconstruct_z_zPosition = NULL;

  TH1D *h_hsMatch_best_closest_z_diffference_x = NULL;
  TH1D *h_hsMatch_best_closest_z_diffference_y = NULL;
  TH1D *h_hsMatch_best_closest_z_diffference_z = NULL;
  TH1D *h_eff_HS_best_Reconstruct_sumpt_squared = NULL; 
  TH1D *h_eff_HS_best_Reconstruct_lPUdz = NULL; 
  TH1D *h_eff_HS_best_Reconstruct_ntracks = NULL; 
  TH1D *h_eff_HS_best_Reconstruct_zPosition = NULL;
  TH1D *h_eff_HS_best_is_sumpt_squared_highest = NULL;
  TH1D *h_eff_HS_best_is_sumpt_squared_highest_eta_bin_000_120 = NULL; 
  TH1D *h_eff_HS_best_is_sumpt_squared_highest_sumpt_squared = NULL; 
  TH1D *h_eff_HS_best_is_sumpt_squared_highest_lPUdz = NULL; 
  TH1D *h_eff_HS_best_is_sumpt_squared_highest_ntracks = NULL; 
  TH1D *h_eff_HS_best_is_sumpt_squared_highest_zPosition = NULL;
  TH1D *h_impurity_HS_best_is_sumpt_squared_highest_sumpt_squared = NULL; 
  TH1D *h_impurity_HS_best_is_sumpt_squared_highest_lPUdz = NULL; 
  TH1D *h_impurity_HS_best_is_sumpt_squared_highest_ntracks = NULL; 
  TH1D *h_impurity_HS_best_is_sumpt_squared_highest_zPosition = NULL;

  TH1D *h_bias_HS_best_Reconstruct_x = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_x_mu = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_x_sumpt_squared = NULL;
    TH3D *h_bias_HS_best_Reconstruct_x_sumpt_squared_ntracks = NULL;
    TH2D *h_bias_HS_best_Reconstruct_x_lPUdz = NULL;
    TH2D *h_bias_HS_best_Reconstruct_x_ntracks = NULL;
    TH2D *h_bias_HS_best_Reconstruct_x_zPosition = NULL;
  TH1D *h_bias_HS_best_Reconstruct_y = NULL;
    TH2D *h_bias_HS_best_Reconstruct_y_mu = NULL;
    TH2D *h_bias_HS_best_Reconstruct_y_sumpt_squared = NULL;
    TH3D *h_bias_HS_best_Reconstruct_y_sumpt_squared_ntracks = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_y_lPUdz = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_y_ntracks = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_y_zPosition = NULL;
  TH1D *h_bias_HS_best_Reconstruct_r = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_r_mu = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_r_sumpt_squared = NULL;
    TH3D *h_bias_HS_best_Reconstruct_r_sumpt_squared_ntracks = NULL;  
    TH2D *h_bias_HS_best_Reconstruct_r_lPUdz = NULL;
    TH2D *h_bias_HS_best_Reconstruct_r_ntracks = NULL;
    TH2D *h_bias_HS_best_Reconstruct_r_zPosition = NULL;
  TH1D *h_bias_HS_best_Reconstruct_z = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_z_mu = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_z_sumpt_squared = NULL;
    TH3D *h_bias_HS_best_Reconstruct_z_sumpt_squared_ntracks = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_z_lPUdz = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_z_ntracks = NULL; 
    TH2D *h_bias_HS_best_Reconstruct_z_zPosition = NULL;
  TH1D *h_res_HS_best_Reconstruct_x = NULL; 
    TH1D *h_res_HS_best_Reconstruct_x_mu = NULL; 
    TH1D *h_res_HS_best_Reconstruct_x_sumpt_squared = NULL;
    TH2D *h_res_HS_best_Reconstruct_x_sumpt_squared_ntracks = NULL;
    TH1D *h_res_HS_best_Reconstruct_x_lPUdz = NULL; 
    TH1D *h_res_HS_best_Reconstruct_x_ntracks = NULL; 
    TH1D *h_res_HS_best_Reconstruct_x_zPosition = NULL;
  TH1D *h_res_HS_best_Reconstruct_y = NULL; 
    TH1D *h_res_HS_best_Reconstruct_y_mu = NULL;
    TH1D *h_res_HS_best_Reconstruct_y_sumpt_squared = NULL;
    TH2D *h_res_HS_best_Reconstruct_y_sumpt_squared_ntracks = NULL;
    TH1D *h_res_HS_best_Reconstruct_y_lPUdz = NULL; 
    TH1D *h_res_HS_best_Reconstruct_y_ntracks = NULL; 
    TH1D *h_res_HS_best_Reconstruct_y_zPosition = NULL;
  TH1D *h_res_HS_best_Reconstruct_r = NULL; 
    TH1D *h_res_HS_best_Reconstruct_r_mu = NULL;
    TH1D *h_res_HS_best_Reconstruct_r_sumpt_squared = NULL;
    TH2D *h_res_HS_best_Reconstruct_r_sumpt_squared_ntracks = NULL;
    TH1D *h_res_HS_best_Reconstruct_r_lPUdz = NULL; 
    TH1D *h_res_HS_best_Reconstruct_r_ntracks = NULL; 
    TH1D *h_res_HS_best_Reconstruct_r_zPosition = NULL;
  TH1D *h_res_HS_best_Reconstruct_z = NULL; 
    TH1D *h_res_HS_best_Reconstruct_z_mu = NULL; 
    TH1D *h_res_HS_best_Reconstruct_z_sumpt_squared = NULL;
    TH2D *h_res_HS_best_Reconstruct_z_sumpt_squared_ntracks = NULL;
    TH1D *h_res_HS_best_Reconstruct_z_lPUdz = NULL;
    TH1D *h_res_HS_best_Reconstruct_z_ntracks = NULL; 
    TH1D *h_res_HS_best_Reconstruct_z_zPosition = NULL;

  TH1D *h_eff_HS_truth_ntracks_association = NULL;
  TH2D *h_truth_associated_hs_ntracks_lPUdz = NULL;
  TH2D *h_true_hs_ntracks_lPUdz = NULL;
    TH2D *h_eff_HS_truth_ntracks_association_sqrt_sumpt_squared = NULL;
    TH2D *h_eff_HS_truth_ntracks_association_sumpt_squared = NULL;
    TH2D *h_eff_HS_truth_ntracks_association_lPUdz = NULL;
    TH2D *h_eff_HS_truth_ntracks_association_ntracks = NULL;
    TH2D *h_eff_HS_truth_ntracks_association_zPosition = NULL;
  TH1D *h_eff_HS_truth_association = NULL;
    TH2D *h_eff_HS_truth_association_mu = NULL; 
    TH2D *h_eff_HS_truth_association_sqrt_sumpt_squared = NULL; 
    TH2D *h_eff_HS_truth_association_sumpt_squared = NULL; 
    TH2D *h_eff_HS_truth_association_lPUdz = NULL;
    TH2D *h_eff_HS_truth_association_ntracks = NULL; 
    TH2D *h_eff_HS_truth_association_zPosition = NULL;
    TH2D *h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120 = NULL; 
    TH2D *h_eff_HS_truth_association_sumpt_squared_eta_bin_000_120 = NULL; 
    TH2D *h_eff_HS_truth_association_lPUdz_eta_bin_000_120 = NULL;
    TH2D *h_eff_HS_truth_association_ntracks_eta_bin_000_120 = NULL; 
    TH2D *h_eff_HS_truth_association_zPosition_eta_bin_000_120 = NULL;
    TH2D *h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270 = NULL; 
    TH2D *h_eff_HS_truth_association_sumpt_squared_eta_bin_120_270 = NULL; 
    TH2D *h_eff_HS_truth_association_lPUdz_eta_bin_120_270 = NULL;
    TH2D *h_eff_HS_truth_association_ntracks_eta_bin_120_270 = NULL; 
    TH2D *h_eff_HS_truth_association_zPosition_eta_bin_120_270 = NULL;
    TH2D *h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340 = NULL; 
    TH2D *h_eff_HS_truth_association_sumpt_squared_eta_bin_270_340 = NULL; 
    TH2D *h_eff_HS_truth_association_lPUdz_eta_bin_270_340 = NULL;
    TH2D *h_eff_HS_truth_association_ntracks_eta_bin_270_340 = NULL; 
    TH2D *h_eff_HS_truth_association_zPosition_eta_bin_270_340 = NULL;
    TH2D *h_eff_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400 = NULL; 
    TH2D *h_eff_HS_truth_association_sumpt_squared_eta_bin_340_400 = NULL; 
    TH2D *h_eff_HS_truth_association_lPUdz_eta_bin_340_400 = NULL;
    TH2D *h_eff_HS_truth_association_ntracks_eta_bin_340_400 = NULL; 
    TH2D *h_eff_HS_truth_association_zPosition_eta_bin_340_400 = NULL;
    TH2D *h_eff_HS_truth_association_sqrt_sumpt_squared_is_highest = NULL; 
    TH2D *h_eff_HS_truth_association_sumpt_squared_is_highest = NULL; 
    TH2D *h_eff_HS_truth_association_lPUdz_is_highest = NULL;
    TH2D *h_eff_HS_truth_association_ntracks_is_highest = NULL;
    TH2D *h_eff_HS_truth_association_zPosition_is_highest = NULL;

  TH1D *h_impurity_HS_truth_ntrack_association = NULL;
  TH2D *h_truth_associated_pu_ntracks_lPUdz = NULL;
  TH2D *h_reconstructed_hs_ntracks_lPUdz = NULL;
    TH2D *h_impurity_HS_truth_ntrack_association_sqrt_sumpt_squared = NULL;
    TH2D *h_impurity_HS_truth_ntrack_association_sumpt_squared = NULL;
    TH2D *h_impurity_HS_truth_ntrack_association_lPUdz = NULL;
    TH2D *h_impurity_HS_truth_ntrack_association_ntracks = NULL;
    TH2D *h_impurity_HS_truth_ntrack_association_zPosition = NULL;
  TH1D *h_impurity_HS_truth_association = NULL;
    TH2D *h_impurity_HS_truth_association_mu = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_000_120 = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared_eta_bin_000_120 = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz_eta_bin_000_120 = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks_eta_bin_000_120 = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition_eta_bin_000_120 = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_120_270 = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared_eta_bin_120_270 = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz_eta_bin_120_270 = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks_eta_bin_120_270 = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition_eta_bin_120_270 = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_270_340 = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared_eta_bin_270_340 = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz_eta_bin_270_340 = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks_eta_bin_270_340 = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition_eta_bin_270_340 = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared_eta_bin_340_400 = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared_eta_bin_340_400 = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz_eta_bin_340_400 = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks_eta_bin_340_400 = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition_eta_bin_340_400 = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared_high_weight_tracks = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared_high_weight_tracks = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz_high_weight_tracks = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks_high_weight_tracks = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition_high_weight_tracks = NULL;
    TH2D *h_impurity_HS_truth_association_sqrt_sumpt_squared_is_highest = NULL; 
    TH2D *h_impurity_HS_truth_association_sumpt_squared_is_highest = NULL; 
    TH2D *h_impurity_HS_truth_association_lPUdz_is_highest = NULL;
    TH2D *h_impurity_HS_truth_association_ntracks_is_highest = NULL; 
    TH2D *h_impurity_HS_truth_association_zPosition_is_highest = NULL;

  TH1D *h_eff_HS_track_association_eta = NULL;
  TH1D *h_impurity_HS_track_association_eta = NULL;

  /*TH1D *h_reconstructed_hs_eta = NULL;
  TH1D *h_associated_pu_track_eta = NULL;
    TH1D *h_impurity_HS_truth_association_eta = NULL;
  TH1D *h_reconstructed_pu_eta = NULL;
  TH1D *h_associated_hs_track_eta = NULL;
    TH1D *h_impurity_PU_association_eta = NULL;*/


  TH1D *h_eff_PU_Reconstruct_sumpt_squared = NULL;
  TH1D *h_eff_PU_Reconstruct_lPUdz = NULL;
  TH1D *h_eff_PU_Reconstruct_ntracks = NULL; 
  TH1D *h_eff_PU_Reconstruct_zPosition = NULL; 
  TH1D *h_bias_PU_Reconstruct_x = NULL; 
    TH2D *h_bias_PU_Reconstruct_x_mu = NULL; 
    TH2D *h_bias_PU_Reconstruct_x_sumpt_squared = NULL;
    TH3D *h_bias_PU_Reconstruct_x_sumpt_squared_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_x_lPUdz = NULL;
    TH2D *h_bias_PU_Reconstruct_x_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_x_zPosition = NULL;
  TH1D *h_bias_PU_Reconstruct_y = NULL; 
    TH2D *h_bias_PU_Reconstruct_y_mu = NULL; 
    TH2D *h_bias_PU_Reconstruct_y_sumpt_squared = NULL;
    TH3D *h_bias_PU_Reconstruct_y_sumpt_squared_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_y_lPUdz = NULL;
    TH2D *h_bias_PU_Reconstruct_y_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_y_zPosition = NULL;
  TH1D *h_bias_PU_Reconstruct_r = NULL; 
    TH2D *h_bias_PU_Reconstruct_r_mu = NULL; 
    TH2D *h_bias_PU_Reconstruct_r_sumpt_squared = NULL;
    TH3D *h_bias_PU_Reconstruct_r_sumpt_squared_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_r_lPUdz = NULL;
    TH2D *h_bias_PU_Reconstruct_r_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_r_zPosition = NULL;
  TH1D *h_bias_PU_Reconstruct_z = NULL; 
    TH2D *h_bias_PU_Reconstruct_z_mu = NULL; 
    TH2D *h_bias_PU_Reconstruct_z_sumpt_squared = NULL;
    TH3D *h_bias_PU_Reconstruct_z_sumpt_squared_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_z_lPUdz = NULL;
    TH2D *h_bias_PU_Reconstruct_z_ntracks = NULL; 
    TH2D *h_bias_PU_Reconstruct_z_zPosition = NULL;
  TH1D *h_res_PU_Reconstruct_x = NULL; 
    TH1D *h_res_PU_Reconstruct_x_mu = NULL; 
    TH1D *h_res_PU_Reconstruct_x_sumpt_squared = NULL;
    TH2D *h_res_PU_Reconstruct_x_sumpt_squared_ntracks = NULL;
    TH1D *h_res_PU_Reconstruct_x_lPUdz = NULL;
    TH1D *h_res_PU_Reconstruct_x_ntracks = NULL; 
    TH1D *h_res_PU_Reconstruct_x_zPosition = NULL; 
  TH1D *h_res_PU_Reconstruct_y = NULL; 
    TH1D *h_res_PU_Reconstruct_y_mu = NULL;
    TH1D *h_res_PU_Reconstruct_y_sumpt_squared = NULL;
    TH2D *h_res_PU_Reconstruct_y_sumpt_squared_ntracks = NULL;
    TH1D *h_res_PU_Reconstruct_y_lPUdz = NULL;
    TH1D *h_res_PU_Reconstruct_y_ntracks = NULL; 
    TH1D *h_res_PU_Reconstruct_y_zPosition = NULL;
  TH1D *h_res_PU_Reconstruct_r = NULL; 
    TH1D *h_res_PU_Reconstruct_r_mu = NULL;
    TH1D *h_res_PU_Reconstruct_r_sumpt_squared = NULL;
    TH2D *h_res_PU_Reconstruct_r_sumpt_squared_ntracks = NULL;
    TH1D *h_res_PU_Reconstruct_r_lPUdz = NULL;
    TH1D *h_res_PU_Reconstruct_r_ntracks = NULL;
    TH1D *h_res_PU_Reconstruct_r_zPosition = NULL;
  TH1D *h_res_PU_Reconstruct_z = NULL; 
    TH1D *h_res_PU_Reconstruct_z_mu = NULL; 
    TH1D *h_res_PU_Reconstruct_z_sumpt_squared = NULL;
    TH2D *h_res_PU_Reconstruct_z_sumpt_squared_ntracks = NULL;
    TH1D *h_res_PU_Reconstruct_z_lPUdz = NULL;
    TH1D *h_res_PU_Reconstruct_z_ntracks = NULL; 
    TH1D *h_res_PU_Reconstruct_z_zPosition = NULL;
};

#endif
