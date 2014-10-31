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

ISF::ISFTruthIncident::ISFTruthIncident( ISF::ISFParticle &primary,
                                         const ISFParticleVector& secondary,
                                         Barcode::PhysicsProcessCode process,
                                         AtlasDetDescr::AtlasRegion geoID,
                                         ISF::KillPrimary killsPrimary,
                                         const HepMC::FourVector *position) :
  ITruthIncident(geoID),
  m_primary(primary),
  m_secondary(secondary),
  m_process(process),
  m_killsPrimary(killsPrimary),
  m_position(position),
  m_passedFilters(secondary.size(), false)
{
}

ISF::ISFTruthIncident::~ISFTruthIncident() {
  delete m_position;
}

const HepMC::FourVector& ISF::ISFTruthIncident::position() const {

  if ( !m_position) {
    // no position was given, compute it
    //  (1.) try retrieve the position from the first secondary particle
    //  (2.) if no secondaries given -> get position from primary particle
    const ISF::ISFParticle *particle = m_secondary.front();
    if ( !particle) particle = &m_primary;
    const Amg::Vector3D &pos = particle->position();

    double time = 0.;  //<! TODO: FIXME
    m_position = new HepMC::FourVector( pos.x(), pos.y(), pos.z(), time );
  }

  return *m_position;
}

Barcode::PhysicsProcessCode ISF::ISFTruthIncident::physicsProcessCode() const {
  return m_process;
}

double ISF::ISFTruthIncident::primaryP2() const {
  return m_primary.momentum().mag2();
}

double ISF::ISFTruthIncident::primaryPt2() const {
  return m_primary.momentum().perp2();
}

double ISF::ISFTruthIncident::primaryEkin() const {
  return m_primary.ekin();
}

int ISF::ISFTruthIncident::primaryPdgCode() const {
  return m_primary.pdgCode();
}

HepMC::GenParticle* ISF::ISFTruthIncident::primaryParticle(bool setPersistent) const {
  return convert( &m_primary, setPersistent);
}

Barcode::ParticleBarcode ISF::ISFTruthIncident::primaryBarcode() const {
  return m_primary.barcode();
}

Barcode::ParticleBarcode ISF::ISFTruthIncident::primaryExtraBarcode() const {
  return m_primary.getExtraBC();
}

HepMC::GenParticle* ISF::ISFTruthIncident::primaryParticleAfterIncident(Barcode::ParticleBarcode newBC,
                                                                        bool setPersistent) {
  // if primary is killed in the interaction -> return 0
  if (m_killsPrimary==ISF::fKillsPrimary) return 0;

  // only update the primary particle, if it survived the interaction:

  // set a new barcode
  m_primary.setBarcode( newBC);
  // return the updated primary particle
  return primaryParticle(setPersistent);
}

unsigned short ISF::ISFTruthIncident::numberOfSecondaries() const {
  return m_secondary.size();
}

double ISF::ISFTruthIncident::secondaryP2(unsigned short index) const {
  return m_secondary[index]->momentum().mag2();
}

double ISF::ISFTruthIncident::secondaryPt2(unsigned short index) const {
  return m_secondary[index]->momentum().perp2();
}

double ISF::ISFTruthIncident::secondaryEkin(unsigned short index) const {
  return m_secondary[index]->ekin();
}


int ISF::ISFTruthIncident::secondaryPdgCode(unsigned short index) const {
  return m_secondary[index]->pdgCode();
}

HepMC::GenParticle* ISF::ISFTruthIncident::secondaryParticle(unsigned short index,
                                                             Barcode::ParticleBarcode bc,
                                                             bool setPersistent) const {
  // the secondary particle
  ISF::ISFParticle *sec = m_secondary[index];

  // set particle barcode of the secondary particle
  if (bc) {
    sec->setBarcode( bc);
  }

  // convert it to HepMC
  return convert( sec, setPersistent);
}

void ISF::ISFTruthIncident::setAllSecondaryBarcodes(Barcode::ParticleBarcode bc) {
  unsigned short numSec = numberOfSecondaries();
  for (unsigned short i=0; i<numSec; i++) {
    // the current particle
    ISF::ISFParticle *p = m_secondary[i];

    // set a new barcode
    p->setBarcode( bc);
  }

  return;
}


void ISF::ISFTruthIncident::setAllSecondaryExtraBarcodes(Barcode::ParticleBarcode bc) 
{
  unsigned short numSec = numberOfSecondaries();
  for (unsigned short i=0; i<numSec; i++) {
     setSecondaryExtraBarcode(i,bc);
  }
  return;
}


void ISF::ISFTruthIncident::setSecondaryExtraBarcode(unsigned short index, Barcode::ParticleBarcode bc) 
{
  ISF::ISFParticle *p = m_secondary[index];

  // MB : this can be removed at some point
  if (p->getUserInformation()==0) {
    p->setUserInformation( new ParticleUserInformation() );
  }

  p->setExtraBC( bc );
}


HepMC::GenParticle* ISF::ISFTruthIncident::convert( ISF::ISFParticle *particle,
                                                    bool setPersistent ) const {
  ITruthBinding *truth = particle->truthBinding();
  // if truth binding exists -> try to retrieve a HepMC::GenParticle from it
  if ( truth) {
    truth->setPersistency( setPersistent);
    // GenParticleTruth type gives a HepMC::GenParticle as truthParticle()
    const GenParticleTruth *mctruth = dynamic_cast<const GenParticleTruth* >(truth);
    if (mctruth) return &(mctruth->truthParticle());
  }

  // HepMC::GenParticle not in TruthBinding -> create new HepMC::GenParticle
  HepMC::GenParticle *hepParticle = ParticleHelper::convert( *particle );

  hepParticle->suggest_barcode( particle->barcode());
  // if no truth binding present, register the new GenParticle as TruthBinding
  if ( !truth) {
    truth = new GenParticleTruth(*hepParticle);
    // TruthBindings created at this point will always be used (e.g. in McTruth Tree)
    truth->setPersistency( setPersistent);
    particle->setTruthBinding( truth);
  }

  return hepParticle;
}

