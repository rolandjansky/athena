/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthAlgs/TrackRecordFilter.h"

// the TrackRecordCollection

#include "TrackRecord/TrackRecordCollection.h"

// particle prop service
#include "GaudiKernel/IPartPropSvc.h"    

// Particle data table
#include "HepPDT/ParticleData.hh"
#include "CLHEP/Units/SystemOfUnits.h"

//StoreGate
#include "StoreGate/StoreGateSvc.h"

TrackRecordFilter::TrackRecordFilter(const std::string& name,
  ISvcLocator* pSvcLocator): Algorithm(name, pSvcLocator)
{
  declareProperty("inputName", m_inputName="MuonEntryLayer");
  declareProperty("outputName",m_outputName="MuonEntryLayerFilter");
  declareProperty("threshold",m_cutOff=100.*CLHEP::MeV);
}



StatusCode TrackRecordFilter::initialize() {

  MsgStream log(msgSvc(), name());
  // Get StoreGate service
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure())  {
    log << MSG::FATAL << "StoreGate service not found!" << endreq;
    return StatusCode::FAILURE;
  }
  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc = 0;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    log << MSG::ERROR << " Could not initialize Particle Properties Service" << endreq;
    return PartPropStatus;
  }      
  m_pParticleTable = p_PartPropSvc->PDT();

   return StatusCode::SUCCESS;
}

StatusCode TrackRecordFilter::execute() {

    // Get message service
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "TrackRecordFilter::execute()" << endreq;


   // retrieve the collection
  const TrackRecordCollection* trackCollection(0);
  if (m_storeGate->contains<TrackRecordCollection>(m_inputName)) {
    if (StatusCode::SUCCESS != m_storeGate->retrieve(trackCollection, m_inputName) ) {
      log << MSG::ERROR << "Could not retrieve TrackRecord collection!" << endreq;
      return StatusCode::SUCCESS;
    }
  }
  else {
    log << MSG::DEBUG << "Could not find TrackRecord collection" << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::DEBUG << "There are " << trackCollection->size() << "tracks in this container " << endreq;

  // create and record a new collection
  TrackRecordCollection* filterCollection = new TrackRecordCollection();   
  if (StatusCode::SUCCESS != m_storeGate->record(filterCollection, m_outputName) ) {
    log << MSG::ERROR << "can not record collection!" << endreq;
    return StatusCode::SUCCESS;
  }

  // iterate over the collection
  for (TrackRecordCollection::const_iterator trkit=trackCollection->begin(); trkit != trackCollection->end() ; ++trkit) {
     int pdgId((*trkit)->GetPDGCode());
     log << MSG::VERBOSE << "Track found with pdg id= " << (*trkit)->GetPDGCode() << " with energy "<< (*trkit)->GetEnergy() << endreq;

     if(pdgId) { //Geant makes particle with pdgid=0...
        // get rid of neutral particles
        const HepPDT::ParticleData* particle =
            m_pParticleTable->particle(HepPDT::ParticleID(abs(pdgId)));
        if(particle){
          if(fabs(particle->charge() ) >0.5 && (*trkit)->GetEnergy() > m_cutOff)
	    filterCollection->push_back(new TrackRecord(**trkit));
        }
     }
  }

  //lock the collection
  StatusCode sc = m_storeGate->setConst(filterCollection);
  if (sc.isFailure())  {
    log << MSG::FATAL << "Cannot set collection to const" << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << "There are " << filterCollection->size() << "that satisfy the filter " << endreq;
  return StatusCode::SUCCESS;
}

StatusCode TrackRecordFilter::finalize() {

   return StatusCode::SUCCESS;
}

