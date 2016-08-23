/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EventDataModelMonitor.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "TrkValAlgs/EventDataModelMonitor.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
// Trk
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSegment/Segment.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "VxVertex/Vertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxCandidate.h"

Trk::EventDataModelMonitor::EventDataModelMonitor(const std::string& name, ISvcLocator* pSvcLocator):
        AthAlgorithm(name,pSvcLocator)
{
}

Trk::EventDataModelMonitor::~EventDataModelMonitor() {

#ifndef NDEBUG
    ATH_MSG_INFO( "EventDataModelMonitor AFTER finalize of algorithms (dbg only):" );
    ATH_MSG_INFO( "left over all Surfaces    : \t"<<Trk::Surface::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over free Surfaces   : \t"<<Trk::Surface::numberOfFreeInstantiations() ); 
    ATH_MSG_INFO( "left over TrackParameters : \t"<<Trk::TrackParameters::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over PrepRawDatas    : \t"<<Trk::PrepRawData::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over RIO_OnTracks    : \t"<<Trk::RIO_OnTrack::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over SpacePoints     : \t"<<Trk::SpacePoint::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over Segments        : \t"<<Trk::Segment::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over Tracks          : \t"<<Trk::Track::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over TrackSummaries  : \t"<<Trk::TrackSummary::numberOfInstantiations() ); 
    ATH_MSG_INFO( "left over VxTrackAtVertices : \t"<<Trk::VxTrackAtVertex::numberOfInstantiations() );
    ATH_MSG_INFO( "left over Trk::Vertex     : \t"<<Trk::Vertex::numberOfInstantiations() );
    ATH_MSG_INFO( "left over VxCandidates    : \t"<<Trk::VxCandidate::numberOfInstantiations() );
    ATH_MSG_INFO( "==> this may still contain the ID geometry and last event, (srv finalize comes now)" );

    if (Trk::TrackParameters::numberOfInstantiations() || Trk::RIO_OnTrack::numberOfInstantiations() || Trk::PrepRawData::numberOfInstantiations() 
       || Trk::SpacePoint::numberOfInstantiations() || Trk::Segment::numberOfInstantiations() || Trk::Track::numberOfInstantiations() 
       || Trk::TrackSummary::numberOfInstantiations() || Trk::VxTrackAtVertex::numberOfInstantiations()
       || Trk::Vertex::numberOfInstantiations() > 2 || Trk::VxCandidate::numberOfInstantiations() ) 
       ATH_MSG_WARNING("Leaks detected!");

#endif  

}

StatusCode Trk::EventDataModelMonitor::initialize() {
    ATH_MSG_INFO("EventDataModelMonitor initialize()" );
    return StatusCode::SUCCESS;
}

StatusCode Trk::EventDataModelMonitor::execute() {
#ifndef NDEBUG
    ATH_MSG_INFO( "EventDataModelMonitor at start of event (dbg only):" );
    ATH_MSG_INFO( "number of all Surfaces    (about const) : \t"<<Trk::Surface::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of free Surfaces   (about const) : \t"<<Trk::Surface::numberOfFreeInstantiations() ); 
    ATH_MSG_INFO( "number of TrackParameters (should be 0) : \t"<<Trk::TrackParameters::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of PrepRawData     (about const) : \t"<<Trk::PrepRawData::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of RIO_OnTrack     (should be 0) : \t"<<Trk::RIO_OnTrack::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of SpacePoints     (about const) : \t"<<Trk::SpacePoint::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of Segments        (should be 0) : \t"<<Trk::Segment::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of Tracks          (should be 0) : \t"<<Trk::Track::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of TrackSummaries  (should be 0) : \t"<<Trk::TrackSummary::numberOfInstantiations() ); 
    ATH_MSG_INFO( "number of VxTrackAtVertices (should be 0) : \t"<<Trk::VxTrackAtVertex::numberOfInstantiations() );
    ATH_MSG_INFO( "number of Trk::Vertex     (should be < 3) : \t"<<Trk::Vertex::numberOfInstantiations() );
    ATH_MSG_INFO( "number of VxCandidates    (should be 0) : \t"<<Trk::VxCandidate::numberOfInstantiations() );
#endif

    return StatusCode::SUCCESS;
}

StatusCode Trk::EventDataModelMonitor::finalize() {
    return StatusCode::SUCCESS;
}

