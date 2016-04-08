/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleTruthMaker.cxx
//   Implementation file for class TrackParticleTruthMaker
///////////////////////////////////////////////////////////////////

#include "InDetTruthAlgs/TrackParticleTruthMaker.h"

#include "Particle/TrackParticleContainer.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "TrkTrack/TrackCollection.h"

#include <map>
#include "DataModel/ElementLink.h"

namespace InDet {

// Constructor with parameters:
TrackParticleTruthMaker::TrackParticleTruthMaker(const std::string &name, 
				   ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_trackParticlesName          ("TrackParticleCandidate"),
  m_trackParticleTruthCollection("TrackParticleTruthCollection"),
  m_tracksName                  ("Tracks"),
  m_tracksTruthName             ("TrackTruthCollection")
{  
  // Get parameter values from jobOptions file
  declareProperty("trackParticlesName"           , m_trackParticlesName);
  declareProperty("trackParticleTruthCollection" , m_trackParticleTruthCollection);
  declareProperty("tracksName"                   , m_tracksName);
  declareProperty("tracksTruthName"              , m_tracksTruthName);
}

// Initialize method:
StatusCode TrackParticleTruthMaker::initialize()
{
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode TrackParticleTruthMaker::execute() 
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG ("TrackParticleTruthMaker::execute()");

  // first check for the truth container, sometimes it's missing
  if( ! evtStore()->contains< TrackTruthCollection >( m_tracksTruthName ) ) {
    ATH_MSG_DEBUG ("Could not retrieve '" << m_tracksTruthName << "' from StoreGate !");
    return StatusCode::SUCCESS;
  }

  // retrieve associated TrackTruth from StoreGate (Track*, TrackTruth)
  const TrackTruthCollection * simTrackMap(0);
  StatusCode sc = evtStore()->retrieve(simTrackMap, m_tracksTruthName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG ("Could not retrieve '" << m_tracksTruthName << "' from StoreGate !");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Retrieved '" << m_tracksTruthName << "' from StoreGate");
  }

  ///
  const TrackCollection* myTracks(0); 
  sc = evtStore()->retrieve(myTracks, m_tracksName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG ("Could not retrieve '" << m_tracksName << "' from StoreGate !");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Retrieved '" << m_tracksName << "' from StoreGate !");
  }
  ///
  
  const Rec::TrackParticleContainer * origTrackPC(0);
  sc = evtStore()->retrieve(origTrackPC, m_trackParticlesName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG ("Could not retrieve '" << m_trackParticlesName << "' from StoreGate !");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG ("Retrieve '" << m_trackParticlesName << "' from StoreGate !");
  }
  
  TrackParticleTruthCollection* tpTruthColl = new TrackParticleTruthCollection(origTrackPC);
  
  typedef Rec::TrackParticleContainer::const_iterator TPCIter;
  for (TPCIter i=origTrackPC->begin(); i!=origTrackPC->end(); i++) {
    const Trk::Track* track = (*i)->originalTrack();
    ElementLink<TrackCollection> tracklink;
    tracklink.setElement(const_cast<Trk::Track*>(track));
    tracklink.setStorableObject(*myTracks); // otherwise the find does not work
    
    TrackTruthCollection::const_iterator tempTrackTruthItr = simTrackMap->find(tracklink);  
    if (tempTrackTruthItr != simTrackMap->end()) {    
	ATH_MSG_VERBOSE ("Barcode: " << (*tempTrackTruthItr).second.particleLink().barcode() <<
			 "\t Probability: " << (*tempTrackTruthItr).second.probability());
	ElementLink< Rec::TrackParticleContainer > myLink;
	myLink.setElement(const_cast<Rec::TrackParticle*>(*i));
	myLink.setStorableObject(*origTrackPC);
	std::pair< ElementLink< Rec::TrackParticleContainer > , TrackParticleTruth>
	  entry(myLink,TrackParticleTruth((*tempTrackTruthItr).second.particleLink(),(*tempTrackTruthItr).second.probability()));
	tpTruthColl->insert(entry);
    }
  } 
  
  // record associated TrackParticleTruth to StoreGate (TrackParticle*, TrackParticleTruth)
  sc = evtStore()->record(tpTruthColl, m_trackParticleTruthCollection, false);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ("Could not record '" << m_trackParticleTruthCollection << "' to StoreGate !");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ("Recorded '" << m_trackParticleTruthCollection << "' to StoreGate !");
  }
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode TrackParticleTruthMaker::finalize() 
{
  ATH_MSG_INFO ("TrackParticleTruthMaker::finalize()");
  return StatusCode::SUCCESS;
}

} // namespace InDet

