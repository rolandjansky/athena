///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainerCnv_p1.cxx 
// Implementation file for class ParticleShallowCloneContainerCnv_p1
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 

#include "AthAllocators/DataPool.h"

// ParticleEvent includes
#define private public
#define protected public
#include "ParticleEvent/ParticleShallowClone.h"
#include "ParticleEvent/ParticleShallowCloneContainer.h"
#undef private
#undef protected


// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowCloneCnv_p1.h"
#include "ParticleEventTPCnv/ParticleShallowCloneContainer_p1.h"
#include "ParticleEventTPCnv/ParticleShallowCloneContainerCnv_p1.h"

// preallocate converters
static ParticleShallowCloneCnv_p1 s_cpCnv;

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
ParticleShallowCloneContainerCnv_p1::persToTrans( const ParticleShallowCloneContainer_p1* pers, 
                                                  ParticleShallowCloneContainer* trans, 
                                                  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading ParticleShallowCloneContainer from persistent state..."
//       << endreq;

  // elements are managed by DataPool
  trans->clear(SG::VIEW_ELEMENTS);

  DataPool<Analysis::ParticleShallowClone> pool( 20 );
  const std::size_t nElems = pers->size();
  if ( pool.capacity() - pool.allocated() < nElems ) {
    pool.reserve( pool.allocated() + nElems );
  }

  trans->reserve( nElems );
  for ( ParticleShallowCloneContainer_p1::const_iterator 
          itr    = pers->begin(),
          itrEnd = pers->end();
        itr != itrEnd;
        ++itr )
    {
      Analysis::ParticleShallowClone * p = pool.nextElementPtr();
      s_cpCnv.persToTrans( &(*itr), p, msg );
      trans->push_back( p );
    }
//   msg << MSG::DEBUG << "Loading ParticleShallowCloneContainer from persistent state [OK]"
//       << endreq;
  return;
}

void 
ParticleShallowCloneContainerCnv_p1::transToPers( const ParticleShallowCloneContainer* trans, 
                                                  ParticleShallowCloneContainer_p1* pers, 
                                                  MsgStream& msg ) 
{
//    msg << MSG::DEBUG
//        << "Creating persistent state of ParticleShallowCloneContainer..."
//        << endreq;

  std::size_t size = trans->size();     
  pers->resize(size);

  // convert vector entries one by one
  ParticleShallowCloneContainer::const_iterator transItr = trans->begin();
  ParticleShallowCloneContainer_p1::iterator    persItr  = pers->begin();
  while(size)
    {
      s_cpCnv.transToPers( *transItr, &(*persItr), msg );
      ++persItr; ++transItr; --size;
    }
  
//   msg << MSG::DEBUG
//       << "Creating persistent state of ParticleShallowCloneContainer [OK]"
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
