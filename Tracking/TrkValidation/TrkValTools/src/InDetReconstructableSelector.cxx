/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetReconstructableSelector.cxx
//   Source file for class InDetReconstructableSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValTools/InDetReconstructableSelector.h"
#include "AtlasHepMC/GenVertex.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

Trk::InDetReconstructableSelector::InDetReconstructableSelector(const std::string& type, const std::string& name,
                                              const IInterface* parent)
  : AthAlgTool (type,name,parent),
    m_particleDataTable{},
    m_minPt             (1000.),
    m_maxEta            (   3.0),
    m_selectPrimariesOnly(false),
    m_maxRStartPrimary  (  25.0*CLHEP::mm),
    m_maxZStartPrimary  ( 200.0*CLHEP::mm),
    m_maxRStartAll      ( 360.0*CLHEP::mm),
    m_maxZStartAll      (2000.0*CLHEP::mm)

{
  declareInterface<IGenParticleSelector>(this);

  declareProperty("MinPt",              m_minPt);
  declareProperty("MaxEta",             m_maxEta);
  declareProperty("SelectPrimariesOnly",m_selectPrimariesOnly, "flag to restrict to primaries or not");
  declareProperty("MaxRStartPrimary",   m_maxRStartPrimary,    "production vtx r for primaries");
  declareProperty("MaxZStartPrimary",   m_maxZStartPrimary,    "production vtx z for primaries");
  declareProperty("MaxRStartAll",       m_maxRStartAll,        "production vtx r for all");
  declareProperty("MaxZStartAll",       m_maxZStartAll,        "production vtx z for all");
}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::InDetReconstructableSelector::initialize() {

  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  StatusCode sc =  service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    ATH_MSG_FATAL (" Could not initialize Particle Properties Service");
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

  ATH_MSG_INFO ("initialise in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Trk::InDetReconstructableSelector::finalize() {
  ATH_MSG_INFO ("starting finalize() in " << name());
  return StatusCode::SUCCESS;
}

std::vector<const HepMC::GenParticle*>*
Trk::InDetReconstructableSelector::selectGenSignal (const McEventCollection* SimTracks) const {

  if (! SimTracks) return NULL;

  std::vector<const HepMC::GenParticle *>* genSignal = 
    new std::vector<const HepMC::GenParticle *>;

  // pile-up: vector of MCEC has more than one entry
  DataVector<HepMC::GenEvent>::const_iterator itCollision = SimTracks->begin();
  
  for( ; itCollision != SimTracks->end(); ++itCollision ) {
    const HepMC::GenEvent*    genEvent = *itCollision;
    HepMC::GenParticle * particle = NULL;
    
    for (HepMC::GenEvent::particle_const_iterator it = genEvent->particles_begin();
         it != genEvent->particles_end(); ++it) {

      particle = *it;

      // 1) require stable particle from generation or simulation
      if ((particle->status()%1000) != 1 )    continue;

      if(particle->production_vertex() == NULL) {
        ATH_MSG_WARNING ("GenParticle without production vertex - simulation corrupt? ");
        ATH_MSG_DEBUG   ("It's this one: " << *particle);
        continue;
      } else {
      
        // 2) require track inside ID - relaxed definition including decays of neutrals (secondaries)
        if ( fabs(particle->production_vertex()->position().perp()) > m_maxRStartAll ||
             fabs(particle->production_vertex()->position().z())    > m_maxZStartAll ) continue;

        // 3) if jobOption, require strict definition of particles from within beam pipe
        if ( m_selectPrimariesOnly && 
             ( fabs(particle->production_vertex()->position().perp()) > m_maxRStartPrimary ||
               fabs(particle->production_vertex()->position().z())    > m_maxZStartPrimary ) ) continue;

        int   pdgCode         = particle->pdg_id();
        if (abs(pdgCode) > 1000000000 ) continue; // ignore nuclei from hadronic interactions
        const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgCode));

        if (!pd) { // nuclei excluded, still problems with a given type?
          ATH_MSG_INFO ("Could not get particle data for particle with pdgCode="<<pdgCode<< ", status=" << particle->status() << ", barcode=" << particle->barcode());
          ATH_MSG_INFO ("GenParticle= " << *particle);
          continue;
        }
        float charge          = pd->charge();
        if (fabs(charge)<0.5) continue;

        if (fabs(particle->momentum().perp()) >  m_minPt  &&  fabs(particle->momentum().pseudoRapidity()) < m_maxEta ) {
          genSignal->push_back(particle);
        }
      }
    } // loop and select particles
  }   // loop and select pile-up vertices
  return genSignal;
}

