/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "VertexMergingTool.h"
#include "VarHolder.h"
#include "PromptUtils.h"

using namespace std;

//=============================================================================
Prompt::VertexMergingTool::VertexMergingTool(const std::string &name,
                                             const std::string &type,
                                             const IInterface  *parent):
  AthAlgTool       (name, type, parent),
  m_vertexFitterSvc("Prompt::VertexFittingSvc/PromptVertexFittingSvc", name)
{  
  declareInterface<Prompt::IVertexMergingTool>(this); 
  
  declareProperty("debug",                 m_debug                 = false);

  declareProperty("useMinNormDist",        m_useMinNormDist        = false);
  declareProperty("minFitProb",            m_minFitProb            = 0.01);
  declareProperty("minDistanceClusterVtx", m_minDistanceClusterVtx = 1.00);

  declareProperty("VertexFittingSvc",      m_vertexFitterSvc);
}

//=============================================================================
StatusCode Prompt::VertexMergingTool::initialize()
{
  ATH_CHECK(m_vertexFitterSvc.retrieve());
  
  return StatusCode::SUCCESS;
}

//=============================================================================
Prompt::MergeResult Prompt::VertexMergingTool::mergeInitVertices(const FittingInput &input,
								 const xAOD::TrackParticle *tracklep, 
								 const std::vector<xAOD::Vertex *> &init_vtxs,
								 const std::vector<const xAOD::TrackParticle *> &selected_tracks)
{
  //
  // Merge initial (2-track) vertices into new merged vertices with three tracks or more
  //
  MergeResult result;

  for(xAOD::Vertex *vtx: init_vtxs) {
    if(passVertexSelection(vtx)) {
      result.vtxs_init_passed.push_back(vtx);
    }
  }

  if(m_debug) {
    ATH_MSG_INFO(name() << "::mergeInitVertices - start processing" << endl
	                << "   number of initial  2-track vertices: " << init_vtxs.size()               << endl
	                << "   number of selected 2-track vertices: " << result.vtxs_init_passed.size() << endl
	                << "   number of selected ID tracks:        " << selected_tracks.size() );
  }

  const unsigned nvtx_init = init_vtxs.size();
  const unsigned nvtx_pass = result.vtxs_init_passed.size();

  //
  // Make vertex clusters
  //
  std::vector<VtxCluster> clusters, clusters_cand, clusters_1vtx;

  makeClusters(clusters, result.vtxs_init_passed);
  
  for(const VtxCluster &cluster: clusters) {
    ATH_MSG_INFO("   cluster candidate with nvertex=" << cluster.vtxs_init.size());
 
    if(cluster.vtxs_init.size() < 1) {
      ATH_MSG_INFO("VertexMergingTool::mergeInitVertices - logic error: cluster with zero vertexes!!");
    }
    else if(cluster.vtxs_init.size() == 1) {
      clusters_1vtx.push_back(cluster);
    }
    else {
      clusters_cand.push_back(cluster);
    }
  }

  ATH_MSG_INFO("   # init vertexes:   " << nvtx_init            << endl
            << "   # pass vertexes:   " << nvtx_pass            << endl
            << "   cluster_1vtx size: " << clusters_1vtx.size() << endl
            << "   cluster_cand size: " << clusters_cand.size() );

  //
  // Found zero clusters with two or more vertices - nothing nore to do
  //
  if(clusters_cand.empty()) {
    return result;
  }

  for(VtxCluster &cluster: clusters_cand) {
    //
    // Initiliase ID tracks
    //
    cluster.trks_curr = cluster.trks_init;

    //
    // Remove lepton tracks from list of tracks for fitting - the lepton track is added by VertexFittingSvc
    //
    cluster.trks_curr.erase(std::remove(cluster.trks_curr.begin(), cluster.trks_curr.end(), tracklep), cluster.trks_curr.end());

    if(m_debug) {
      ATH_MSG_INFO("Cluster vtxs_init size=" << cluster.vtxs_init.size() << endl
	        << "        trks_init size=" << cluster.trks_init.size() << endl
	        << "        trks_curr size=" << cluster.trks_curr.size() );

      for(const xAOD::Vertex *vtx: cluster.vtxs_init) {
	ATH_MSG_INFO("   init vtx:  " << VtxAsStr(vtx, true));
      }
      for(const xAOD::TrackParticle *trk: cluster.trks_init) {
	ATH_MSG_INFO("   init trk:  " << TrkAsStr(trk));
      }
      for(const xAOD::TrackParticle *trk: cluster.trks_curr) {
	ATH_MSG_INFO("   curr trk:  " << TrkAsStr(trk));
      }
    }

    if(cluster.trks_curr.size() != cluster.vtxs_init.size()) {
      ATH_MSG_WARNING("mergeInitVertices - input vertices are not all 2-track: nvtx != ntrk: " << cluster.trks_curr.size() << "!=" << cluster.vtxs_init.size());
    }
  }

  if(m_debug) {
    ATH_MSG_INFO("Process " << clusters_cand.size() << " candidate clusters");
  }

  for(VtxCluster &cluster: clusters_cand) {
    //
    // Fit cluster of vertices to obtain one merged vertex
    //
    fitVertexCluster(input, tracklep, cluster);

    if(cluster.vtx_merged) {
      result.vtxs_new_merged.push_back(cluster.vtx_merged);
    }
    else {
      if(m_debug) {
	ATH_MSG_INFO("   FAILED TO MERGE VERTEX");
      }
    }
    
    cluster.vtxs_fitted_bad.clear();
  }

  if(m_debug) {
    ATH_MSG_INFO(name() << "::mergeInitVertices - result size=" << result.vtxs_new_merged.size());
  }

  return result;
}

//=============================================================================
bool Prompt::VertexMergingTool::passVertexSelection(const xAOD::Vertex *vtx) const
{
  //
  // Check whether vertex passes quality cuts
  //
  if(!vtx) {
    ATH_MSG_WARNING("passVertexSelection - input vertex is null pointer");
    return false;
  }

  if(!(vtx->numberDoF() > 0 && vtx->chiSquared() > 0)) {
    return false;
  }
  
  const double fit_prob = Prompt::GetVertexFitProb(vtx);

  ATH_MSG_DEBUG("passVertexSelection - vertex pointer=" << vtx << " chi2/ndof=" << vtx->chiSquared() << "/" << vtx->numberDoF() << ", prob=" << fit_prob);  

  return fit_prob > m_minFitProb;
}

//=============================================================================
bool Prompt::VertexMergingTool::makeClusters(std::vector<VtxCluster> &clusters, std::vector<xAOD::Vertex *> &init_vtxs)
{
  //
  // Make clusters from initial vertexes
  //
  std::vector<xAOD::Vertex *>::iterator icurr_vtx = init_vtxs.begin(); 

  //
  // Seed initial cluster with the first vertex on the list
  //
  while(icurr_vtx != init_vtxs.end()) {
    bool match_curr = false;

    //
    // First check whether this vertex can be included with existing clusters
    //
    for(VtxCluster &cluster: clusters) {
      if(matchVtxToCluster(cluster, *icurr_vtx)) {
	addInitVtxToCluster(cluster, *icurr_vtx);
	match_curr = true;
	break;
      }
    }


    if(!match_curr) {
      //
      // Start new cluster with current vertex
      //
      clusters.push_back(VtxCluster());
      addInitVtxToCluster(clusters.back(), *icurr_vtx);
    }

    //
    // Erase current vertex and start at the beginning
    //
    init_vtxs.erase(icurr_vtx);
    icurr_vtx = init_vtxs.begin();
  }

  return !clusters.empty();
}

//=============================================================================
bool Prompt::VertexMergingTool::matchVtxToCluster(const VtxCluster &cluster, const xAOD::Vertex *vtx) const
{
  //
  // Add vertex to cluster - always add vertex to empty cluster
  //
  if(!vtx) {
    ATH_MSG_WARNING("matchVtxToCluster - input vertex is null pointer");
    return false;
  }

  //
  // Empty cluster does not match any vertex
  //
  bool match_vtx = false;

  for(const xAOD::Vertex *cluster_vtx: cluster.vtxs_init) {
    double dist = Prompt::GetDistance(cluster_vtx, vtx);

    if(m_useMinNormDist) { dist = getMinNormDistVtx  (cluster_vtx, vtx); }
    else                 { dist = Prompt::GetDistance(cluster_vtx, vtx); }

    if(dist < m_minDistanceClusterVtx) {
      match_vtx = true;
      break;
    }
  }

  return match_vtx;
}

//=============================================================================
bool Prompt::VertexMergingTool::addInitVtxToCluster(VtxCluster &cluster, const xAOD::Vertex *vtx) const
{
  //
  // Add vertex to cluster - always add vertex to empty cluster
  //
  if(!vtx) {
    ATH_MSG_WARNING("AddVtxToCluster - input vertex is null pointer");
    return false;
  }

  if(vtx->nTrackParticles() != 2) {
    ATH_MSG_WARNING("AddVtxToCluster - wrong number of tracks: " << vtx->nTrackParticles());
  }
  
  cluster.vtxs_init.push_back(vtx);
  
  for(unsigned k = 0; k < vtx->nTrackParticles(); ++k) {
    const xAOD::TrackParticle *track = vtx->trackParticle(k);
    
    if(track) {
      cluster.trks_init.push_back(track);
    }
    else {
      ATH_MSG_WARNING("passVertexSelection - vertex contains TrackParticle null pointer");
    }
  }
  
  return true;
}

//=============================================================================
bool Prompt::VertexMergingTool::fitVertexCluster(const FittingInput &input,
						 const xAOD::TrackParticle *tracklep, 
						 VtxCluster &cluster)
{
  //
  // Fit recursively merged vertex until:
  //  -- good quality vertex is obtained OR;
  //  -- number of tracks is less than 3.
  //
  if(cluster.trks_curr.size() < 2) {
    ATH_MSG_WARNING("fitVertexCluster - number of input tracks is " << cluster.trks_curr.size() << " - nothing to do");
    return false;
  }

  if(m_debug) {
    ATH_MSG_INFO("fitVertexCluster - trks_curr.size()=" << cluster.trks_curr.size() << endl
              << "   lepton: " << TrkAsStr(tracklep)); 
    
    for(const xAOD::TrackParticle *trk: cluster.trks_curr) {
      ATH_MSG_INFO("   track:  " << TrkAsStr(trk));
    }
  }
 
  std::pair<xAOD::Vertex *, int> secVtx = m_vertexFitterSvc->fitVertexWithPrimarySeed(input, cluster.trks_curr, kDeepMergedVtx);
  
  if(!secVtx.first) {
    ATH_MSG_WARNING("fitVertexCluster - failed to fit vertex");
    return false;
  }
  
  if(passVertexSelection(secVtx.first)) {
    //
    // Obtained good vertex fit - stop iterations
    //
    cluster.vtx_merged = secVtx.first;

    return true;
  }

  return false;
}

//=============================================================================
double Prompt::VertexMergingTool::getMinNormDistVtx(const xAOD::Vertex *vtx1, const xAOD::Vertex *vtx2) const
{  
  double mini_normDist = 9999.0;

  if((!vtx1) || (!vtx2)) {
    return mini_normDist;
  }
 
  const Amg::Vector3D psvtx1 = vtx1->position();
  const Amg::Vector3D psvtx2 = vtx2->position();
  
  double svtx12_normDist1 = Prompt::GetNormDist(psvtx1, psvtx2, vtx1->covariance(), msg(MSG::WARNING));
  double svtx12_normDist2 = Prompt::GetNormDist(psvtx1, psvtx2, vtx2->covariance(), msg(MSG::WARNING));

  if (svtx12_normDist2 < svtx12_normDist1) { mini_normDist = svtx12_normDist2; }
  else                                     { mini_normDist = svtx12_normDist1; }

  return mini_normDist;
}

