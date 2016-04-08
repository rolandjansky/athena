///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleContainerCnv_p1.cxx 
// Implementation file for class CompositeParticleContainerCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "AthAllocators/DataPool.h"

// ParticleEvent includes
#define private public
#define protected public
#include "ParticleEvent/CompositeParticle.h"
#include "ParticleEvent/CompositeParticleContainer.h"
#undef private
#undef protected


// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticleCnv_p1.h"
#include "ParticleEventTPCnv/CompositeParticleContainer_p1.h"
#include "ParticleEventTPCnv/CompositeParticleContainerCnv_p1.h"

// preallocate converters
static CompositeParticleCnv_p1 s_cpCnv;

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
CompositeParticleContainerCnv_p1::persToTrans( const CompositeParticleContainer_p1* pers, 
                                  CompositeParticleContainer* trans, 
                                  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading CompositeParticleContainer from persistent state..."
//       << endreq;

  // elements are managed by DataPool
  trans->clear(SG::VIEW_ELEMENTS);

  DataPool<CompositeParticle> pool( 20 );
  const std::size_t nElems = pers->size();
  if ( pool.capacity() - pool.allocated() < nElems ) {
    pool.reserve( pool.allocated() + nElems );
  }

  trans->reserve( nElems );
  for ( CompositeParticleContainer_p1::const_iterator 
          itr    = pers->begin(),
          itrEnd = pers->end();
        itr != itrEnd;
        ++itr ) {
    CompositeParticle * p = pool.nextElementPtr();
    s_cpCnv.persToTrans( &(*itr), p, msg );
    trans->push_back( p );
  }
//   msg << MSG::DEBUG << "Loading CompositeParticleContainer from persistent state [OK]"
//       << endreq;
  return;
}

void 
CompositeParticleContainerCnv_p1::transToPers( const CompositeParticleContainer* trans, 
                                  CompositeParticleContainer_p1* pers, 
                                  MsgStream& msg ) 
{
//    msg << MSG::DEBUG
//        << "Creating persistent state of CompositeParticleContainer..."
//        << endreq;

  std::size_t size = trans->size();     
  pers->resize(size);

  // convert vector entries one by one
  CompositeParticleContainer::const_iterator transItr = trans->begin();
  CompositeParticleContainer_p1::iterator    persItr  = pers->begin();
  while(size) {
      s_cpCnv.transToPers( *transItr, &(*persItr), msg );
      ++persItr; ++transItr; --size;
  }
  
//   msg << MSG::DEBUG
//       << "Creating persistent state of CompositeParticleContainer [OK]"
//       << endreq;
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
