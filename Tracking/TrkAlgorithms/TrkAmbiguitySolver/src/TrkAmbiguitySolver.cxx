/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkAmbiguitySolver/TrkAmbiguitySolver.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "StoreGate/StoreGateSvc.h"

Trk::TrkAmbiguitySolver::TrkAmbiguitySolver(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm (name, pSvcLocator),
  m_resolvedTracksLocation("Tracks"),
  m_resolveTracks(true), 
  m_oldTracks(SG::VIEW_ELEMENTS), // must be view or will delete tracks when cleared.
  m_tracks(nullptr),
  m_trackInCount(0),   
  m_trackOutCount(0),
  m_ambiTool("Trk::SimpleAmbiguityProcessorTool/TrkAmbiguityProcessor")
{  
  declareProperty("TrackInput"        , m_tracksLocation);
  declareProperty("TrackOutput"       , m_resolvedTracksLocation);
  declareProperty("AmbiguityProcessor", m_ambiTool);
  declareProperty("ResolveTracks"     , m_resolveTracks);
}

//--------------------------------------------------------------------------
Trk::TrkAmbiguitySolver::~TrkAmbiguitySolver(void)
{}

//-----------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::initialize()
{
  ATH_MSG_INFO( "TrkAmbiguitySolver::initialize(). " );

  if (!m_resolveTracks)
    ATH_MSG_INFO( "ATTENTION: Resolving tracks turned off! " );
  else {
    // Get Tools sevices
    if (m_ambiTool.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_ambiTool << endmsg;
      return StatusCode::FAILURE;
    } else
      ATH_MSG_INFO( "Retrieved tool " << m_ambiTool );
  }
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
StatusCode
Trk::TrkAmbiguitySolver::execute()
{
  ATH_MSG_VERBOSE ("TrkAmbiguitySolver::execute()");

  StatusCode loadedTracks = getTracks();

  if ( loadedTracks.isFailure() )
    {
      msg(MSG::ERROR) << "TrkAmbiguitySolver could not find any tracks. Aborting." << endmsg;
      return StatusCode::SUCCESS;
    }
	
  // Now pass tracks to AmbiTool
  resolveTracks();
  saveTracks();
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
Trk::TrkAmbiguitySolver::finalize()
{
  m_ambiTool->statistics();

  ATH_MSG_INFO( "Finalizing with "<< m_trackInCount << " tracks input, and "<< m_trackOutCount<< " output");

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------


StatusCode Trk::TrkAmbiguitySolver::getTracks(){

  m_oldTracks.clear();  
  
  std::vector<std::string>::const_iterator it    = m_tracksLocation.begin();  
  std::vector<std::string>::const_iterator itEnd = m_tracksLocation.end();
  for ( ; it!=itEnd ; it++)
    {
      const TrackCollection * tmpTracks=0;    
      StatusCode sc = sgSvc()->retrieve(tmpTracks, *it);// load tracks    
      if (sc.isFailure())    
	msg(MSG::WARNING) << "Could not retrieve tracks from "<< *it << endmsg;    
      else
	{      
	  ATH_MSG_VERBOSE ("Successfully retrieved "<<tmpTracks->size()
			   <<" tracks from "<< *it);      
	  copy(tmpTracks->begin(),tmpTracks->end(), std::back_inserter(m_oldTracks));    
	}  
    }
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------------------------------

void Trk::TrkAmbiguitySolver::saveTracks() const
{
  StatusCode sc = sgSvc()->record(m_tracks, m_resolvedTracksLocation,false);

  if (sc.isFailure())
    msg(MSG::ERROR) << "Could not record tracks" << endmsg;
  else
    ATH_MSG_VERBOSE ("Saved "<<m_tracks->size()<<" tracks");
  
  return;
}

//-------------------------------------------------------------------------------

void Trk::TrkAmbiguitySolver::resolveTracks()
{ 
  if (m_resolveTracks)
    {
      // okay, and let's call the ambiguity processor, just for a laugh.
      ATH_MSG_VERBOSE ("TrkAmbiguitySolver::resolveTracks() resolving "
		      << m_oldTracks.size()<<"  tracks" );
      m_trackInCount += m_oldTracks.size();
      m_tracks = m_ambiTool->process( m_oldTracks.asDataVector() );
      m_trackOutCount += m_tracks->size(); 
    }
  else
    {
      // copy tracks. NOT a shallow copy as it was before, since
      // this causes mem leaks when reading back in again
      // see bug#9886 for details
      m_tracks = new TrackCollection;
      TrackCollection::const_iterator it = m_oldTracks.begin();
      TrackCollection::const_iterator itEnd = m_oldTracks.end();
      for (;it!=itEnd;it++) m_tracks->push_back( new Track(**it) );
    }
  return;
}
