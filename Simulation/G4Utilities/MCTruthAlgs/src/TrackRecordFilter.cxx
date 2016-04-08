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

TrackRecordFilter::TrackRecordFilter(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator), m_pParticleTable(0)
{
  declareProperty("inputName", m_inputName="MuonEntryLayer");
  declareProperty("outputName",m_outputName="MuonEntryLayerFilter");
  declareProperty("threshold",m_cutOff=100.*CLHEP::MeV);
}

StatusCode TrackRecordFilter::initialize() {
  //FIXME Old syntax
  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc = 0;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    ATH_MSG_ERROR ( " Could not initialize Particle Properties Service" );
    return PartPropStatus;
  }
  m_pParticleTable = p_PartPropSvc->PDT();

  return StatusCode::SUCCESS;
}

StatusCode TrackRecordFilter::execute() {

  // Get message service
  ATH_MSG_DEBUG ( "TrackRecordFilter::execute()" );

  // retrieve the collection
  const TrackRecordCollection* trackCollection(0);
  if (evtStore()->contains<TrackRecordCollection>(m_inputName)) {
    CHECK( evtStore()->retrieve(trackCollection, m_inputName) );
  }
  else {
    ATH_MSG_DEBUG ( "Could not find TrackRecord collection" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "There are " << trackCollection->size() << "tracks in this container " );

  // create and record a new collection
  TrackRecordCollection* filterCollection = new TrackRecordCollection();
  CHECK( evtStore()->record(filterCollection, m_outputName) );

  // iterate over the collection
  for (auto trkit : *trackCollection) {
    int pdgId(trkit.GetPDGCode());
    ATH_MSG_VERBOSE ( "Track found with pdg id= " << trkit.GetPDGCode() << " with energy "<< trkit.GetEnergy() );

    if(pdgId) { //Geant makes particle with pdgid=0...
      // get rid of neutral particles
      const HepPDT::ParticleData* particle =
        m_pParticleTable->particle(HepPDT::ParticleID(abs(pdgId)));
      if(particle){
        if(fabs(particle->charge() ) >0.5 && trkit.GetEnergy() > m_cutOff)
          filterCollection->push_back(TrackRecord(trkit));
      }
    }
  }

  //lock the collection
  CHECK( evtStore()->setConst(filterCollection) );

  ATH_MSG_DEBUG ( "There are " << filterCollection->size() << "that satisfy the filter " );
  return StatusCode::SUCCESS;
}

StatusCode TrackRecordFilter::finalize() {
  return StatusCode::SUCCESS;
}
