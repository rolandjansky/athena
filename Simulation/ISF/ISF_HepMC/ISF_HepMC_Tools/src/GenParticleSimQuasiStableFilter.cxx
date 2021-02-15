/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleSimQuasiStableFilter.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "GenParticleSimQuasiStableFilter.h"

// HepMC includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

#include <algorithm>

/** Constructor **/
ISF::GenParticleSimQuasiStableFilter::GenParticleSimQuasiStableFilter( const std::string& t,
                                                       const std::string& n,
                                                       const IInterface* p )
  : base_class(t,n,p)
{
    // different options
    declareProperty("StatusCode", m_statuscode);
    declareProperty("MinProdRadius", m_minProdRadius);
    declareProperty("MinDecayRadius", m_minDecayRadius);
}

// Athena algtool's Hooks
StatusCode  ISF::GenParticleSimQuasiStableFilter::initialize()
{
    ATH_MSG_VERBOSE("Initializing ...");
    
    m_minProdRadius.resize(m_statuscode.size(),0);
    m_minDecayRadius.resize(m_statuscode.size(),0);

    // All done!
    return StatusCode::SUCCESS;
}
#ifdef HEPMC3
bool ISF::GenParticleSimQuasiStableFilter::pass(HepMC::ConstGenParticlePtr particle) const
{
#else
bool ISF::GenParticleSimQuasiStableFilter::pass(const HepMC::GenParticle& particleref) const
{
   auto particle=&particleref;
#endif
  ATH_MSG_VERBOSE( "Checking whether " << particle << " passes the filter." );
  
  bool passFilter = false;
  for(unsigned int i=0;i<m_statuscode.size();++i) {
    if(particle->status()==m_statuscode[i]) {
      passFilter=true;
      if(particle->end_vertex()        && (m_minDecayRadius[i] > particle->end_vertex()->position().perp())        ) passFilter = false;
      if(particle->production_vertex() && (m_minProdRadius[i]  > particle->production_vertex()->position().perp()) ) passFilter = false;
      if(passFilter) break;
    }
  }
  
  if(passFilter) {
    ATH_MSG_DEBUG( "Accpeting particle " << particle );
    if(particle->production_vertex()) ATH_MSG_DEBUG("  prod r="<<particle->production_vertex()->position().perp()<<" : "<<particle->production_vertex ());
    if(particle->end_vertex()) ATH_MSG_DEBUG("  decay r="<<particle->end_vertex()->position().perp()<<" : "<<particle->end_vertex ());
  } else {
//      ATH_MSG_DEBUG( "Rejecting particle " << particle );
//      if(particle.production_vertex ()) ATH_MSG_DEBUG("  prod r="<<particle.production_vertex()->position().perp()<<", min="<<m_minProdRadius[i]<<" : "<<*(particle.production_vertex ()));
//      if(particle.end_vertex ()) ATH_MSG_DEBUG("  decay r="<<particle.end_vertex()->position().perp()<<", min="<<m_minDecayRadius[i]<<" : "<<*(particle.end_vertex ()));
  }

  return passFilter;
}

StatusCode  ISF::GenParticleSimQuasiStableFilter::finalize()
{
    ATH_MSG_VERBOSE("Finalizing ...");
    return StatusCode::SUCCESS;
}

