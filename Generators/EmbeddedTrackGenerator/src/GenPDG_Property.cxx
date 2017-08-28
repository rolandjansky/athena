/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2008

#include "EmbeddedTrackGenerator/GenPDG_Property.h"
#include "CLHEP/Random/RandomEngine.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include <cmath>


//================================================================


StatusCode GenPDG_Property::initialize(const HepPDT::ParticleDataTable* particleTable) {
  MsgStream log(m_parent->msgSvc(), m_parent->name() );
  const HepPDT::ParticleData* particle_data = particleTable->particle(HepPDT::ParticleID(std::abs( m_pdg_id )));
  if (!particle_data) {
    log << MSG::FATAL << "GenPDG_Property::initialize(): can't get data for particle with pdg_id=" << std::abs(m_pdg_id) << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_mass = particle_data->mass().value();
  log << MSG::DEBUG << "GenPDG_Property::initialize(): got mass=" << m_mass << " for pdg_id=" << m_pdg_id << endmsg;
  
  return StatusCode::SUCCESS;
}


//================================================================


GenPDG_Property::ParticleDescription
GenPDG_Property::getParticle(CLHEP::HepRandomEngine* rndEng) const {
  int id = m_generateAntiParticles && (rndEng->flat() < 0.5) ?  -m_pdg_id : m_pdg_id;
  return std::make_pair(id, m_mass);
}
