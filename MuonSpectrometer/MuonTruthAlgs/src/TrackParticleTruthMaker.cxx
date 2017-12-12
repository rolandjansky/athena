/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleTruthMaker.cxx
//   Implementation file for class TrackParticleTruthMaker
///////////////////////////////////////////////////////////////////

#include "MuonTruthAlgs/TrackParticleTruthMaker.h"

#include "Particle/TrackParticleContainer.h"
#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/TruthTrajectory.h"
#include "HepMC/GenParticle.h"

#include <map>
#include "AthLinks/ElementLink.h"

namespace Muon {

  // Constructor with parameters:
  TrackParticleTruthMaker::TrackParticleTruthMaker(const std::string &name, 
						   ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_trackParticlesName          ("MooreTrackParticles"),  // input track particles
    m_trackParticleTruthCollection("MooreTrackParticlesTruth"), // output container
    m_tracksName                  ("MooreTracks"),  // input tracks
    m_tracksTruthName             ("MooreTracksTruth")  // input track truth
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
    StatusCode sc;

    ///
    const TrackCollection* myTracks(0); 
    sc = evtStore()->retrieve(myTracks, m_tracksName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ("Could not retrieve '" << m_tracksName << "' from StoreGate !");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG ("Retrieved '" << m_tracksName << "' from StoreGate !");
    }
    ///
  
    const DetailedTrackTruthCollection * simTrackMap(0);
  
    // retrieve associated TrackTruth from StoreGate (Track*, TrackTruth)
    sc = evtStore()->retrieve(simTrackMap, m_tracksTruthName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ("Could not retrieve '" << m_tracksTruthName << "' from StoreGate !");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG ("Retrieved '" << m_tracksTruthName << "' from StoreGate");
    }
  
    const Rec::TrackParticleContainer * origTrackPC(0);
    sc = evtStore()->retrieve(origTrackPC, m_trackParticlesName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ("Could not retrieve '" << m_trackParticlesName << "' from StoreGate !");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG ("Retrieve '" << m_trackParticlesName << "' from StoreGate ! " << origTrackPC->size() );
    }
  
    TrackParticleTruthCollection* tpTruthColl = new TrackParticleTruthCollection(origTrackPC);
  
    typedef Rec::TrackParticleContainer::const_iterator TPCIter;
    for (TPCIter i=origTrackPC->begin(); i!=origTrackPC->end(); i++) {
      const Trk::Track* track = (*i)->originalTrack();
      ElementLink<TrackCollection> tracklink;
      tracklink.setElement(const_cast<Trk::Track*>(track));
      tracklink.setStorableObject(*myTracks); // otherwise the find does not work
    
    
      // this should work since both DetailedTrackTruthCollection and TrackTruthCollection use the same kay: Trk::TrackTruthKey
      DetailedTrackTruthCollection::const_iterator tempTrackTruthItr = simTrackMap->find(tracklink);  
      if (tempTrackTruthItr != simTrackMap->end()) {
	for (unsigned int j=0; j< (*tempTrackTruthItr).second.trajectory().size(); j++) {
	  if(!(*tempTrackTruthItr).second.trajectory().at(j).cptr()) continue;
	  ATH_MSG_DEBUG ("Barcode: " << (*tempTrackTruthItr).second.trajectory().at(j).cptr()->pdg_id() 
			   << "Status: "  <<  (*tempTrackTruthItr).second.trajectory().at(j).cptr()->status() << "\t Probability: " << 1.);
	  ElementLink< Rec::TrackParticleContainer > myLink;
	  myLink.setElement(const_cast<Rec::TrackParticle*>(*i));
	  myLink.setStorableObject(*origTrackPC);
	  std::pair< ElementLink< Rec::TrackParticleContainer > , TrackParticleTruth> 
	    entry(myLink,TrackParticleTruth((HepMcParticleLink)(*tempTrackTruthItr).second.trajectory().at(j),1.));
	  tpTruthColl->insert(entry);
	}
      }else{
	ATH_MSG_DEBUG(" no truth track found " );
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
    return StatusCode::SUCCESS;
  }

} // namespace Muon

