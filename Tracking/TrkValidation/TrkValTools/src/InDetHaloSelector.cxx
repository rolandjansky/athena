/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetHaloSelector.cxx
//   Source file for class InDetHaloSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValTools/InDetHaloSelector.h"
#include "AtlasHepMC/GenVertex.h"
#include "CLHEP/Geometry/Point3D.h"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

Trk::InDetHaloSelector::InDetHaloSelector(const std::string& type, const std::string& name,
                                              const IInterface* parent)
  : AthAlgTool (type,name,parent),
  m_particleDataTable{}

{
  declareInterface<IGenParticleSelector>(this);

}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::InDetHaloSelector::initialize() {

  // get the Particle Properties Service
  IPartPropSvc* partPropSvc = 0;
  StatusCode sc =  service("PartPropSvc", partPropSvc, true);
  if (sc.isFailure()) {
    ATH_MSG_FATAL (" Could not initialize Particle Properties Service");
    return StatusCode::FAILURE;
  }
  m_particleDataTable = partPropSvc->PDT();

  ATH_MSG_DEBUG ("initialise in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Trk::InDetHaloSelector::finalize() {
  ATH_MSG_DEBUG ("starting finalize() in " << name());
  return StatusCode::SUCCESS;
}

std::vector<const HepMC::GenParticle*>*
Trk::InDetHaloSelector::selectGenSignal (const McEventCollection* SimTracks) const {

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

      
      int   pdgCode         = particle->pdg_id();
      if (abs(pdgCode) > 1000000000 ) continue; // ignore nuclei from hadronic interactions
      const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgCode));
      ATH_MSG_DEBUG( "checking particle barcode = " <<  particle->barcode() );
      if (!pd) { // nuclei excluded, still problems with a given type?
	ATH_MSG_INFO ("Could not get particle data for particle with pdgCode="<<pdgCode
		      << ", status=" << particle->status() << ", barcode=" << particle->barcode());
	ATH_MSG_INFO ("GenParticle= " << *particle);
	continue;
      }
      float charge          = pd->charge();
      ATH_MSG_DEBUG( "particle charge = " << charge );
      if (fabs(charge)<0.5) continue;
      
      genSignal->push_back(particle);
    
    
    } // loop and select particles
  }   // loop and select pile-up vertices
  return genSignal;
}

