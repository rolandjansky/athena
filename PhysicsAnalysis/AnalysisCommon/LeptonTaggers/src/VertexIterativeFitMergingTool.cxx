/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "VertexIterativeFitMergingTool.h"
#include "VarHolder.h"
#include "PromptUtils.h"

using namespace std;

//=============================================================================
Prompt::VertexIterativeFitMergingTool::VertexIterativeFitMergingTool(const std::string &name,
								     const std::string &type,
								     const IInterface  *parent):
  AthAlgTool       (name, type, parent),
  m_vertexFitterSvc("Prompt::VertexFittingSvc/PromptVertexFittingSvc", name),
  m_histSvc        ("THistSvc/THistSvc", name)
{  
  declareInterface<Prompt::IVertexMergingTool>(this); 
  
  declareProperty("debug",                       m_debug                       = false);
  declareProperty("runInDevelopmentMode",        m_runInDevelopmentMode        = true);

  declareProperty("minFitProb",                  m_minFitProb                  = 0.01, "minimum fit probability requirement for a vertex");
  declareProperty("minCandOverSeedFitProbRatio", m_minCandOverSeedFitProbRatio = 0.2,  "minimum requirement of the fit probability of new merged vertex / fit probability of seed vertex");
  declareProperty("maxExtraTracks",              m_maxExtraTracks              = 10,   "maximum number of tracks without good lepton+track vertex that we will used for further fitting of vertexes without lepton");
  declareProperty("outputStream",                m_outputStream);

  declareProperty("VertexFittingSvc",            m_vertexFitterSvc);
}

//=============================================================================
StatusCode Prompt::VertexIterativeFitMergingTool::initialize()
{
  ATH_CHECK(m_vertexFitterSvc.retrieve());
  ATH_CHECK(m_histSvc        .retrieve());

  ATH_CHECK(makeHist(h_nvtx_2trk_init,     "nvtx_2trk_init",                   25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_nvtx_2trk_pass,     "nvtx_2trk_pass",                   25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_nvtx_2trk_unmerged, "nvtx_2trk_unmerged",               25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_nvtx_merged,        "nvtx_merged",                      25, -0.5, 24.5));

  ATH_CHECK(makeHist(h_newVtxFit_chi2,             "newVtxFit_chi2",                   100, 0.0, 50.0));
  ATH_CHECK(makeHist(h_newVtxFit_prob,             "newVtxFit_prob",                   100, 0.0,  1.0));

  ATH_CHECK(makeHist(h_newVtxFit_dist_toCurr,      "newVtxFit_dist_toCurr",            100, 0.0, 10.0));
  ATH_CHECK(makeHist(h_newVtxFit_dist_toSeed,      "newVtxFit_dist_toSeed",            100, 0.0, 10.0));
  ATH_CHECK(makeHist(h_newVtxFit_dist_toSeed_pass, "newVtxFit_dist_toSeed_pass",       100, 0.0, 10.0));
  ATH_CHECK(makeHist(h_newVtxFit_dist_toSeed_fail, "newVtxFit_dist_toSeed_fail",       100, 0.0, 10.0));

  ATH_CHECK(makeHist(h_newVtxFit_prob_candOverSeed,           "newVtxFit_prob_candOverSeed",           100, 0.0,  4.0));
  ATH_CHECK(makeHist(h_newVtxFit_prob_candOverSeed_pass,      "newVtxFit_prob_candOverSeed_pass",      100, 0.0,  4.0));
  ATH_CHECK(makeHist(h_newVtxFit_prob_candOverSeed_fail,      "newVtxFit_prob_candOverSeed_fail",      100, 0.0,  4.0));
  ATH_CHECK(makeHist(h_newVtxFit_prob_candOverSeed_3trk,      "newVtxFit_prob_candOverSeed_3trk",      100, 0.0,  4.0));
  ATH_CHECK(makeHist(h_newVtxFit_prob_candOverSeed_3trk_pass, "newVtxFit_prob_candOverSeed_3trk_pass", 100, 0.0,  4.0));

  ATH_CHECK(makeHist(h_Vtx2trkPair_dist,      "Vtx2trkPair_dist",      100, 0.0, 100.0));
  ATH_CHECK(makeHist(h_Vtx2trkPair_dist_zoom, "Vtx2trkPair_dist_zoom", 100, 0.0,  10.0));
  ATH_CHECK(makeHist(h_Vtx2trkPair_sig1,      "Vtx2trkPair_sig1",      100, 0.0,  20.0));
  ATH_CHECK(makeHist(h_Vtx2trkPair_sig2,      "Vtx2trkPair_sig2",      100, 0.0,  20.0));

  ATH_CHECK(makeHist(h_selectedTrack_CountAll,         "selectedTrack_CountAll",         25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_selectedTrack_CountMatch2Vtx ,  "selectedTrack_CountMatch2Vtx",   25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_selectedTrack_CountWithout2Vtx, "selectedTrack_CountWithout2Vtx", 25, -0.5, 24.5));

  ATH_CHECK(makeHist(h_vtxWithoutLepton2trk_NTrack,        "vtxWithoutLepton2trk_NTrack",         25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_vtxWithoutLepton2trk_NPass,         "vtxWithoutLepton2trk_NPass",          25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_vtxWithoutLepton2trk_NPassUnmerged, "vtxWithoutLepton2trk_NPassUnmerged",  25, -0.5, 24.5));
  ATH_CHECK(makeHist(h_vtxWithoutLepton2trk_NMerged,       "vtxWithoutLepton2trk_NMerged",        25, -0.5, 24.5));

  return StatusCode::SUCCESS;
}

//=============================================================================
Prompt::MergeResult Prompt::VertexIterativeFitMergingTool::mergeInitVertices(const FittingInput &input,
									     const xAOD::TrackParticle *tracklep, 
									     const std::vector<xAOD::Vertex *> &init_vtxs,
									     const std::vector<const xAOD::TrackParticle *> &selected_tracks)
{
  //
  // Merge initial (2-track) vertices into new merged vertices with three tracks or more
  //
  
  ATH_MSG_DEBUG("===========================================================================" << endl
             << name() << "::mergeInitVertices - start processing");
  

  MergeResult result;

  for(xAOD::Vertex *vtx: init_vtxs) {
    if(passVertexSelection(vtx)) {
      result.vtxs_init_passed.push_back(vtx);
    }
  }

  const unsigned nvtx_init = init_vtxs.size();
  const unsigned nvtx_pass = result.vtxs_init_passed.size();

  FillTH1(h_nvtx_2trk_init, nvtx_init);  

  //---------------------------------------------------------------------------------------------
  // Find tracks that do not form 2-track vertex with lepton track
  //
  
  ATH_MSG_DEBUG(name() << "::mergeInitVertices - processes vertexes without lepton");
  

  std::vector<const xAOD::TrackParticle *> tracks_without_vertex = getTracksWithoutVertex(result.vtxs_init_passed, selected_tracks);

  //
  // Rank the tracks by pT, will keep the top `m_maxExtraTracks` tracks for fitting vertexes without lepton 
  //
  if(tracks_without_vertex.size() > m_maxExtraTracks) {
    ATH_MSG_DEBUG(" number of tracks without good lepton+track vertex: " << tracks_without_vertex.size() << endl
        << " will only keep the top " << m_maxExtraTracks << " tracks"); 

    std::sort(tracks_without_vertex.begin(), tracks_without_vertex.end(), Prompt::SortByIDTrackPt());

    tracks_without_vertex.erase(tracks_without_vertex.begin() + m_maxExtraTracks, tracks_without_vertex.end());
  }


  MergeResult result_extra;
  result_extra.vtxs_init_passed = fit2TrackVertexes(input, tracks_without_vertex, Prompt::kTwoTrackVtxWithoutLepton);


  ATH_MSG_DEBUG(name() << "::mergeInitVertices - will merge vertexes without lepton"        << endl
      << "   number of selected tracks without good lepton+track vertex: " << tracks_without_vertex        .size() << endl
      << "   number of 2-track vertexes without lepton:                  " << result_extra.vtxs_init_passed.size());



  //
  // Merge 2-track vertex without lepton only when there are two or more vertices
  //  
  mergeIteratively2TrackVtxs(input, result_extra, Prompt::kIterativeFitVtxWithoutLepton);  

  if(result_extra.vtxs_init_passed.size() > 1) {
    //
    // Fill histograms only when there are two or more vertices
    //
    FillTH1(h_vtxWithoutLepton2trk_NTrack,        tracks_without_vertex.size());
    FillTH1(h_vtxWithoutLepton2trk_NPass,         result_extra.vtxs_init_passed           .size());
    FillTH1(h_vtxWithoutLepton2trk_NPassUnmerged, result_extra.vtxs_init_passed_not_merged.size());
    FillTH1(h_vtxWithoutLepton2trk_NMerged,       result_extra.vtxs_new_merged            .size());
  }


  ATH_MSG_DEBUG(name() << "::mergeInitVertices - finished merging vertexes without lepton" << endl
      << "   number of tracks without good lepton+track vertex:  " << tracks_without_vertex                   .size() << endl
      << "   number of          2-track vertexes without lepton: " << result_extra.vtxs_init_passed           .size() << endl
      << "   number of unmerged 2-track vertexes without lepton: " << result_extra.vtxs_init_passed_not_merged.size() << endl
      << "   number of merged  vertexes without lepton:          " << result_extra.vtxs_new_merged            .size());


  //---------------------------------------------------------------------------------------------
  // Next, processes 2-track vertexes that contain lepton track
  //

  ATH_MSG_DEBUG("===========================================================================" << endl
      << name() << "::mergeInitVertices - process 2-track vertexes with lepton" << endl
      << "   lepton track pT="                      << tracklep->pt()                 << endl
      << "   number of initial  2-track vertices: " << init_vtxs.size()               << endl
      << "   number of selected 2-track vertices: " << result.vtxs_init_passed.size() << endl
      << "   number of selected ID tracks:        " << selected_tracks.size() );


  //
  // Merge 2-track vertexes that contain lepton track
  //
  mergeIteratively2TrackVtxs(input, result, Prompt::kIterativeFitVtx);

  if(result.vtxs_init_passed.size() > 1) {
    //
    // Fill histograms only when there are at least two vertexes to merge
    //
    FillTH1(h_nvtx_2trk_pass,     nvtx_pass);
    FillTH1(h_nvtx_2trk_unmerged, result.vtxs_init_passed_not_merged.size());
    FillTH1(h_nvtx_merged,        result.vtxs_new_merged            .size());
  }

  //
  // Erase vector of 2-track vertices without lepton that were merged
  //
  result_extra.vtxs_init_passed.clear();

  //---------------------------------------------------------------------------------------------
  // Add vertices without lepton track to result
  //
  result.vtxs_init_passed_not_merged.insert(result.vtxs_init_passed_not_merged.end(), 
      result_extra.vtxs_init_passed_not_merged.begin(), 
      result_extra.vtxs_init_passed_not_merged.end());

  result.vtxs_new_merged.insert(result.vtxs_new_merged.end(), 
      result_extra.vtxs_new_merged.begin(), 
      result_extra.vtxs_new_merged.end());


  ATH_MSG_DEBUG("   number of initial  2-track vertices: " << init_vtxs                         .size() << endl
      << "   number of passed   2-track vertices: " << result.vtxs_init_passed           .size() << endl
      << "   number of unmerged 2-track vertices: " << result.vtxs_init_passed_not_merged.size() << endl
      << "   number of merged           vertices: " << result.vtxs_new_merged            .size() << endl
      << endl
      << "   number of tracks without good lepton+track vertex:  " << tracks_without_vertex                   .size() << endl
      << "   number of          2-track vertexes without lepton: " << result_extra.vtxs_init_passed           .size() << endl
      << "   number of unmerged 2-track vertexes without lepton: " << result_extra.vtxs_init_passed_not_merged.size() << endl
      << "   number of merged  vertexes without lepton:          " << result_extra.vtxs_new_merged            .size() << endl
      << name() << "::mergeInitVertices - ALL DONE" << endl
      << "===========================================================================" );


  return result;
}

//=============================================================================
bool Prompt::VertexIterativeFitMergingTool::mergeIteratively2TrackVtxs(const FittingInput &input,
    MergeResult &result, 
    const VtxType vtxType)
{
  /*
     This function merges iterively vertexes with this algorithm:
     o) Sort 2-track vertexes by sum of track pT
     o) Select the vertex with the highest sum of track pT as the seed vertex
     - Sort all other vertexes by the distance to the seed vertex
     - Add tracks from the closest vertex to the selected vertex
     - Fit new vertex:
     -- if the new vertex passes cuts, select as the new seed vertex
     -- if the new vertex fails cuts, continue with the original seed vertex
     -- Remove this closest vertex from the list
     - Resort remaining tracks by the distance to the seed vertex and repeat
     o) Remove the 2-track vertexes that were merged from the global list and repeat
     */

  //
  // Only 0 or 1 2-track vertices - add the vertex to not merged list and return
  //
  if(result.vtxs_init_passed.size() < 2) {
    result.vtxs_init_passed_not_merged = result.vtxs_init_passed;


    ATH_MSG_DEBUG(name() << "::mergeIteratively2TrackVtxs - too few vertexes: nothing more to do");


    return false;
  }

  //
  // Make 2-track vertex data structures
  //
  vector<TwoTrackVtx> vtxs_2track;

  for(xAOD::Vertex *vtx: result.vtxs_init_passed) {
    if(vtx->nTrackParticles() != 2) {
      ATH_MSG_WARNING("mergeIteratively2TrackVtxs - wrong number of tracks: " << vtx->nTrackParticles());
      continue;
    }

    if(vtx->nTrackParticles() !=2 ) {
      ATH_MSG_WARNING("mergeIteratively2TrackVtxs - vertex does not contain 2 TrackParticles: ntrack=" << vtx->nTrackParticles());
      continue;      
    }

    TwoTrackVtx vtx2track;    
    vtx2track.track_id0 = vtx->trackParticle(0);
    vtx2track.track_id1 = vtx->trackParticle(1);

    if(!vtx2track.track_id0 || !vtx2track.track_id1) {
      ATH_MSG_WARNING("mergeIteratively2TrackVtxs - failed to find TrackParticles for 2-track vertex");      
      continue;
    }

    vtx2track.vertex          = vtx;
    vtx2track.vertex_fit_prob = Prompt::GetVertexFitProb(vtx);
    vtx2track.sum_track_pt    = vtx2track.track_id0->pt() + vtx2track.track_id1->pt();

    vtxs_2track.push_back(vtx2track);
  }


  ATH_MSG_DEBUG(name() << "::mergeIteratively2TrackVtxs - start processing with " << vtxs_2track.size() << " input vertexes ");
  

  if(vtxs_2track.size() < 2) {
    ATH_MSG_WARNING("mergeIteratively2TrackVtxs - logic error: found only " << vtxs_2track.size() << " 2-track vertex");
    return false;
  }

  //
  // Sort 2-track vertexes by ID track pT
  //
  std::sort(vtxs_2track.begin(), vtxs_2track.end(), Prompt::SortTwoTrackVtxBySumTrackPt());

  
  ATH_MSG_DEBUG(name() << "::mergeIteratively2TrackVtxs - number of 2 track passed vertexes=" << vtxs_2track.size());

  for(const TwoTrackVtx &vtx: vtxs_2track) {
    ATH_MSG_DEBUG("Input vertex with 2 tracks sum pT=" << vtx.sum_track_pt << "\n    " << VtxAsStr(vtx.vertex, true));
    }


    //
    // Plot distances between all unique pairs of 2-track vertexes
    //
    plotVertexDistances(vtxs_2track);

    //
    // Iterative fit vertices for merging:
    //
    vector<TwoTrackVtx>::iterator curr_vit = vtxs_2track.begin();

    while(curr_vit != vtxs_2track.end()) {
      //
      // Seed new vertex with 2-track vertex containing highest pT ID track (non-lepton track)
      //
      const double curr_sum_track_pt = curr_vit->sum_track_pt;
      const double curr_track0_pt    = curr_vit->track_id0->pt();
      const double curr_track1_pt    = curr_vit->track_id1->pt();


      ATH_MSG_DEBUG("*************************************************************************** START NEW" << endl
	        << "Curr sum track pT=" << curr_sum_track_pt
	        << ", track0 pT =" << curr_track0_pt
	        << ", track1 pT =" << curr_track1_pt );
    

    //
    // Select 2-track vertex other than the seed vertex itself
    //
    vector<TwoTrackVtx> others;

    for(vector<TwoTrackVtx>::iterator vit = vtxs_2track.begin(); vit != vtxs_2track.end(); vit++) {
      if(vit != curr_vit) {
	others.push_back(*vit);
      }
    }

    //
    // Sort other vertices by distance to the seed vertex
    //
    xAOD::Vertex *seed_vtx = curr_vit->vertex;

    std::sort(others.begin(), others.end(), Prompt::SortTwoTrackVtxByDistToSeed(seed_vtx));
    
    for(vector<TwoTrackVtx>::iterator vit = others.begin(); vit != others.end(); vit++) {
      const double dist = Prompt::GetDistance(seed_vtx, vit->vertex);
      const double sig1 = Prompt::GetNormDist(seed_vtx->position(), vit->vertex->position(), seed_vtx   ->covariance(), msg(MSG::WARNING));
      const double sig2 = Prompt::GetNormDist(seed_vtx->position(), vit->vertex->position(), vit->vertex->covariance(), msg(MSG::WARNING));

      
      ATH_MSG_DEBUG("    other: track0 pT=" << vit->track_id0->pt() << ", track1 pT=" << vit->track_id1->pt() 
	          << ", dist=" << dist 
	          << ", sig1=" << sig1
	          << ", sig2=" << sig2 );
      
    }

    //
    // Call recursive function to fit seed+closest vertex pairs
    //
    xAOD::Vertex *new_merged_vtx = fitSeedVertexCluster(input, seed_vtx, vtxType, others);

    if(!new_merged_vtx) {
      ATH_MSG_ERROR("mergeIteratively2TrackVtxs - logic error: null vertex pointer is removed by fitSeedVertexCluster function");
      curr_vit++;
      continue;
    }
    
    if(new_merged_vtx != seed_vtx) {
      //
      // Save new merged vertex (it is new because it is distinct from seed vertex)
      //
      result.vtxs_new_merged.push_back(new_merged_vtx);

      //
      // Remove 2-track vertexes that were merged
      //
      removeMerged2TrackVertexes(new_merged_vtx, vtxs_2track);
		
      //
      // Reset current vertex iterator to beginning
      //
      curr_vit = vtxs_2track.begin();

      
      ATH_MSG_DEBUG(name() << "::mergeIteratively2TrackVtxs - new merged vertex:\n" << VtxAsStr(new_merged_vtx, false));
      
    }
    else {
      //
      // This vertex could not be merged - try next one
      //
      curr_vit++;

      
      ATH_MSG_DEBUG(name() << "::mergeIteratively2TrackVtxs - could not merge 2-track vertex:\n" << VtxAsStr(seed_vtx, false));
      
    }

    
    ATH_MSG_DEBUG("Done with sum track pT=" << curr_sum_track_pt
		   << ", track0 pT =" << curr_track0_pt
		   << ", track1 pT =" << curr_track1_pt << endl
		   << "***************************************************************************");
    
  }

  //
  // Record unmerged two track vertexes
  //

  for(TwoTrackVtx &vtx: vtxs_2track) {
    result.vtxs_init_passed_not_merged.push_back(vtx.vertex);

    
    ATH_MSG_DEBUG("Unmerged " << VtxAsStr(vtx.vertex, true));

  }  

  if(m_debug) { 
    ATH_MSG_DEBUG(name() << "::mergeIteratively2TrackVtxs - finished processing:" << endl
	 << "   number of unmerged 2-track vertexes=" << vtxs_2track           .size() << endl
	 << "   number of merged           vertexes=" << result.vtxs_new_merged.size() );

    for(TwoTrackVtx &vtx: vtxs_2track) {
      ATH_MSG_DEBUG("Unmerged " << VtxAsStr(vtx.vertex, true));
    }

    for(const xAOD::Vertex *vtx: result.vtxs_new_merged) {
      ATH_MSG_DEBUG("Merged " << VtxAsStr(vtx, true));
    }
  } 

  return true;
}

//=============================================================================
xAOD::Vertex* Prompt::VertexIterativeFitMergingTool::fitSeedVertexCluster(const FittingInput &input,
									  xAOD::Vertex *seed_vtx, 
									  const VtxType vtxType,
									  vector<TwoTrackVtx> &others)
{
  //
  // Reached end of the recursive loop 
  //
  if(others.empty()) {
    return seed_vtx;
  }

  //
  // Re-sort other vertices by distance to the seed vertex - needed because seed position changes when vertices are merged
  //
  std::sort(others.begin(), others.end(), Prompt::SortTwoTrackVtxByDistToSeed(seed_vtx));

  //
  // Take closest vertex
  //
  xAOD::Vertex *curr_vtx = others.front().vertex;
  
  if(!curr_vtx) {
    ATH_MSG_WARNING("VertexIterativeFitMergingTool::fitSeedVertexCluster - current vertex is null pointer");
    return seed_vtx;
  }

  //
  // Remove closest vertex from the list
  //
  others.erase(others.begin());

  //
  // Found nearby vertex - fit merged vertex
  //
  xAOD::Vertex *cand_vtx = fitSeedPlusOtherVertex(input, seed_vtx, curr_vtx, vtxType);
    
  if(!cand_vtx) {
    //
    // Failed to fit new vertex - continue with current seed vertex
    //
    
    ATH_MSG_DEBUG("fitSeedVertexCluster - NEW MERGED VERTEX FIT FAILED" << endl
	        << "---------------------------------------------------------------------------");
    

    return fitSeedVertexCluster(input, seed_vtx, vtxType, others);
  }

  const double prob_cand = GetVertexFitProb(cand_vtx);
  const double prob_seed = GetVertexFitProb(seed_vtx);

  double prob_cand_over_seed = -1.0;
  
  if(prob_seed > 0.0) {
    prob_cand_over_seed = prob_cand/prob_seed;
  }

  const double dist_to_seed = GetDistance(seed_vtx, cand_vtx);
  const double dist_to_curr = GetDistance(curr_vtx, cand_vtx);
  
  FillTH1(h_newVtxFit_chi2, cand_vtx->chiSquared());
  FillTH1(h_newVtxFit_prob, prob_cand);

  FillTH1(h_newVtxFit_dist_toSeed,       dist_to_seed);
  FillTH1(h_newVtxFit_dist_toCurr,       dist_to_curr);  
  FillTH1(h_newVtxFit_prob_candOverSeed, prob_cand_over_seed);
  
  if(seed_vtx->nTrackParticles() > 2) {
    FillTH1(h_newVtxFit_prob_candOverSeed_3trk, prob_cand_over_seed);
  }
  
  stringstream str;
  
  
    str << "   dist to seed=" << dist_to_seed << ", prob_cand_over_seed=" << prob_cand_over_seed << endl
	<< "   seed: "        << VtxAsStr(seed_vtx, false) << endl
	<< "   curr: "        << VtxAsStr(curr_vtx, true)
	<< "   cand: "        << VtxAsStr(cand_vtx, true)
	<< "fitSeedVertexCluster - finished" << endl
	<< "---------------------------------------------------------------------------" << endl;
   

  
  if(!(passVertexSelection(cand_vtx) && prob_cand_over_seed > m_minCandOverSeedFitProbRatio)) {
    //
    // New fitted merged vertex failed selection
    //
    
    ATH_MSG_DEBUG("fitSeedVertexCluster - FAIL NEW MERGED VERTEX\n" << str.str());
    
    
    FillTH1(h_newVtxFit_dist_toSeed_fail,       dist_to_seed);
    FillTH1(h_newVtxFit_prob_candOverSeed_fail, prob_cand_over_seed);

    //
    // Continue with current seed vertex
    //
    return fitSeedVertexCluster(input, seed_vtx, vtxType, others);
  }

  FillTH1(h_newVtxFit_dist_toSeed_pass,       dist_to_seed);
  FillTH1(h_newVtxFit_prob_candOverSeed_pass, prob_cand_over_seed);

  if(seed_vtx->nTrackParticles() > 2) {
    FillTH1(h_newVtxFit_prob_candOverSeed_3trk_pass, prob_cand/prob_seed);
  }

  //
  // Succesfully fitted new vertex
  //      
  
  ATH_MSG_DEBUG("fitSeedVertexCluster - PASS NEW MERGED VERTEX" << str.str());
  
  
  return fitSeedVertexCluster(input, cand_vtx, vtxType, others);
}

//=============================================================================
unsigned Prompt::VertexIterativeFitMergingTool::removeMerged2TrackVertexes(const xAOD::Vertex *merged_vtx, 
									   std::vector<TwoTrackVtx> &vtxs)
{
  //
  // Remove 2-tracks that are succesfully merged into one vertex
  //
  if(!merged_vtx) {
    ATH_MSG_WARNING("VertexIterativeFitMergingTool::removeMerged2TrackVertexes - merged vertex is null pointer");
    return 0;
  }

  unsigned icount = 0;  
  std::vector<TwoTrackVtx>::iterator vit = vtxs.begin();

  while(vit != vtxs.end()) {
    int icount_matched_track = 0;

    for(unsigned k = 0; k < merged_vtx->nTrackParticles(); ++k) {
      const xAOD::TrackParticle *track = merged_vtx->trackParticle(k);
      
      if(!track) {
	ATH_MSG_WARNING("removeMerged2TrackVertexes - merged vertex contains null TrackParticle pointer");
	continue;
      }
      
      if(vit->track_id0 == track) { icount_matched_track++; }
      if(vit->track_id1 == track) { icount_matched_track++; }
    }

    if(icount_matched_track == 2) {
      //
      // Found 2-track vertex that was merged - remove this vertex
      //
      vit = vtxs.erase(vit);
      icount++;

      ATH_MSG_DEBUG("removeMerged2TrackVertexes - removed merged 2-track vertex");
    } 
    else {
      vit++;

      ATH_MSG_DEBUG("removeMerged2TrackVertexes - skip unmerged 2-track vertex");
    }
  }

  
  ATH_MSG_DEBUG(name() << "::removeMerged2TrackVertexes - merged vertex ntrack=" << merged_vtx->nTrackParticles()
	 << ", removed " << icount << " merged 2-track vertexes");
  

  return icount;
}

//=============================================================================
void Prompt::VertexIterativeFitMergingTool::plotVertexDistances(const vector<TwoTrackVtx> &others)
{
  for(vector<TwoTrackVtx>::const_iterator fit = others.begin(); fit != others.end(); fit++) {
    for(vector<TwoTrackVtx>::const_iterator sit = fit+1; sit != others.end(); sit++) {
      const double dist = Prompt::GetDistance(fit->vertex, sit->vertex);
      const double sig1 = Prompt::GetNormDist(fit->vertex->position(), sit->vertex->position(), fit->vertex->covariance(), msg(MSG::WARNING));
      const double sig2 = Prompt::GetNormDist(fit->vertex->position(), sit->vertex->position(), sit->vertex->covariance(), msg(MSG::WARNING));

      FillTH1(h_Vtx2trkPair_dist,      dist);
      FillTH1(h_Vtx2trkPair_dist_zoom, dist);
      FillTH1(h_Vtx2trkPair_sig1,      sig1);
      FillTH1(h_Vtx2trkPair_sig2,      sig2);
    }
  }
}

//=============================================================================
std::vector<const xAOD::TrackParticle *> Prompt::VertexIterativeFitMergingTool::getTracksWithoutVertex(const std::vector<xAOD::Vertex *> &pass_vtxs,
												       const std::vector<const xAOD::TrackParticle *> &selected_tracks)
{
  //
  // Plot unmatched tracks
  //
  std::vector<const xAOD::TrackParticle *> tracks_without_vertex;

  unsigned icount_domatch = 0;

  for(const xAOD::TrackParticle *track: selected_tracks) {
    bool match = false;

    for(const xAOD::Vertex *vtx: pass_vtxs) {
      for(unsigned k = 0; k < vtx->nTrackParticles(); ++k) {
	const xAOD::TrackParticle *vtx_track = vtx->trackParticle(k);

	if(vtx_track == track) {
	  match = true;
	  break;
	}
      }   
    }

    if(match) {
      icount_domatch++;
    }
    else {
      tracks_without_vertex.push_back(track);
    }
  }

  FillTH1(h_selectedTrack_CountAll,         selected_tracks.size());
  FillTH1(h_selectedTrack_CountMatch2Vtx,   icount_domatch);
  FillTH1(h_selectedTrack_CountWithout2Vtx, tracks_without_vertex.size());

  return tracks_without_vertex;
}

//=============================================================================
bool Prompt::VertexIterativeFitMergingTool::passVertexSelection(const xAOD::Vertex *vtx) const
{
  //
  // Check whether vertex passes quality cuts
  //
  if(!vtx) {
    ATH_MSG_WARNING("passVertexSelection - input vertex is null pointer");
    return false;
  }

  if(!(vtx->numberDoF() > 0 && vtx->chiSquared() >= 0)) {
    return false;
  }
  
  const double fit_prob = Prompt::GetVertexFitProb(vtx);

  ATH_MSG_DEBUG("passVertexSelection - vertex pointer=" << vtx << " chi2/ndof=" << vtx->chiSquared() << "/" << vtx->numberDoF() << ", prob=" << fit_prob);  

  return fit_prob > m_minFitProb;
}

//=============================================================================
xAOD::Vertex* Prompt::VertexIterativeFitMergingTool::fitSeedPlusOtherVertex(const FittingInput &input,
									    const xAOD::Vertex *seed_vtx, 
									    const xAOD::Vertex *other_vtx,
									    const VtxType vtxType)
{
  //
  // Fit two 2-track vertexes
  //
  if(!seed_vtx) {
    ATH_MSG_WARNING("fitSeedPlusOtherVertex - null seed Vertex pointer");    
    return 0;
  }

  if(!other_vtx) {
    ATH_MSG_WARNING("fitSeedPlusOtherVertex - null other Vertex pointer");
    return 0;
  }

  if(other_vtx->nTrackParticles() != 2) {
    ATH_MSG_WARNING("fitSeedPlusOtherVertex - other Vertex does not have 2 tracks: ntrack=" << other_vtx->nTrackParticles());
    return 0;
  }

  //
  // Collect tracks from the seed vertex
  //
  vector<const xAOD::TrackParticle *> tracks;

  for(unsigned k = 0; k < seed_vtx->nTrackParticles(); ++k) {
    const xAOD::TrackParticle *track = seed_vtx->trackParticle(k);
    
    if(track) {
      tracks.push_back(track);
    }
    else {
      ATH_MSG_WARNING("fitSeedPlusOtherVertex - seed vertex contains TrackParticle null pointer");
    }
  }

  //
  // Collect tracks from other vertices
  //
  for(unsigned k = 0; k < other_vtx->nTrackParticles(); ++k) {
    const xAOD::TrackParticle *track = other_vtx->trackParticle(k);
    
    if(track) {
      tracks.push_back(track);
    }
    else {
      ATH_MSG_WARNING("fitSeedPlusOtherVertex - other vertex contains TrackParticle null pointer");
    }
  }
  
  //
  // Fit new vertex
  //
  std::pair<xAOD::Vertex *, int> secVtx = m_vertexFitterSvc->fitVertexWithSeed(input, tracks, seed_vtx->position(), vtxType);
  
  if(!secVtx.first) {
    ATH_MSG_WARNING("fitSeedPlusOtherVertex - failed to fit vertex");
    return 0;
  }
  
  return secVtx.first;
}

//=============================================================================
std::vector<xAOD::Vertex *> Prompt::VertexIterativeFitMergingTool::fit2TrackVertexes(const FittingInput &input,
										     std::vector<const xAOD::TrackParticle *> &selected_tracks,
										     const VtxType vtxType)
{
  //
  // Fit all possible combinations of two 2-track vertexes
  //
  std::vector<xAOD::Vertex *> pass_vtxs;

  if(selected_tracks.size() < 2) {
    ATH_MSG_DEBUG("fit2TrackVertexeses - 0 or 1 input tracks - nothing to do");    
    return pass_vtxs;
  }

  
  ATH_MSG_DEBUG(name() << "::fit2TrackVertexes - start with " << selected_tracks.size() << " tracks");
  

  //
  // Sort tracks by decreasing pT
  //
  std::sort(selected_tracks.begin(), selected_tracks.end(), SortTracksByPt());

  unsigned icount = 0;

  for(std::vector<const xAOD::TrackParticle *>::const_iterator it1 = selected_tracks.begin(); it1 != selected_tracks.end(); ++it1) {
    for(std::vector<const xAOD::TrackParticle *>::const_iterator it2 = it1 + 1; it2 != selected_tracks.end(); ++it2) {
      const xAOD::TrackParticle *track1 = *it1;
      const xAOD::TrackParticle *track2 = *it2;
      
      
      if(!track1 || !track2) {
	ATH_MSG_WARNING("fit2TrackVertexeses - logic error: TrackParticle null pointer");
	continue;
      }

      std::vector<const xAOD::TrackParticle *> fit_tracks = {track1, track2};

      //
      // Fit new vertex
      //
      std::pair<xAOD::Vertex *, int> vtx = m_vertexFitterSvc->fitVertexWithPrimarySeed(input, fit_tracks, vtxType);
      
      icount++;

      if(!vtx.first) {
	ATH_MSG_WARNING("fit2TrackVertexeses - failed to fit vertex");
	continue;
      }

      if(passVertexSelection(vtx.first)) {
	pass_vtxs.push_back(vtx.first);

	
        ATH_MSG_DEBUG("fit2TrackVertexeses - pass vertex: " << VtxAsStr(vtx.first, true));
	
      }
      else {
	//
	// Do not save vertex that failed selection - allocated memory will be cleaned by fitting service
	//
      }
    }
  }

  
  ATH_MSG_DEBUG(name() << "::fit2TrackVertexes - finished processing: " << endl
	      << "   number of input tracks:            " << selected_tracks.size() << endl
	      << "   number of 2-track combinations:    " << icount                 << endl
	      << "   number of passed 2-track vertexes: " << pass_vtxs      .size() << endl
	      << name() << "::fit2TrackVertexes - all is done" );
  

  return pass_vtxs;
}

//=============================================================================
StatusCode Prompt::VertexIterativeFitMergingTool::makeHist(TH1 *&h, const std::string &key, int nbin, double xmin, double xmax)
{
  //
  // Initiliase histogram pointer. If configured to run in validation mode, then create and register histogram
  //
  h = 0;

  if(m_outputStream.empty() || key.empty()) {    
    return StatusCode::SUCCESS;
  }

  const std::string hname    = name() + "_" + key;
  const std::string hist_key = "/"+m_outputStream+"/"+hname;

  h = new TH1D(hname.c_str(), hname.c_str(), nbin, xmin, xmax);
  h->SetDirectory(0);

  return m_histSvc->regHist(hist_key, h);
}
