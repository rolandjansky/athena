/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalPrimaryVertexMonAlg.h
 * Implementation of inner detector global Primary Vertex monitoring tool
 *
 *@author
 * Leonid Serkin <lserkin@cern.ch> @n
 * Per Johansson <Per.Johansson@cern.ch> @n
 *
 * based on InDetGlobalPrimaryVertexMonTool.cxx
 *
 ****************************************************************************/

//main header
#include "InDetGlobalPrimaryVertexMonAlg.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


//Standard c++
#include <vector>


InDetGlobalPrimaryVertexMonAlg::InDetGlobalPrimaryVertexMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_splitVertexTrkInvFraction(2),
  m_distanceSplitVxMatch(5.0),
  m_splitMatchingMetric(3),
  m_doEnhancedMonitoring(false)
{
  declareProperty("splitVertexTrkInvFraction", m_splitVertexTrkInvFraction, "inverse fraction to split tracks (1:N)");
  declareProperty("distanceSplitVertexMatch", m_distanceSplitVxMatch, "Distance for matching split-original Vertex in selection efficiency");
  declareProperty("splitMatchingMetric", m_splitMatchingMetric, "Determines which function to use to calculate matching between split vertices and original input vertex -- used in selection efficiency");
  declareProperty("doEnhancedMonitoring" , m_doEnhancedMonitoring, "turn on the enhanced vertex monitoring, it is triggered by the same InDetFlag that also triggers the creation of no beam constraint and split vertices");
}


InDetGlobalPrimaryVertexMonAlg::~InDetGlobalPrimaryVertexMonAlg() {}


StatusCode InDetGlobalPrimaryVertexMonAlg::initialize() {
  
  
  ATH_CHECK( m_vxContainerName.initialize() );
  ATH_CHECK( m_vxContainerNameWithoutBeamConstraint.initialize() );
  ATH_CHECK( m_vxContainerNameSplit.initialize() );
  
  
  return AthMonitorAlgorithm::initialize();
}


StatusCode InDetGlobalPrimaryVertexMonAlg::fillHistograms( const EventContext& ctx ) const {
  using namespace Monitored;
  
  //*******************************************************************************
  //************************** Begin of filling Track Histograms ******************
  //*******************************************************************************
  ATH_MSG_DEBUG("Filling InDetGlobalPrimaryVertexMonAlg");
  
  // For histogram naming
  auto pvGroup = getGroup("PrimaryVertex");
  
  // retrieving vertices
  auto handle_vxContainer = SG::makeHandle(m_vxContainerName, ctx);   // another way to access ??
  
  if (!handle_vxContainer.isPresent()) {
    ATH_MSG_DEBUG ("InDetGlobalPrimaryVertexMonAlg: StoreGate doesn't contain primary vertex container with key "+m_vxContainerName.key());
    return StatusCode::SUCCESS;
  }
  if (!handle_vxContainer.isValid()) {
    ATH_MSG_ERROR ("InDetGlobalPrimaryVertexMonAlg: Could not retrieve primary vertex container with key "+m_vxContainerName.key());
    return StatusCode::RECOVERABLE;
  }
  
  auto vertexContainer = handle_vxContainer.cptr();


  // Total number of vertices (primary and pile up)
  int pvN = vertexContainer->size()-1;  // exclude dummy vertex
  auto pvN_m = Monitored::Scalar<int>( "m_PvN", pvN);
  fill(pvGroup, pvN_m);
  
  int nPriVtx = 0;
  int nPileupVtx = 0;
 
  for(const auto & vtx : *vertexContainer) {
      
      if ( !vtx ) continue;
      
      // Count different types of vertices
      if (vtx->vertexType() == xAOD::VxType::PriVtx) nPriVtx++;
      if (vtx->vertexType() == xAOD::VxType::PileUp) nPileupVtx++;
      
      
      // Select primary vertex
      if (vtx->vertexType() != xAOD::VxType::PriVtx) continue;
      if (vtx->numberDoF() <= 0) continue;
      
      float pvX = vtx->position().x();
      auto  pvX_m = Monitored::Scalar<float>( "m_PvX", pvX);
      fill(pvGroup, pvX_m);
      
      float pvY = vtx->position().y();
      auto  pvY_m = Monitored::Scalar<float>( "m_PvY", pvY);
      fill(pvGroup, pvY_m);
      
      float pvZ = vtx->position().z();
      auto  pvZ_m = Monitored::Scalar<float>( "m_PvZ", pvZ);
      fill(pvGroup, pvZ_m);
      
      float pvErrX = Amg::error( vtx->covariancePosition(), Trk::x);
      auto  pvErrX_m = Monitored::Scalar<float>( "m_PvErrX", pvErrX);
      fill(pvGroup, pvErrX_m);
      
      float pvErrY = Amg::error( vtx->covariancePosition(), Trk::y);
      auto  pvErrY_m = Monitored::Scalar<float>( "m_PvErrY", pvErrY);
      fill(pvGroup, pvErrY_m);
      
      float pvErrZ = Amg::error( vtx->covariancePosition(), Trk::z);
      auto  pvErrZ_m = Monitored::Scalar<float>( "m_PvErrZ", pvErrZ);
      fill(pvGroup, pvErrZ_m);
      
      float pvChiSqDoF = vtx->chiSquared() / vtx->numberDoF() ;
      auto  pvChiSqDoF_m = Monitored::Scalar<float>( "m_PvChiSqDoF", pvChiSqDoF);
      fill(pvGroup, pvChiSqDoF_m);
      
      
      auto & trackparticles = vtx->trackParticleLinks();
      
      int pvNTracks = trackparticles.size() ;
      auto  pvNTracks_m = Monitored::Scalar<int>( "m_PvNTracks", pvNTracks);
      fill(pvGroup, pvNTracks_m);
      
      
      // original tracks used for primary vertex
      for (const auto & trackparticle : trackparticles)
      {
	  const Trk::Perigee & measuredPerigee = (*trackparticle)->perigeeParameters();
	  
	  float pvTrackEta = measuredPerigee.eta()  ;
	  auto  pvTrackEta_m = Monitored::Scalar<float>( "m_PvTrackEta", pvTrackEta);
	  fill(pvGroup, pvTrackEta_m);
	  
	  float pvTrackPt = measuredPerigee.pT()/1000. ; // Histo is in GeV
	  auto  pvTrackPt_m = Monitored::Scalar<float>( "m_PvTrackPt", pvTrackPt);
	  fill(pvGroup, pvTrackPt_m);
	  
	}
      
    } // vxContainer
  
  
  auto nPriVtx_m = Monitored::Scalar<int>( "m_nPriVtx", nPriVtx);
  fill(pvGroup, nPriVtx_m);
  
  auto nPileupVtx_m = Monitored::Scalar<int>( "m_nPileupVtx", nPileupVtx);
  fill(pvGroup, nPileupVtx_m);
  
  
  // EnhancedMonitoring is OFF 
  
  
  //*******************************************************************************
  //**************************** End of filling Track Histograms ******************
  //*******************************************************************************
  
  return StatusCode::SUCCESS;
}
