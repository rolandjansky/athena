/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_AmbiguitySolver.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetZVTOPVxFinder/ZVTOP_AmbiguitySolver.h"
#include "InDetZVTOPVxFinder/IZVTOP_TrkProbTubeCalc.h"
#include "InDetZVTOPVxFinder/IZVTOP_SimpleVtxProbCalc.h"
#include "VxVertex/VxCandidate.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxTrackAtVertex.h"
#include <map>

//================ Constructor =================================================

InDet::ZVTOP_AmbiguitySolver::ZVTOP_AmbiguitySolver(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  base_class(t,n,p),
  m_TrkProbTubeCalc("InDet::ZVTOP_TrkProbTubeCalc"),
  m_VtxProbCalc("InDet::ZVTOP_VtxProbCalc")
{
  //  template for property decalration
  declareProperty("TrkProbTubeCalcTool",m_TrkProbTubeCalc);
  declareProperty("VtxProbCalcTool",m_VtxProbCalc);
}

//================ Destructor =================================================

InDet::ZVTOP_AmbiguitySolver::~ZVTOP_AmbiguitySolver()
{}


//================ Initialisation =================================================

StatusCode InDet::ZVTOP_AmbiguitySolver::initialize()
{
  //Gaussian Probability Tube for the Track Trajectory
  ATH_CHECK( m_TrkProbTubeCalc.retrieve());
  //Vertex Probability Function
  ATH_CHECK( m_VtxProbCalc.retrieve() );
  ATH_MSG_DEBUG( "initialize() successful ");
  return StatusCode::SUCCESS;
}

std::vector<xAOD::Vertex*> InDet::ZVTOP_AmbiguitySolver::solveAmbiguities(std::vector<xAOD::Vertex*> VertexContainer) const
{
  std::vector<xAOD::Vertex*> newVertexContainer;
  std::vector<xAOD::Vertex*>::iterator itr = VertexContainer.begin();
  std::map<double, xAOD::Vertex*> vxprob_map;
  double sum_TrkProbTube = 0.;
  double sum2_TrkProbTube = 0.;
  for (; itr != VertexContainer.end(); ++itr){
    Amg::Vector3D pos = (*itr)->position();
    Trk::Vertex vtx_pos(pos);
    std::vector<Trk::VxTrackAtVertex> trksAtVtx = (*itr)->vxTrackAtVertex();
    std::vector<Trk::VxTrackAtVertex>::iterator trk_itr = trksAtVtx.begin();
    for (; trk_itr != trksAtVtx.end(); ++trk_itr ){
      const Trk::Perigee*	perigee = dynamic_cast<const Trk::Perigee*>((*trk_itr).initialPerigee());
      if (not perigee) continue;
      double TrkProbTube = m_TrkProbTubeCalc->calcProbTube(perigee, vtx_pos);
      sum_TrkProbTube  += TrkProbTube;
      sum2_TrkProbTube += (TrkProbTube * TrkProbTube);
    }
    double vtx_prob = m_VtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
    vxprob_map.insert(std::map<double, xAOD::Vertex*>::value_type(vtx_prob,(*itr)));
  }
  
  std::map<double, xAOD::Vertex*>::reverse_iterator mapItr = vxprob_map.rbegin();
  std::map<double, xAOD::Vertex*> initial_perigee_pos_map;
  for(; mapItr!=vxprob_map.rend(); ++mapItr){
    xAOD::Vertex* vtx = (*mapItr).second;
    std::vector<Trk::VxTrackAtVertex> trkAtVtx = vtx->vxTrackAtVertex();
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtx_Iter = trkAtVtx.begin();
    for (; trkAtVtx_Iter!= trkAtVtx.end(); ++trkAtVtx_Iter){
      const Trk::Perigee*	perigee = dynamic_cast<const Trk::Perigee*>((*trkAtVtx_Iter).initialPerigee());
      if (not perigee) continue;
      Amg::Vector3D pos = perigee->position();
      initial_perigee_pos_map.insert(std::map<double, xAOD::Vertex*>::value_type(pos.mag(),vtx));
    }
  }
  mapItr = vxprob_map.rbegin();
  for (; mapItr!= vxprob_map.rend(); ++mapItr){
    xAOD::Vertex* vtx = (*mapItr).second;
    std::vector<Trk::VxTrackAtVertex> trkAtVtx = vtx->vxTrackAtVertex();
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtx_Iter = trkAtVtx.begin();
    for (; trkAtVtx_Iter!= trkAtVtx.end(); ++trkAtVtx_Iter){
      const Trk::Perigee*	perigee = dynamic_cast<const Trk::Perigee*>((*trkAtVtx_Iter).initialPerigee());
      if (not perigee) continue;
      double pos_mag = perigee->position().mag();
      std::map<double, xAOD::Vertex*>::iterator pos_magItr = initial_perigee_pos_map.find(pos_mag);
      if (pos_magItr != initial_perigee_pos_map.end()) {
        if (vtx != (*pos_magItr).second){
          trkAtVtx.erase(trkAtVtx_Iter);
          if (trkAtVtx_Iter == trkAtVtx.end()) break;
        }
      }
    }
    if (trkAtVtx.size() > 1) { 
      newVertexContainer.push_back(vtx);
    }
  }
  return newVertexContainer;
}
//================ Finalisation =================================================

StatusCode InDet::ZVTOP_AmbiguitySolver::finalize()
{
  return StatusCode::SUCCESS;
}

//============================================================================================

