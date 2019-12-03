///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv_p1.cxx 
// Implementation file for class AthExParticlesCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getThinningCache.h"
#include "AthenaKernel/ThinningDecisionBase.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExParticlesCnv_p1.h"


void 
AthExParticlesCnv_p1::persToTransWithKey( const AthExParticles_p1* persObj, 
                                          AthExParticles* transObj, 
                                          const std::string& /*key*/,
                                          MsgStream &msg ) const
{
  msg << MSG::DEBUG 
      << "Loading Particles from persistent state..."
      << endmsg;

  const std::vector<AthExParticle_p1>& particles = persObj->m_particles;
  const std::size_t nMax = particles.size();
  transObj->reserve( nMax );

  for (const AthExParticle_p1& p : particles) {
    transObj->push_back( std::make_unique<AthExParticle>( p.m_px,
                                                          p.m_py,
                                                          p.m_pz,
                                                          p.m_ene ) );
  }

  msg << MSG::DEBUG 
      << "Loaded Particles from persistent state [OK]"
      << endmsg;
  return;
}


void 
AthExParticlesCnv_p1::transToPersWithKey( const AthExParticles* transObj, 
                                          AthExParticles_p1* persObj,
                                          const std::string& key,
                                          MsgStream &msg ) const
{
  msg << MSG::DEBUG 
      << "Creating persistent state of Particles..."
      << endmsg;

  const SG::ThinningDecisionBase* dec =
    SG::getThinningDecision (key);

  for ( unsigned int i = 0; i != transObj->size(); ++i ) {
    if (dec && dec->thinned(i)) continue;
    const AthExParticle * p = (*transObj)[i];
    if ( 0 == p ) {
      std::cerr << "## skipping element [" << i << "] ##" << std::endl;
      continue;
    }
    
    persObj->m_particles.emplace_back( p->px(),
                                       p->py(),
                                       p->pz(),
                                       p->e() );
  }

  msg << MSG::DEBUG 
      << "Created persistent state of Particles [OK]"
      << endmsg;
  return;
}
