/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//================ Constructor =================================================

InDet::ZVTOP_AmbiguitySolver::ZVTOP_AmbiguitySolver(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_TrkProbTubeCalc("InDet::ZVTOP_TrkProbTubeCalc"),
  m_VtxProbCalc("InDet::ZVTOP_VtxProbCalc")
{
  declareInterface<IZVTOP_AmbiguitySolver>(this);

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
  
  StatusCode sc = AlgTool::initialize();

  if (sc.isFailure()) return sc;
  //Gaussian Probability Tube for the Track Trajectory
  if ( m_TrkProbTubeCalc.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_TrkProbTubeCalc<< endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_TrkProbTubeCalc << endreq;

  //Vertex Probability Function
  if ( m_VtxProbCalc.retrieve().isFailure() ) {
      msg (MSG::FATAL) << "Failed to retrieve tool " << m_VtxProbCalc<< endreq;
      return StatusCode::FAILURE;
  } else msg (MSG::INFO) << "Retrieved tool " << m_VtxProbCalc << endreq;
  msg (MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//std::vector<Trk::VxCandidate*> InDet::ZVTOP_AmbiguitySolver::solveAmbiguities(std::vector<Trk::VxCandidate*> VxContainer) --David S.
std::vector<xAOD::Vertex*> InDet::ZVTOP_AmbiguitySolver::solveAmbiguities(std::vector<xAOD::Vertex*> VertexContainer)
{
  //std::vector<Trk::VxCandidate*> newVxContainer; --David S.
  std::vector<xAOD::Vertex*> newVertexContainer;
  //std::vector<Trk::VxCandidate*>::iterator itr = VxContainer.begin(); --David S.
  std::vector<xAOD::Vertex*>::iterator itr = VertexContainer.begin();
  //std::map<double, Trk::VxCandidate*> vxprob_map; --David S.
  std::map<double, xAOD::Vertex*> vxprob_map;
  double sum_TrkProbTube = 0.;
  double sum2_TrkProbTube = 0.;
  //for (; itr != VxContainer.end(); ++itr){ --David S.
  for (; itr != VertexContainer.end(); ++itr){
    //Amg::Vector3D pos = (*itr)->recVertex().position(); --David S.
    Amg::Vector3D pos = (*itr)->position();
    Trk::Vertex vtx_pos(pos);
    //std::vector<Trk::VxTrackAtVertex*> trksAtVtx = *((*itr)->vxTrackAtVertex()); --David S.
    std::vector<Trk::VxTrackAtVertex> trksAtVtx = (*itr)->vxTrackAtVertex();
    //std::vector<Trk::VxTrackAtVertex*>::iterator trk_itr = trksAtVtx.begin(); --David S.
    std::vector<Trk::VxTrackAtVertex>::iterator trk_itr = trksAtVtx.begin();
    for (; trk_itr != trksAtVtx.end(); ++trk_itr ){
      const Trk::Perigee*	perigee = dynamic_cast<const Trk::Perigee*>((*trk_itr).initialPerigee());
      double TrkProbTube = m_TrkProbTubeCalc->calcProbTube(perigee, vtx_pos);
      sum_TrkProbTube  += TrkProbTube;
      sum2_TrkProbTube += pow(TrkProbTube,2);
    }
    double vtx_prob = m_VtxProbCalc->calcVtxProb(sum_TrkProbTube, sum2_TrkProbTube);
    //vxprob_map.insert(std::map<double, Trk::VxCandidate*>::value_type(vtx_prob,(*itr))); --David S.
    vxprob_map.insert(std::map<double, xAOD::Vertex*>::value_type(vtx_prob,(*itr)));
  }
  
  //std::map<double, Trk::VxCandidate*>::reverse_iterator  mapItr = vxprob_map.rbegin(); --David S.
  std::map<double, xAOD::Vertex*>::reverse_iterator mapItr = vxprob_map.rbegin();
  //std::map<double, Trk::VxCandidate*> initial_perigee_pos_map; --David S.
  std::map<double, xAOD::Vertex*> initial_perigee_pos_map;
  for(; mapItr!=vxprob_map.rend(); ++mapItr){
    //Trk::VxCandidate* vtx = (*mapItr).second; --David S.
    xAOD::Vertex* vtx = (*mapItr).second;
    //std::vector< Trk::VxTrackAtVertex*> trkAtVtx = *(vtx->vxTrackAtVertex()); --David S.
    std::vector<Trk::VxTrackAtVertex> trkAtVtx = vtx->vxTrackAtVertex();
    //std::vector< Trk::VxTrackAtVertex*>::iterator trkAtVtx_Iter = trkAtVtx.begin(); --David S.
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtx_Iter = trkAtVtx.begin();
    for (; trkAtVtx_Iter!= trkAtVtx.end(); ++trkAtVtx_Iter){
      const Trk::Perigee*	perigee = dynamic_cast<const Trk::Perigee*>((*trkAtVtx_Iter).initialPerigee());
      Amg::Vector3D pos = perigee->position();
      //initial_perigee_pos_map.insert(std::map<double, Trk::VxCandidate*>::value_type(pos.mag(),vtx)); --David S.
      initial_perigee_pos_map.insert(std::map<double, xAOD::Vertex*>::value_type(pos.mag(),vtx));
    }
  }
  mapItr = vxprob_map.rbegin();
  for (; mapItr!= vxprob_map.rend(); ++mapItr){
    //Trk::VxCandidate* vtx = (*mapItr).second; --David S.
    xAOD::Vertex* vtx = (*mapItr).second;
    //std::vector< Trk::VxTrackAtVertex*> trkAtVtx = *(vtx->vxTrackAtVertex()); --David S.
    std::vector<Trk::VxTrackAtVertex> trkAtVtx = vtx->vxTrackAtVertex();
    //std::vector< Trk::VxTrackAtVertex*>::iterator trkAtVtx_Iter = trkAtVtx.begin(); --David S.
    std::vector<Trk::VxTrackAtVertex>::iterator trkAtVtx_Iter = trkAtVtx.begin();
    for (; trkAtVtx_Iter!= trkAtVtx.end(); ++trkAtVtx_Iter){
      const Trk::Perigee*	perigee = dynamic_cast<const Trk::Perigee*>((*trkAtVtx_Iter).initialPerigee());
      double pos_mag = perigee->position().mag();
      //std::map<double, Trk::VxCandidate*>::iterator pos_magItr = initial_perigee_pos_map.find(pos_mag); --David S.
      std::map<double, xAOD::Vertex*>::iterator pos_magItr = initial_perigee_pos_map.find(pos_mag);
      if (pos_magItr != initial_perigee_pos_map.end()) {
	if (vtx != (*pos_magItr).second){
	  trkAtVtx.erase(trkAtVtx_Iter);
	  if (trkAtVtx_Iter == trkAtVtx.end()) break;
	}
      }
    }
    if (trkAtVtx.size() > 1) { 
      //newVxContainer.push_back(vtx); --David S.
      newVertexContainer.push_back(vtx);
    }
  }
  //return newVxContainer; --David S.
  return newVertexContainer;
}
//================ Finalisation =================================================

StatusCode InDet::ZVTOP_AmbiguitySolver::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

//============================================================================================

