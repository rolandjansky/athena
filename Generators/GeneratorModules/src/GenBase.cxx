/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorModules/GenBase.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/Incident.h"
#include <fstream>


GenBase::GenBase(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_mcEventKey("GEN_EVENT"),
    m_ppSvc("PartPropSvc", name),
    m_mcevents("GEN_EVENT"), m_mcevents_const("GEN_EVENT")
{
  declareProperty("McEventKey", m_mcEventKey="GEN_EVENT", "StoreGate key of the MC event collection");
  declareProperty("MakeMcEvent", m_mkMcEvent=false, "Create a new MC event collection if it doesn't exist");
  declareProperty("PartPropSvc", m_ppSvc);
  declareProperty("McEventsRW", m_mcevents=SG::RWVar<McEventCollection>(m_mcEventKey));
  declareProperty("McEventsR", m_mcevents_const=SG::RVar<McEventCollection>(m_mcEventKey));
}


StatusCode GenBase::initialize() {
  // Get the particle property service
  if (m_ppSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not initialize ATLAS Particle Property Service");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


/// Access the current event's McEventCollection
SG::RWVar<McEventCollection> GenBase::events() {
  // Make a new MC event collection if necessary
  if (!evtStore()->contains<McEventCollection>(m_mcEventKey) && m_mkMcEvent) {
    ATH_MSG_DEBUG("Creating new McEventCollection in the event store");
    McEventCollection* mcevents = new McEventCollection();
    if (evtStore()->record(mcevents, m_mcEventKey).isFailure())
      ATH_MSG_ERROR("Failed to record a new McEventCollection");
  }

  return m_mcevents;
}


/// @todo Add HepMC units awareness and do it differently when HepMC provides
///       this functionality directly (and reference-based FourVector accessors)
void GenBase::GeVToMeV(HepMC::GenEvent* evt) {
  for (HepMC::GenEvent::particle_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p) {
    const HepMC::FourVector fv((*p)->momentum().px() * 1000,
                               (*p)->momentum().py() * 1000,
                               (*p)->momentum().pz() * 1000,
                               (*p)->momentum().e()  * 1000);
    (*p)->set_momentum(fv);
    (*p)->set_generated_mass(1000 * (*p)->generated_mass());
  }
}
void GenBase::MeVToGeV(HepMC::GenEvent* evt) {
  for (HepMC::GenEvent::particle_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p) {
    const HepMC::FourVector fv((*p)->momentum().px() / 1000,
                               (*p)->momentum().py() / 1000,
                               (*p)->momentum().pz() / 1000,
                               (*p)->momentum().e()  / 1000);
    (*p)->set_momentum(fv);
    (*p)->set_generated_mass((*p)->generated_mass() / 1000);
  }
}
void GenBase::cmTomm(HepMC::GenEvent* evt) {
  for (HepMC::GenEvent::vertex_iterator vtx = evt->vertices_begin(); vtx != evt->vertices_end(); ++vtx) {
    const HepMC::FourVector fv((*vtx)->position().x() * 10,
                               (*vtx)->position().y() * 10,
                               (*vtx)->position().z() * 10,
                               (*vtx)->position().t() * 10);
    (*vtx)->set_position(fv);
  }
}
void GenBase::mmTocm(HepMC::GenEvent* evt) {
  for (HepMC::GenEvent::vertex_iterator vtx = evt->vertices_begin(); vtx != evt->vertices_end(); ++vtx) {
    const HepMC::FourVector fv((*vtx)->position().x() / 10,
                               (*vtx)->position().y() / 10,
                               (*vtx)->position().z() / 10,
                               (*vtx)->position().t() / 10);
    (*vtx)->set_position(fv);
  }
}
