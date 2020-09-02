/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InDetTrigExtensProcessor.cxx
//   Implementation file for class InDetTrigExtensProcessor
///////////////////////////////////////////////////////////////////

#include "InDetTrigExtensProcessor/InDetTrigExtensProcessor.h"

// Gaudi includes
#include "StoreGate/StoreGate.h"

//tools
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"

//
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include <iomanip>
#include <cassert>


// Constructor with parameters:
InDet::InDetTrigExtensProcessor::InDetTrigExtensProcessor(const std::string &name, ISvcLocator *pSvcLocator) :
  HLT::FexAlgo(name,pSvcLocator),
  m_refitPrds(true),
  m_runOutlier(true),
  m_matEffects(4),
  m_suppressHoleSearch(false),
  m_keepFailedExtensionOnTrack(true),
  m_tracks(0),
  m_trackExtensionMap(0),
  m_ITrackFitter("Trk::KalmanFitter/InDetTrigTrackFitter"),
  m_trkSummaryTool("", this),
  m_scoringTool("Trk::TrackScoringTool/InDetTrigScoringTool"),
  m_newtracks(0),
  m_cosmics(false)
{  
  m_ParticleHypothesis = Trk::undefined;  //for coverity, reset in hltInitialize

  // Get parameter values from jobOptions file
  declareProperty("TrackFitter",     m_ITrackFitter);
  declareProperty("SummaryTool" ,    m_trkSummaryTool);
  declareProperty("ScoringTool",     m_scoringTool);
  declareProperty("runOutlier",      m_runOutlier);
  declareProperty("RefitPrds",       m_refitPrds,"switch whether to do the fit with re-calibrated clusters (true) or not");
  declareProperty("matEffects",      m_matEffects);
  declareProperty("suppressHoleSearch", m_suppressHoleSearch,"suppressing hole search for comparison");
  declareProperty("keepFailedExtension",m_keepFailedExtensionOnTrack);
  declareProperty("Cosmics",         m_cosmics, "switch whether we are running on cosmics");

  declareMonitoredVariable("numTracksOut",m_numNewTracks);
  declareMonitoredVariable("numTracksIn",m_numTracks);
  declareMonitoredVariable("numExtenTracks",m_numExtenTracks);
  declareMonitoredVariable("numOrigTracks",m_numOrigTracks);
}

// Initialize method:
HLT::ErrorCode InDet::InDetTrigExtensProcessor::hltInitialize() {

  msg() << MSG::INFO << "InDetTrigExtensProcessor::initialize()" << endmsg;

  // get track fitter tool
  if ( m_ITrackFitter.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_ITrackFitter << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  else{
    msg() << MSG::INFO << "Retrieved tool " << m_ITrackFitter << endmsg;
  }

  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_ParticleHypothesis = particleSwitch.particle[m_matEffects];

  if (!m_trkSummaryTool.empty()) {
     if ( m_trkSummaryTool.retrieve().isFailure()) {
        msg() << MSG::FATAL << "Failed to retrieve tool " << m_scoringTool << endmsg;
        return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
     }
  }
  else {
     m_trkSummaryTool.disable();
  }

  // get scoring tool
  if ( m_scoringTool.retrieve().isFailure() ) {
    msg() << MSG::FATAL << "Failed to retrieve tool " << m_scoringTool << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
  }
  else{
    msg() << MSG::INFO << "Retrieved tool " << m_scoringTool << endmsg;
  }
  

  return HLT::OK;
}

///////////////////////////////////////////////////////////////////
// Execute HLT Algorithm
///////////////////////////////////////////////////////////////////
HLT::ErrorCode InDet::InDetTrigExtensProcessor::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) { 


  int outputLevel = msgLvl();
  
  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " InDetTrigExtensProcessor::execute()" << endmsg;

  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // latest track collection associated to it.
  //----------------------------------------------------------------------

  m_numExtenTracks = -1;
  m_numOrigTracks = -1;
  m_numNewTracks = -1;
  m_numTracks = -1;


  if ( HLT::OK != getFeature(outputTE, m_tracks) ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endmsg;

    return HLT::NAV_ERROR;
  }

  if(m_tracks->size()==0){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Input track collection has 0 size. Algorithm not executed!" << endmsg;
    
    return HLT::OK; 
  }

  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " REGTEST: Retrieved track collection correctly. Size = " << m_tracks->size() << endmsg;

  // get track ExensionMap from the trigger element
  // (the ExtensionMap is saved by the TRT_TrigTrackExtension algorithm):

  if ( HLT::OK != getFeature(outputTE, m_trackExtensionMap, "ExtTracks") ) {
    msg() << MSG::ERROR << " Track extension map collection could not be found " << endmsg;
    
    return HLT::NAV_ERROR;
  }
  if(m_trackExtensionMap->size()==0){
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << " Track extension map has 0 size. Algorithm not executed!" << endmsg;
    
    return HLT::OK; 
  }

  if(outputLevel <= MSG::DEBUG)
    msg() <<MSG::DEBUG <<" REGTEST: TrackExtensionMap found, size:" 
	  << m_trackExtensionMap->size() <<endmsg; 

  m_numExtenTracks = 0;
  m_numOrigTracks = 0;

  // create new collection of tracks to write in storegate
  m_newtracks = new TrackCollection;

  // loop over tracks
  for (TrackCollection::const_iterator itr  = (*m_tracks).begin(); 
       itr < (*m_tracks).end(); itr++){

    // try to find this track in extension map
    TrackExtensionMap::const_iterator itEx = m_trackExtensionMap->find(*itr);

    // copy input track if this track is not found in list
    if (itEx == m_trackExtensionMap->end()) {

      if(outputLevel <= MSG::DEBUG)
	msg() << MSG::DEBUG 
	    << " REGTEST: track not in extension map, copy original track to output" 
	    << endmsg;

      // push track into output, does copy, needs fixing
      m_newtracks->push_back( new Trk::Track(**itr) );
      ++m_numOrigTracks;

    } else {

      Trk::Track* newtrack;
      
      if (m_refitPrds) {
	// create new vector
	std::vector<const Trk::PrepRawData*> vecPrd;
	// insert PrepRawData
	std::vector<const Trk::MeasurementBase*>::const_iterator it    = itEx->second.begin();
	std::vector<const Trk::MeasurementBase*>::const_iterator itend = itEx->second.end();
	for (;it!=itend;++it) {
	  const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (*it);
	  if (!rot) {
	    ATH_MSG_ERROR ("cast to ROT failed, should not happen here !"); 
	  } else {
	    vecPrd.push_back(rot->prepRawData());
	  }
	}  
	
	// we have the PRD Vector, we need to refit the track and see if it is better
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::VERBOSE << "fit track " << itEx->first 
		<< " with PRDs, number: " << vecPrd.size() << endmsg;
	
	if (!m_cosmics) {
	  newtrack = m_ITrackFitter->fit(*(itEx->first),vecPrd,m_runOutlier,m_ParticleHypothesis);
	} else {
	  //cosmics
	  const Trk::Perigee *siliconper=itEx->first->perigeeParameters();
	  std::vector<const Trk::PrepRawData*> vecPrd2;
	  DataVector<const Trk::MeasurementBase>::const_iterator RIOit=itEx->first->measurementsOnTrack()->begin();
          for ( ;RIOit!=itEx->first->measurementsOnTrack()->end();RIOit++) {
	    const Trk::RIO_OnTrack* rot = dynamic_cast <const Trk::RIO_OnTrack*> (*RIOit);
	    if (rot) {
	      vecPrd2.push_back(rot->prepRawData());
	    } else {
	      ATH_MSG_ERROR("Cast to ROT failed");
	    }
          }
	  for (unsigned int i=0;i<vecPrd.size();i++){
            double inprod=(vecPrd[i]->detectorElement()->surface(vecPrd[i]->identify()).center()-siliconper->position()).dot(siliconper->momentum());
            if (inprod<0){
              vecPrd2.insert(vecPrd2.begin(),vecPrd[i]);
            }
            else {
              vecPrd2.push_back(vecPrd[i]);
            }     
          }
	  newtrack=m_ITrackFitter->fit(vecPrd2,*siliconper,m_runOutlier,m_ParticleHypothesis);
	}

	//storing

      } else {
	// we have an extension map with ROTs, we need to refit the track and see if it is better
	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::VERBOSE << "fit track " << itEx->first 
		<< " with ROTs, number: " << itEx->second.size() << endmsg;
	

	if (!m_cosmics) {
	  newtrack = m_ITrackFitter->fit(*(itEx->first),itEx->second,m_runOutlier,m_ParticleHypothesis);
	} else {
	  const Trk::Perigee *siliconper=itEx->first->perigeeParameters();
          Trk::MeasurementSet rotset;
          DataVector<const Trk::MeasurementBase>::const_iterator RIOit=itEx->first->measurementsOnTrack()->begin();
          for ( ;RIOit!=itEx->first->measurementsOnTrack()->end();RIOit++) {
            rotset.push_back(*RIOit);
          }
          for (unsigned int i=0;i<itEx->second.size();i++){
            double inprod=(itEx->second[i]->associatedSurface().center()-siliconper->position()).dot(siliconper->momentum());
            if (inprod<0){
              rotset.insert(rotset.begin(),itEx->second[i]);
            }
            else {
              rotset.push_back(itEx->second[i]);
            }     
          }
          newtrack=m_ITrackFitter->fit(rotset,*siliconper,m_runOutlier,m_ParticleHypothesis);
        }
 

      }

      if (! newtrack) {

	if(outputLevel <= MSG::DEBUG)
	  msg() << MSG::DEBUG 
		<< "refit of extended track failed, copy original track to output" 
		<< endmsg;

	++m_numOrigTracks;
	// push track into output, does copy, needs fixing
	if (m_keepFailedExtensionOnTrack)
	  m_newtracks->push_back( trackPlusExtension( *itr, itEx->second ) );
	else
	  m_newtracks->push_back( new Trk::Track(**itr) );
	
      } else {
	
	// score old and new tool and decide which one to push back
	Trk::TrackScore oldScore = m_scoringTool->score( **itr, m_suppressHoleSearch );
	if (outputLevel <= MSG::DEBUG) msg()<<MSG::DEBUG<<"original track has score: "<<oldScore<<endmsg;
        if (!newtrack->trackSummary() && m_trkSummaryTool.isEnabled()) {
           m_trkSummaryTool->computeAndReplaceTrackSummary(*newtrack,nullptr /** no shared hits */, m_suppressHoleSearch);
        }
	Trk::TrackScore newScore = m_scoringTool->score( *newtrack, m_suppressHoleSearch );
	if (outputLevel <= MSG::DEBUG) msg()<<MSG::DEBUG<<"new track has score     : "<<newScore<<endmsg;

	if (newScore > oldScore) {

	  if (outputLevel <= MSG::DEBUG) msg()<<MSG::DEBUG<<"take extended track, it's better !"<< endmsg;
	  // push track into output
	  m_newtracks->push_back( newtrack );
	  ++m_numExtenTracks;
	  
	} else {
	  
	  // clean up newtrack
	  delete (newtrack);
	  
	  if (outputLevel <= MSG::DEBUG) msg()<<MSG::DEBUG<<"take original track, new one is worse !"<< endmsg;
	  
	  ++m_numOrigTracks;
	  // push track into output, does copy, needs fixing
	  if (m_keepFailedExtensionOnTrack)
	    m_newtracks->push_back( trackPlusExtension( *itr, itEx->second ) );
	  else
	    m_newtracks->push_back(new Trk::Track(**itr));
	}
      }
    }
  }

  std::string collKey = "ExtProcTracks";
  if ( HLT::OK !=  attachFeature(outputTE, m_newtracks, collKey) ) {
    msg() << MSG::ERROR << "Could not attache feature to the TE" << endmsg;
    
    return HLT::NAV_ERROR;
  }

  if(outputLevel <= MSG::DEBUG) 
    msg() << MSG::DEBUG << " Stored " << m_newtracks->size() 
	  << " tracks in SG. Track collection " << collKey << endmsg; 

  if (m_newtracks->size() != m_tracks->size()){
    if(outputLevel <= MSG::DEBUG) 
      msg() << MSG::ERROR << "Lost tracks after extension ??? This is a bug !" << endmsg;
  }

  if (msgLvl() <= MSG::VERBOSE){
    for (size_t i=0; i<m_newtracks->size() ; i++){
      msg() << MSG::VERBOSE << *(m_newtracks->at(i)) << endmsg;
    }
  }
  
  m_numNewTracks = m_newtracks->size();
  m_numTracks = m_tracks->size();

  return HLT::OK;
}
//------------------------------------------------------------------------------
// Finalize method:
HLT::ErrorCode InDet::InDetTrigExtensProcessor::hltFinalize() {

  msg() << MSG::INFO << "InDetTrigExtensProcessor::finalize()" << endmsg;
  
  return HLT::OK;
}
//-----------------------------------------------------------------------------
Trk::Track* InDet::InDetTrigExtensProcessor::trackPlusExtension(const Trk::Track* siTrack,
								const std::vector<const Trk::MeasurementBase*>& extension) const
{
  int outputLevel = msgLvl();

  DataVector<const Trk::TrackStateOnSurface>* extendedTrajectory
    = new DataVector<const Trk::TrackStateOnSurface>;
  	 
  int nSiStates=0, nExtStates=0;

  // copy existing si track as first part to new track - including all track pars since fit does not change
  {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it    = siTrack->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd = siTrack->trackStateOnSurfaces()->end();
    for ( ; it!=itEnd; ++it, ++nSiStates) {
      if (*it){
	Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface((**it));
	extendedTrajectory->push_back( tsos );
      } else 
	msg() << MSG::WARNING << "NULL trackStateOnSurface" << endmsg;
    }
  }
  
  // copy proposed (but failed) extension as second part onto new track - all hits flagged as outliers.
  {
    std::vector<const Trk::MeasurementBase*>::const_iterator it    = extension.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator itEnd = extension.end();
    for (;it!=itEnd;++it, ++nExtStates) {
      if (*it){
	std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
	typePattern.set(Trk::TrackStateOnSurface::Outlier);
	const Trk::TrackStateOnSurface* aState = new Trk::TrackStateOnSurface((*it)->clone(),0,0,0,typePattern);
	extendedTrajectory->push_back( aState );
      } else {
	msg() << MSG::WARNING << "NULL trackStateOnSurface" << endmsg;
      }
    }
  }
  
  Trk::TrackInfo trkinfo(siTrack->info());
  Trk::Track* extTrack = new Trk::Track(trkinfo, // with best wishes from the plagiator...
					extendedTrajectory,
					( siTrack->fitQuality() ? siTrack->fitQuality()->clone() : 0 )
					);
  if (outputLevel <= MSG::DEBUG) msg() << MSG::VERBOSE << "rejected extension saved as Trk::Track with " << nSiStates
			     << " fitted hits and " << nExtStates << " additional Outliers." << endmsg;
  return extTrack;
}
