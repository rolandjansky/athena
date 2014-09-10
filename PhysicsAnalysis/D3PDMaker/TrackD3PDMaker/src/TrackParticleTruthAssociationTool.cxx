/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthAssociationTool.h"

#include "AthenaKernel/errorcheck.h"
#include "HepMC/GenParticle.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include <map>

namespace D3PD {

TrackParticleTruthAssociationTool::TrackParticleTruthAssociationTool (const std::string& type,
									  const std::string& name,
									  const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("MapKey", m_TruthMap = "TrackParticleTruthMap");
  declareProperty ("SGKey", m_TPContainer = "TrackParticleCandidate");

  book().ignore(); // Avoid coverity warnings.
}


StatusCode TrackParticleTruthAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrackParticleTruthAssociationTool::book ()
{
  CHECK( addVariable ("probability", m_trueProb) );
  CHECK( addVariable ("barcode", m_trueBarcode) );

  return StatusCode::SUCCESS;
}


const HepMC::GenParticle* TrackParticleTruthAssociationTool::get (const Rec::TrackParticle& track)
{

   /// Get Track and TrackTruth Collections
  /// Should be using Getter tools intead of StoreGate
  // -- TrackParticles:
  const Rec::TrackParticleContainer* tp;
  StatusCode sc = evtStore()->retrieve(tp, m_TPContainer);  
  if(sc.isFailure() || !tp) {
    return 0;
  }
  // -- TrackParticleTruth:
  const TrackParticleTruthCollection* tm;
  sc = evtStore()->retrieve(tm, m_TruthMap);
  if(sc.isFailure() || !tm) {
    return 0;
  }

  const HepMC::GenParticle *particle = 0;
  *m_trueProb = 0;
  *m_trueBarcode = -1;

  ElementLink<Rec::TrackParticleContainer> tlink;
  tlink.setElement(&track);
  tlink.setStorableObject(*tp);
  TrackParticleTruthCollection::const_iterator found = tm->find(tlink);
  if(found != tm->end()) {
    *m_trueProb = found->second.probability();
    *m_trueBarcode = found->second.particleLink().barcode();
    particle = found->second.particleLink().cptr();
  }

  return particle;
}

} // namespace D3PD
