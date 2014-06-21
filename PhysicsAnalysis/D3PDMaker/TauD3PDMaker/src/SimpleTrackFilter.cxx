/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleTrackFilter.h"
#include "Particle/TrackParticle.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "Particle/TrackParticleContainer.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"
#include "DataModel/ConstDataVector.h"

SimpleTrackFilter::SimpleTrackFilter(const std::string& name, 
				     ISvcLocator* pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator), 
    m_tracksName("TrackParticleCandidate"),
    m_outputTracksName("TauSelectedTracks"),
    m_ptCut(500.),
    m_nSCTPixHitsCut(6)
{ 
  // string property for Track container 
  declareProperty("TracksName",m_tracksName);
  // string property for Track container 
  declareProperty("OutputTracksName",m_outputTracksName);
  // float minimum track pt threshold
  declareProperty("ptCut",m_ptCut);
  // string property for Track container 
  declareProperty("nSCTPixHitsCut",m_nSCTPixHitsCut);
}

SimpleTrackFilter::~SimpleTrackFilter()
{ }

StatusCode SimpleTrackFilter::initialize()
{
  return StatusCode::SUCCESS;    
}


StatusCode SimpleTrackFilter::finalize()
{
 return StatusCode::SUCCESS;
}

///////////////
StatusCode SimpleTrackFilter::execute()
{
  ATH_MSG_DEBUG(" in execute");

  typedef Rec::TrackParticleContainer CONTAINER; 

  const CONTAINER* trkcoll;

  StatusCode sc = evtStore()->retrieve(trkcoll, m_tracksName); 

  if(StatusCode::SUCCESS != sc ) 
    {
      REPORT_MESSAGE(MSG::ERROR) << " unable to retrieve TrackParticle container: "<< m_tracksName; 
      return sc;
    }
  
  
  ConstDataVector<CONTAINER>* outputCont =
    new ConstDataVector<CONTAINER>(SG::VIEW_ELEMENTS) ;
  sc = evtStore()->record(outputCont, m_outputTracksName) ; 
  
  if(StatusCode::SUCCESS != sc) 
    {
      REPORT_MESSAGE(MSG::ERROR) << " unable to record Tracks container: "<< m_outputTracksName;
      return sc;
    }
  
  CONTAINER::const_iterator f_trk = trkcoll->begin();
  CONTAINER::const_iterator l_trk = trkcoll->end();
  
  for ( ; f_trk!=l_trk; ++f_trk)
    {
	const Rec::TrackParticle* track = (*f_trk);

	if(!track) continue;
	
	const Trk::Perigee* perigee = track->measuredPerigee();
	if (!perigee)
	    return StatusCode::SUCCESS;       

	const Trk::TrackSummary* summary = track->trackSummary();
	if (!summary)
	    return StatusCode::SUCCESS;       

	int numberOfSCTHits  = summary->get(Trk::numberOfSCTHits);
	int numberOfSCTHoles = summary->get(Trk::numberOfSCTDeadSensors);
	
	int numberOfPixelHits  = summary->get(Trk::numberOfPixelHits);
	int numberOfPixelHoles = summary->get(Trk::numberOfPixelDeadSensors);
	
	float pt = perigee->pT();
	
	
	if ((pt > m_ptCut) && ((numberOfSCTHits + numberOfPixelHits + numberOfSCTHoles + numberOfPixelHoles) > m_nSCTPixHitsCut))
	    outputCont->push_back(track); 
    }
  
  return sc;
}
