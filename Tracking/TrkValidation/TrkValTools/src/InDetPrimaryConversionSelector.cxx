/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetPrimaryConversionSelector.cxx
//   Source file for class InDetPrimaryConversionSelector
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValTools/InDetPrimaryConversionSelector.h"
#include "AtlasHepMC/GenVertex.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

Trk::InDetPrimaryConversionSelector::InDetPrimaryConversionSelector(const std::string& type, const std::string& name,
                                              const IInterface* parent)
  : AthAlgTool (type,name,parent),
    m_particleDataTable{},
    m_minPt             ( 500.    ),
    m_maxEta            (   2.5   ),
    m_maxRStartAll      ( 800.0*CLHEP::mm),
    m_maxZStartAll      (2000.0*CLHEP::mm)

{
  declareInterface<IGenParticleSelector>(this);

  declareProperty("MinPt",              m_minPt);
  declareProperty("MaxEta",             m_maxEta);
  declareProperty("MaxRStartAll",       m_maxRStartAll,        "production vtx r for all");
  declareProperty("MaxZStartAll",       m_maxZStartAll,        "production vtx z for all");
}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::InDetPrimaryConversionSelector::initialize() {

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

StatusCode Trk::InDetPrimaryConversionSelector::finalize() {
  ATH_MSG_INFO ("starting finalize() in " << name());
  return StatusCode::SUCCESS;
}

std::vector<const HepMC::GenParticle*>*
Trk::InDetPrimaryConversionSelector::selectGenSignal (const McEventCollection* SimTracks) const {

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

        int   pdgCode         = particle->pdg_id();
        if (abs(pdgCode) > 1000000000 ) continue; // ignore nuclei from hadronic interactions
        const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgCode));

        if (!pd) { // nuclei excluded, still problems with a given type?
          ATH_MSG_INFO ("Could not get particle data for particle with pdgCode="<<pdgCode<< ", status=" << particle->status() << ", barcode=" << particle->barcode());
          ATH_MSG_INFO ("GenParticle= " << *particle);
          continue;
        }

	ATH_MSG_DEBUG ("found particle = " << *particle);

	// assume for the moment we're only running over single gamma MC files ...
	HepMC::GenVertex* prodVertex( particle->production_vertex());
	if ( abs(pdgCode) == 11 ) {
	  ATH_MSG_DEBUG ("Electron/Positron detected -- checking for production process ...");
	  HepMC::GenVertex::particles_in_const_iterator inParticle     = prodVertex->particles_in_const_begin();
	  HepMC::GenVertex::particles_out_const_iterator inParticleEnd = prodVertex->particles_in_const_end();
	  for ( ; inParticle != inParticleEnd; ++inParticle) {
	    ATH_MSG_DEBUG(" --> checking morther: " << *(*inParticle) );
	    if ( abs((*inParticle)->pdg_id()) == 22 || abs((*inParticle)->pdg_id()) == 11 ){
	      if (fabs(particle->momentum().perp()) >  m_minPt  &&  fabs(particle->momentum().pseudoRapidity()) < m_maxEta ) {
		genSignal->push_back(particle);
		ATH_MSG_DEBUG ("Selected this electron/positron!");
		break;
	      } // if (particle pt, eta)
	    } // if (mother is gamma wit barcode ...)
	  } // loop over mother particles
	} // if (have electron/positron)
      } // if (have stable particle)
    } // loop and select particles
  }   // loop and select pile-up vertices
  return genSignal;
}

