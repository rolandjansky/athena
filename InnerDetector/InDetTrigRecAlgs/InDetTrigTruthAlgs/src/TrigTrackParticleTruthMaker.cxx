/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrigTrackParticleTruthMaker.cxx
//   class TrackParticleTruthMaker adopted to the trigger
// author Iwona Grabowska-Bold Iwona.Grabowska@cern.ch
///////////////////////////////////////////////////////////////////

#include "InDetTrigTruthAlgs/TrigTrackParticleTruthMaker.h"

#include "Particle/TrackParticleContainer.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "TrkTrack/TrackCollection.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"

#include <map>
#include "AthLinks/ElementLink.h"

namespace InDet {

// Constructor with parameters:
TrigTrackParticleTruthMaker::TrigTrackParticleTruthMaker(const std::string 
							 &name, ISvcLocator *pSvcLocator) :
  HLT::FexAlgo(name,pSvcLocator),
  m_doTruth(false)
{  
  // Get parameter values from jobOptions file
  declareProperty("doTruthAss",   m_doTruth);

  declareMonitoredVariable("numTracks", m_numTracks   );
  declareMonitoredVariable("numTracksTruth", m_numTracksTruth   );
  declareMonitoredVariable("numParticleTracks", m_numParticleTracks   );
  declareMonitoredVariable("numParticleTracksTruth", m_numParticleTracksTruth   );

}

//--------------------------------------------------------------------------
// Initialize method:
HLT::ErrorCode TrigTrackParticleTruthMaker::hltInitialize() {

  if(!m_doTruth) {
    msg() << MSG::INFO << "TrigTrackParticleTruthMaker is skipped: doTruth = False" << endmsg;
    return HLT::OK;
  }

  msg() << MSG::INFO << "TrigTrackParticleTruthMaker::initialize()" << endmsg;


  msg() << MSG::INFO << "Initialization successful" << endmsg;
  
  return HLT::OK;
}

//----------------------------------------------------------------------------
HLT::ErrorCode TrigTrackParticleTruthMaker::hltExecute(const HLT::TriggerElement*,
						       HLT::TriggerElement* outputTE) {
  int outputLevel = msgLvl();

  m_numTracks = 0;
  m_numTracksTruth = 0;
  m_numParticleTracks = 0;
  m_numParticleTracksTruth = 0;

  if(!m_doTruth) {
    if(outputLevel <= MSG::DEBUG)
      msg() << MSG::DEBUG << "TrigTrackParticleTruthMaker is skipped: doTruth = False" << endmsg;
    return HLT::OK;
  }


  if(outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << " In execHLTAlgorithm()" << endmsg;
  

  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // input track collection.
  //----------------------------------------------------------------------

  const TrackCollection* myTracks(0);
 
  if ( HLT::OK != getFeature(outputTE, myTracks) ) {
    msg() << MSG::ERROR << " Input track collection could not be found " << endmsg;
    return HLT::NAV_ERROR;
  }

  if ( !myTracks ) {
    msg() << MSG::DEBUG << "No Input track collection present" << endmsg; 
    return HLT::OK; 
  }

  m_numTracks = myTracks->size();
  if (outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST: Retrieved input track collection with "
          << m_numTracks << " tracks. " << endmsg;

  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // input track truth collection.
  //----------------------------------------------------------------------

  const TrackTruthCollection* simTrackMap(0);

  if ( HLT::OK != getFeature(outputTE, simTrackMap) && !simTrackMap) {
    msg() << MSG::ERROR << "Failed to get TrackTruthCollections from the trigger element" << endmsg;
    return HLT::NAV_ERROR;
  }

  if ( !simTrackMap ) {
    msg() << MSG::DEBUG << "No TrackTruthCollections present" << endmsg; 
    return HLT::OK; 
  }
  
  m_numTracksTruth = simTrackMap->size();
  if (outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST: Retrieved input track truth collection with "
	  << m_numTracksTruth << " tracks. " << endmsg;
  
  //----------------------------------------------------------------------
  // Trigger specific part: navigate throw the trigger element to get the
  // input track container.
  //----------------------------------------------------------------------
  const Rec::TrackParticleContainer* origTrackPC(0);

  if ( HLT::OK != getFeature(outputTE, origTrackPC)) {
    msg() << MSG::ERROR << "Failed to get TrackContainer from the trigger element" << endmsg; 
    return HLT::NAV_ERROR;
  }
  
  if ( !origTrackPC ) {
    msg() << MSG::DEBUG << "No TrackContainer present" << endmsg; 
    return HLT::OK; 
  }

  m_numParticleTracks =  origTrackPC->size();
  if (outputLevel <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST: Retrieved input track particle container with "  
	  << m_numParticleTracks << " tracks. " << endmsg;

  TrackParticleTruthCollection* tpTruthColl = new TrackParticleTruthCollection;
  
  typedef Rec::TrackParticleContainer::const_iterator TPCIter;
  for (TPCIter i=origTrackPC->begin(); i!=origTrackPC->end(); i++) {
    const Trk::Track* track = (*i)->originalTrack();
    ElementLink<TrackCollection> tracklink;
    tracklink.setElement(const_cast<Trk::Track*>(track));
    tracklink.setStorableObject(*myTracks); // otherwise the find does not work
    
    TrackTruthCollection::const_iterator
      tempTrackTruthItr = simTrackMap->find(tracklink);  
    if (tempTrackTruthItr != simTrackMap->end()) 
      {    
	msg() << MSG::VERBOSE << "Barcode: " << (*tempTrackTruthItr).second.particleLink().barcode()
	      << "\t Probability: " << (*tempTrackTruthItr).second.probability() << endmsg;
	ElementLink< Rec::TrackParticleContainer > myLink;
	myLink.setElement(const_cast<Rec::TrackParticle*>(*i));
	myLink.setStorableObject(*origTrackPC);
	std::pair< ElementLink< Rec::TrackParticleContainer > , TrackParticleTruth>
	  entry(myLink,TrackParticleTruth((*tempTrackTruthItr).second.particleLink(),(*tempTrackTruthItr).second.probability()));
	tpTruthColl->insert(entry);
      }
  } 

  if ( HLT::OK !=  attachFeature(outputTE, tpTruthColl, name() ) ) {
    msg() << MSG::ERROR << "Could not attache feature to the TE" << endmsg;
  }
  
  m_numParticleTracksTruth = tpTruthColl->size();
  msg() << MSG::DEBUG << "REGTEST: Container TrackParticleTruth has size :" 
	<< m_numParticleTracksTruth << endmsg;

  return HLT::OK;
}
//------------------------------------------------------------------------
// Finalize method:
HLT::ErrorCode TrigTrackParticleTruthMaker::hltFinalize() {

  if(!m_doTruth) {
    msg() << MSG::INFO << "TrigTrackParticleTruthMaker is skipped: doTruth = False" << endmsg;
    return HLT::OK;
  }

  // Get the messaging service, print where you are
  msg() << MSG::INFO << "TrigTrackParticleTruthMaker::finalize()" << endmsg;
  return HLT::OK;
}

}//end of namespace
//-------------------------------------------------------------------------
