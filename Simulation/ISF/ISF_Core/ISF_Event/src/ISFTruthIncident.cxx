/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISFTruthIncident.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header
#include "ISF_Event/ISFTruthIncident.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/ParticleHelper.h"

// HepMC includes
#include "HepMC/SimpleVector.h"
#include "HepMC/GenParticle.h"

ISF::ISFTruthIncident::ISFTruthIncident( ISF::ISFParticle &parent,
                                         const ISFParticleVector& children,
                                         Barcode::PhysicsProcessCode process,
                                         AtlasDetDescr::AtlasRegion geoID,
                                         ISF::KillPrimary killsPrimary,
                                         const HepMC::FourVector *position) :
  ITruthIncident(geoID, children.size()),
  m_parent(parent),
  m_children(children),
  m_process(process),
  m_killsPrimary(killsPrimary),
  m_position(position)
{
}

ISF::ISFTruthIncident::~ISFTruthIncident() {
  delete m_position;
}

const HepMC::FourVector& ISF::ISFTruthIncident::position() const {

  if ( !m_position) {
    // no position was given, compute it
    //  (1.) try retrieve the position from the first child particle
    //  (2.) if no child particles given -> get position from parent particle
    const ISF::ISFParticle *particle = m_children.front();
    if ( !particle) particle = &m_parent;
    const Amg::Vector3D &pos = particle->position();

    double time = 0.;  //<! TODO: FIXME
    m_position = new HepMC::FourVector( pos.x(), pos.y(), pos.z(), time );
  }

  return *m_position;
}

int ISF::ISFTruthIncident::physicsProcessCategory() const {
  return -1;
}

Barcode::PhysicsProcessCode ISF::ISFTruthIncident::physicsProcessCode() const {
  return m_process;
}

double ISF::ISFTruthIncident::parentP2() const {
  return m_parent.momentum().mag2();
}

double ISF::ISFTruthIncident::parentPt2() const {
  return m_parent.momentum().perp2();
}

double ISF::ISFTruthIncident::parentEkin() const {
  return m_parent.ekin();
}

int ISF::ISFTruthIncident::parentPdgCode() const {
  return m_parent.pdgCode();
}

HepMC::GenParticle* ISF::ISFTruthIncident::parentParticle() const {

  if ( m_parent.getTruthBinding() ) {
    return getHepMCTruthParticle(m_parent);
  } else {
    return updateHepMCTruthParticle(m_parent, &m_parent);
  }
}

Barcode::ParticleBarcode ISF::ISFTruthIncident::parentBarcode() const {
  return m_parent.barcode();
}

int ISF::ISFTruthIncident::parentBCID() const {
  return m_parent.getBCID();
}

bool ISF::ISFTruthIncident::parentSurvivesIncident() const {
  return !(m_killsPrimary == ISF::fKillsPrimary);
}

HepMC::GenParticle* ISF::ISFTruthIncident::parentParticleAfterIncident(Barcode::ParticleBarcode newBC) {
  // if parent is killed in the interaction -> return nullptr
  if (m_killsPrimary==ISF::fKillsPrimary) return nullptr;

  // only update the parent particle, if it survived the interaction

  m_parent.setBarcode( newBC );

  return updateHepMCTruthParticle(m_parent, &m_parent);
}

double ISF::ISFTruthIncident::childP2(unsigned short index) const {
  return m_children[index]->momentum().mag2();
}

double ISF::ISFTruthIncident::childPt2(unsigned short index) const {
  return m_children[index]->momentum().perp2();
}

double ISF::ISFTruthIncident::childEkin(unsigned short index) const {
  return m_children[index]->ekin();
}


int ISF::ISFTruthIncident::childPdgCode(unsigned short index) const {
  return m_children[index]->pdgCode();
}

HepMC::GenParticle* ISF::ISFTruthIncident::childParticle(unsigned short index,
                                                         Barcode::ParticleBarcode bc) const {
  // the child particle
  ISF::ISFParticle *sec = m_children[index];

  // set particle barcode of the child particle
  if (bc) {
    sec->setBarcode( bc);
  }

  return updateHepMCTruthParticle( *sec, &m_parent );
}

HepMC::GenParticle* ISF::ISFTruthIncident::updateChildParticle(unsigned short /*index*/,
                                                               HepMC::GenParticle *existingChild) const {
  // Dummy implementation
  return existingChild;
}

void ISF::ISFTruthIncident::setAllChildrenBarcodes(Barcode::ParticleBarcode bc) {
  unsigned short numSec = numberOfChildren();
  for (unsigned short i=0; i<numSec; i++) {
    // the current particle
    ISF::ISFParticle *p = m_children[i];

    // set a new barcode
    p->setBarcode( bc);
  }

  return;
}


/** return attached truth particle */
HepMC::GenParticle* ISF::ISFTruthIncident::getHepMCTruthParticle( const ISF::ISFParticle& particle ) const {
  auto* truthBinding     = particle.getTruthBinding();
  auto* hepTruthParticle = truthBinding ? truthBinding->getTruthParticle() : nullptr;

  return hepTruthParticle;
}


/** convert ISFParticle to GenParticle and attach to ISFParticle's TruthBinding */
HepMC::GenParticle* ISF::ISFTruthIncident::updateHepMCTruthParticle( ISF::ISFParticle& particle,
                                                                     const ISF::ISFParticle* parent ) const {
  auto* truthBinding     = particle.getTruthBinding();
  auto* hepTruthParticle = ParticleHelper::convert( particle );

  if (truthBinding) {
    truthBinding->setTruthParticle(hepTruthParticle);
  } else {
    auto* parentTruthBinding = parent ? parent->getTruthBinding() : nullptr;
    auto* hepPrimaryParticle = parentTruthBinding ? parentTruthBinding->getPrimaryTruthParticle() : nullptr;
    auto* hepGenZeroParticle = hepTruthParticle;
    truthBinding = new TruthBinding( hepTruthParticle, hepPrimaryParticle, hepGenZeroParticle );
    particle.setTruthBinding(truthBinding);
  }

  return hepTruthParticle;
}

