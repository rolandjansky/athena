/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/** @file InDetGlobalPrimaryVertexMonAlg.h
 * Implementation of inner detector global Primary Vertex monitoring tool
 *
 *@author
 * Leonid Serkin <lserkin@cern.ch> @n
 *
 * based on InDetGlobalPrimaryVertexMonTool.cxx
 *
 ****************************************************************************/

//main header
#include "InDetGlobalMonitoringRun3Test/InDetGlobalPrimaryVertexMonAlg.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"


//Root
#include "TMath.h"

//Standard c++
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <functional>


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
  auto vxContainer = SG::makeHandle(m_vxContainerName, ctx);   // another way to access ??
  
  if (!vxContainer.isPresent()) {
    ATH_MSG_DEBUG ("InDetGlobalPrimaryVertexMonAlg: StoreGate doesn't contain primary vertex container with key "+m_vxContainerName.key());
    return StatusCode::SUCCESS;
  }
  if (!vxContainer.isValid()) {
    ATH_MSG_ERROR ("InDetGlobalPrimaryVertexMonAlg: Could not retrieve primary vertex container with key "+m_vxContainerName.key());
    return StatusCode::RECOVERABLE;
  }
  
  
  // Total number of vertices (primary and pile up)
  int PvN = vxContainer->size()-1;  // exclude dummy vertex
  auto PvN_m = Monitored::Scalar<int>( "m_PvN", PvN);
  fill(pvGroup, PvN_m);
  
  int nPriVtx = 0;
  int nPileupVtx = 0;
  
  for (xAOD::VertexContainer::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter) 
    {
      
      // Count different types of vertices
      if ((*vxIter)->vertexType() == xAOD::VxType::PriVtx) nPriVtx++;
      if ((*vxIter)->vertexType() == xAOD::VxType::PileUp) nPileupVtx++;
      
      
      // Select primary vertex
      if ((*vxIter)->vertexType() != xAOD::VxType::PriVtx) continue;
      if ((*vxIter)->numberDoF() <= 0) continue;
      
      float PvX = (*vxIter)->position().x();
      auto  PvX_m = Monitored::Scalar<float>( "m_PvX", PvX);
      fill(pvGroup, PvX_m);
      
      float PvY = (*vxIter)->position().y();
      auto  PvY_m = Monitored::Scalar<float>( "m_PvY", PvY);
      fill(pvGroup, PvY_m);
      
      float PvZ = (*vxIter)->position().z();
      auto  PvZ_m = Monitored::Scalar<float>( "m_PvZ", PvZ);
      fill(pvGroup, PvZ_m);
      
      float PvErrX = Amg::error( (*vxIter)->covariancePosition(), Trk::x);
      auto  PvErrX_m = Monitored::Scalar<float>( "m_PvErrX", PvErrX);
      fill(pvGroup, PvErrX_m);
      
      float PvErrY = Amg::error( (*vxIter)->covariancePosition(), Trk::y);
      auto  PvErrY_m = Monitored::Scalar<float>( "m_PvErrY", PvErrY);
      fill(pvGroup, PvErrY_m);
      
      float PvErrZ = Amg::error( (*vxIter)->covariancePosition(), Trk::z);
      auto  PvErrZ_m = Monitored::Scalar<float>( "m_PvErrZ", PvErrZ);
      fill(pvGroup, PvErrZ_m);
      
      float PvChiSqDoF = (*vxIter)->chiSquared() / (*vxIter)->numberDoF() ;
      auto  PvChiSqDoF_m = Monitored::Scalar<float>( "m_PvChiSqDoF", PvChiSqDoF);
      fill(pvGroup, PvChiSqDoF_m);
      
      
      auto & trackparticles = (*vxIter)->trackParticleLinks();
      
      int PvNTracks = trackparticles.size() ;
      auto  PvNTracks_m = Monitored::Scalar<int>( "m_PvNTracks", PvNTracks);
      fill(pvGroup, PvNTracks_m);
      
      
      // original tracks used for primary vertex
      for (auto trackparticle  : trackparticles )
	{
	  const Trk::Perigee & measuredPerigee = (*trackparticle)->perigeeParameters();
	  
	  float PvTrackEta = measuredPerigee.eta()  ;
	  auto  PvTrackEta_m = Monitored::Scalar<float>( "m_PvTrackEta", PvTrackEta);
	  fill(pvGroup, PvTrackEta_m);
	  
	  float PvTrackPt = measuredPerigee.pT()/1000. ; // Histo is in GeV
	  auto  PvTrackPt_m = Monitored::Scalar<float>( "m_PvTrackPt", PvTrackPt);
	  fill(pvGroup, PvTrackPt_m);
	  
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
