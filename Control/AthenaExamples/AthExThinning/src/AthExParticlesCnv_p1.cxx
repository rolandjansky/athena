///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv_p1.cxx 
// Implementation file for class AthExParticlesCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExParticlesCnv_p1.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
AthExParticlesCnv_p1::persToTrans( const AthExParticles_p1* persObj, 
				   AthExParticles* transObj, 
				   MsgStream &msg ) 
{
  msg << MSG::DEBUG 
      << "Loading Particles from persistent state..."
      << endreq;

  const std::vector<AthExParticle_p1>& particles = persObj->m_particles;
  const std::size_t nMax = particles.size();
  transObj->reserve( nMax );

  for ( unsigned int i = 0; i != nMax; ++i ) {
    const AthExParticle_p1& p = particles[i];
    transObj->push_back( new AthExParticle( p.m_px,
					    p.m_py,
					    p.m_pz,
					    p.m_ene ) );
  }

  msg << MSG::DEBUG 
      << "Loaded Particles from persistent state [OK]"
      << endreq;
  return;
}

void 
AthExParticlesCnv_p1::transToPers( const AthExParticles* transObj, 
				   AthExParticles_p1* persObj, 
				   MsgStream &msg )
{
  msg << MSG::DEBUG 
      << "Creating persistent state of Particles..."
      << endreq;

  for ( unsigned int i = 0; i != transObj->size(); ++i ) {
    const AthExParticle * p = (*transObj)[i];
    if ( 0 == p ) {
      std::cerr << "## skipping element [" << i << "] ##" << std::endl;
      continue;
    }
    
    persObj->m_particles.push_back( AthExParticle_p1( p->px(),
						      p->py(),
						      p->pz(),
						      p->e() ) );
  }

  msg << MSG::DEBUG 
      << "Created persistent state of Particles [OK]"
      << endreq;
  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

