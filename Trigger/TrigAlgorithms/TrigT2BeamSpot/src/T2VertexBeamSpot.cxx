/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//============================================================
//
// T2VertexBeamSpot.cxx, (c) ATLAS Detector software
// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
//
// Online beam spot measurement and monitoring using
// L2 recontructed primary vertices.
//
// Authors : David W. Miller, Rainer Bartoldus,   
//           Su Dong
//============================================================

#include "T2VertexBeamSpot.h"

// General ATHENA/Trigger stuff
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoringKernel/Monitored.h"

using namespace PESA;

// Constructor
T2VertexBeamSpot::T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm(name, pSvcLocator)
{
   declareProperty("doTrackBeamSpot",     m_doTrackBeamSpot  = true);
}

StatusCode T2VertexBeamSpot::initialize() {

   ATH_CHECK(m_beamSpotTool.retrieve());
   ATH_CHECK(m_trackBSTool.retrieve());
   ATH_CHECK(m_monTool.retrieve());

   ATH_CHECK( m_trackCollectionKey.initialize() );

   return StatusCode::SUCCESS;
}

StatusCode T2VertexBeamSpot::execute(const EventContext& ctx) const{
   // Start the overall timer
   auto tTotal = Monitored::Timer("TIME_TotalTime");

   // Retrieve tracks
   SG::ReadHandle<TrackCollection> trackCollection(m_trackCollectionKey, ctx);
   ATH_CHECK(trackCollection.isValid());
   const TrackCollection* tracks = trackCollection.cptr();

   // call track-based tool
   if (m_doTrackBeamSpot) {
      m_trackBSTool->updateBS(*tracks, ctx.eventID());
   }

   // call vertex-based tool
   m_beamSpotTool->updateBS(*tracks, ctx);

   //What should go as an output? SelectedTrackCollection and Vertices?
   //Atm just try add vertex
   //TODO: adding split vertices as well, will need an array

  auto mon = Monitored::Group(m_monTool, tTotal);

   return StatusCode::SUCCESS;
} 
