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

HepMC::GenParticle* ISF::ISFTruthIncident::parentParticle(bool setPersistent) const {
  return convert( &m_parent, setPersistent);
}

Barcode::ParticleBarcode ISF::ISFTruthIncident::parentBarcode() const {
  return m_parent.barcode();
}

Barcode::ParticleBarcode ISF::ISFTruthIncident::parentExtraBarcode() const {
  return m_parent.getExtraBC();
}

bool ISF::ISFTruthIncident::parentSurvivesIncident() const {
  if (m_killsPrimary == ISF::fKillsPrimary) {
      return false;
  } else {
      return true;
  }
}

HepMC::GenParticle* ISF::ISFTruthIncident::parentParticleAfterIncident(Barcode::ParticleBarcode newBC,
                                                                        bool setPersistent) {
  // if parent is killed in the interaction -> return 0
  if (m_killsPrimary==ISF::fKillsPrimary) return 0;

  // only update the parent particle, if it survived the interaction:

  // set a new barcode
  m_parent.setBarcode( newBC);
  // return the updated parent particle
  return parentParticle(setPersistent);
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
                                                             Barcode::ParticleBarcode bc,
                                                             bool setPersistent) const {
  // the child particle
  ISF::ISFParticle *sec = m_children[index];

  // set particle barcode of the child particle
  if (bc) {
    sec->setBarcode( bc);
  }

  // convert it to HepMC
  return convert( sec, setPersistent);
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


void ISF::ISFTruthIncident::setAllChildrenExtraBarcodes(Barcode::ParticleBarcode bc) 
{
  unsigned short numSec = numberOfChildren();
  for (unsigned short i=0; i<numSec; i++) {
     setChildExtraBarcode(i,bc);
  }
  return;
}


void ISF::ISFTruthIncident::setChildExtraBarcode(unsigned short index, Barcode::ParticleBarcode bc) 
{
  ISF::ISFParticle *p = m_children[index];

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

