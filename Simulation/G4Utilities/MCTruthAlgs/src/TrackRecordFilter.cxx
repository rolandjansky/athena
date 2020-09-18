/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthAlgs/TrackRecordFilter.h"


// particle prop service
#include "GaudiKernel/IPartPropSvc.h"
// particle table
#include "HepPDT/ParticleDataTable.hh"
// Particle data table
#include "HepPDT/ParticleData.hh"
#include "CLHEP/Units/SystemOfUnits.h"

TrackRecordFilter::TrackRecordFilter(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator),
  m_inputKey("MuonEntryLayer"),
  m_outputKey("MuonEntryLayerFilter"),
  m_pParticleTable(0)
{
  declareProperty("inputName", m_inputKey);
  declareProperty("outputName", m_outputKey);
  declareProperty("threshold",m_cutOff=100.*CLHEP::MeV);
}

StatusCode TrackRecordFilter::initialize() {
  //FIXME Old syntax
  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc = 0;
  constexpr bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    ATH_MSG_ERROR ( " Could not initialize Particle Properties Service" );
    return PartPropStatus;
  }
  m_pParticleTable = p_PartPropSvc->PDT();

  ATH_CHECK(m_inputKey.initialize());
  ATH_CHECK(m_outputKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode TrackRecordFilter::execute(const EventContext& ctx) const {

  // Get message service
  ATH_MSG_DEBUG ( "TrackRecordFilter::execute()" );

  // retrieve the collection
  SG::ReadHandle<TrackRecordCollection> trackCollection(m_inputKey, ctx);
  if (!trackCollection.isPresent() || !trackCollection.isValid()) {
    ATH_MSG_DEBUG ( "Could not find TrackRecord collection" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG ( "There are " << trackCollection->size() << "tracks in this container " );

  SG::WriteHandle<TrackRecordCollection> filterCollection(m_outputKey, ctx);
  // create and record a new collection
  CHECK( filterCollection.record(std::make_unique<TrackRecordCollection>()) );

  // iterate over the collection
  for (const auto& trkit : *trackCollection) {
    int pdgId(trkit.GetPDGCode());
    ATH_MSG_VERBOSE ( "Track found with pdg id= " << trkit.GetPDGCode() << " with energy "<< trkit.GetEnergy() );

    if(pdgId) { //Geant makes particle with pdgid=0...
      // get rid of neutral particles
      const HepPDT::ParticleData* particle =
        m_pParticleTable->particle(HepPDT::ParticleID(std::abs(pdgId)));
      if(particle){
        if(std::fabs(particle->charge() ) >0.5 && trkit.GetEnergy() > m_cutOff)
          filterCollection->push_back(TrackRecord(trkit));
      }
    }
  }


  ATH_MSG_DEBUG ( "There are " << filterCollection->size() << "that satisfy the filter " );
  return StatusCode::SUCCESS;
}

StatusCode TrackRecordFilter::finalize() {
  return StatusCode::SUCCESS;
}

