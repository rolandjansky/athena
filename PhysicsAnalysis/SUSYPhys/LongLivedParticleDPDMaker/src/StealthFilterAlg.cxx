/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/StealthFilterAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

/// Standard Service Constructor
StealthFilterAlg::StealthFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator), 
    m_nProcessed(0),
    m_nEventsPassAll(0)
	
{
  declareProperty("TrackParticleContainer", m_trackParticleContainerName = "TrackParticleCandidate");
  declareProperty("cutMinTrackSd0", m_cutMinTrackSd0 = 15.);
  declareProperty("cutMinTrackpT", m_cutMinTrackpT = 2000.);
  declareProperty("cutMinTrackEta", m_cutMinTrackEta = 2.5);
  declareProperty("cutMinNTracks", m_cutMinNTracks = 30);
  declareProperty("passAll", m_passAll = false); 
}

/// Destructor
StealthFilterAlg::~StealthFilterAlg() {}

/// Gaudi Service Interface method implementations:
StatusCode StealthFilterAlg::initialize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  msg(MSG::INFO) << "==> initialize ..." << name() << endreq;
  msg(MSG::INFO)<< "******* StealthFilterAlg"<< name() <<" Configuration **********" << endreq;  
  msg(MSG::INFO)<<"cutMinTrackSd0         "<<         m_cutMinTrackSd0        <<endreq;
  msg(MSG::INFO)<<"cutMinNTracks          "<<         m_cutMinNTracks        <<endreq;
  msg(MSG::INFO)<<"cutMinTrackpT          "<<         m_cutMinTrackpT        <<endreq;
  msg(MSG::INFO)<<"cutMinTrackEta         "<<         m_cutMinTrackEta        <<endreq;
  msg(MSG::INFO)<<"passAll                "<<            m_passAll           <<endreq;

  return sc;
}

StatusCode StealthFilterAlg::execute()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  m_nProcessed += 1;
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "==> execute " << name() << " on " << m_nProcessed << " event..." << endreq;

  //If passAll is selected, accept all events
  if(m_passAll){
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << name() << " event passed because passAll is true" << endreq;
    setFilterPassed(true);
    return sc;
  }
  bool passCut = false;

  //Load tracks
  const Rec::TrackParticleContainer* trackTES1=0;
   sc=evtStore()->retrieve( trackTES1, m_trackParticleContainerName);
   if( sc.isFailure() ) {
     msg(MSG::FATAL) << "Fatal error retrieving TrackParticleContainer " << m_trackParticleContainerName << endreq;
     return sc;
   }
   if ( !trackTES1 ) {
     msg(MSG::ERROR) << "No TrackParticle container " << m_trackParticleContainerName << " found" << endreq;
     return StatusCode::SUCCESS;
   }  
   if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TrackParticleContainer successfully retrieved" << endreq;

   Rec::TrackParticleContainer::const_iterator trackItr1  = trackTES1->begin();
   Rec::TrackParticleContainer::const_iterator trackItrE1 = trackTES1->end();

   int nTracksPassed=0;
  //Iterate over all tracks in event  
  for(unsigned int i=0; trackItr1 != trackItrE1; ++ trackItr1,++i) {

    const Trk::TrackSummary *summary = (*trackItr1)->trackSummary();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "AACC summary pointer track particle " << summary << endreq;

    if( !summary ) continue;

    float theta = (*trackItr1)->measuredPerigee()->parameters()[Trk::theta];
    float qOverPt = (*trackItr1)->measuredPerigee()->parameters()[Trk::qOverP]/sin(theta);
    float pT = (1/qOverPt);
	float eta = -1.0*log(tan(0.5*theta));
    float ApT = fabs(pT);
	float Aeta = fabs(eta);
	   
	float track_d0 = (*trackItr1)->measuredPerigee()->parameters()[Trk::d0];
	const AmgSymMatrix(5)* cov=(*trackItr1)->measuredPerigee()->covariance();
	double err_d0 = Amg::error(*cov,Trk::d0);

	//see if track passes cuts
	 if(Aeta < m_cutMinTrackEta && ApT > m_cutMinTrackpT){	
		float track_Sd0 = track_d0/err_d0;
		if(fabs(track_Sd0) > m_cutMinTrackSd0) nTracksPassed++;
	 }
	}

		////
  
  if(nTracksPassed > m_cutMinNTracks) passCut = true;
  
  if (passCut) {
    setFilterPassed(true);
    m_nEventsPassAll++;
  }
  else setFilterPassed(false);

  return sc;  
}

StatusCode StealthFilterAlg::finalize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);
  
  // Calculate the efficiencies and their errors
  
  // Efficiencies
  double effiPassedAll  = m_nProcessed > 0 ? m_nEventsPassAll / (1.0*m_nProcessed) : 0.0;
  
  // Error on efficiencies
  
  double rootn = m_nProcessed > 0 ? sqrt((double)m_nProcessed) : 0.0;
  double effiErrPassedAll = (rootn > 0 && effiPassedAll >= 0.0 && effiPassedAll <= 1.0)? sqrt(effiPassedAll*(1.0-effiPassedAll)) / rootn : 0.0;
  
  effiPassedAll *= 100.0;
  effiErrPassedAll *= 100.0;

  
  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "==> finalize " << name() << endreq;
    msg(MSG::INFO) << "***************************************************************" << endreq;
    msg(MSG::INFO) << "Cut-flow table of " << name() << " skimming algorithm:" << endreq;
    msg(MSG::INFO) << "-------------" << endreq;
    msg(MSG::INFO) << " Number of processed events: " << m_nProcessed << endreq;
    msg(MSG::INFO) << " Events passed all cuts:  " << m_nEventsPassAll << " and resulting efficiency = (" << effiPassedAll << "+/-" << effiErrPassedAll << ")" << endreq;
    msg(MSG::INFO) << "***************************************************************" << endreq;
  }

  return sc;  
}
