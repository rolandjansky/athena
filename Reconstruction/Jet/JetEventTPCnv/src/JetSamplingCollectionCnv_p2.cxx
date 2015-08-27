///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollectionCnv_p2.cxx 
// Implementation file for class JetSamplingCollectionCnv_p2
// Created by Belen.Salvachua@cern.ch based on JetSamplingCollectionCnv_p1.cxx from
//    S.Binet<binet@cern.ch>
// Date: Dec 2008
/////////////////////////////////////////////////////////////////// 

// DataModel includes
#include "DataModel/DataPool.h"

// JetEvent includes
#define private public
#define protected public
#include "JetEvent/JetSampling.h"
#include "JetEvent/JetSamplingCollection.h"
#undef private
#undef protected


// JetEventTPCnv includes
#include "JetEventTPCnv/JetSamplingCnv_p2.h"
#include "JetEventTPCnv/JetSamplingCollection_p2.h"
#include "JetEventTPCnv/JetSamplingCollectionCnv_p2.h"

// preallocate converters
static JetSamplingCnv_p2 jetSamplingCnv;

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
JetSamplingCollectionCnv_p2::persToTrans( const JetSamplingCollection_p2* pers, 
					  JetSamplingCollection* trans, 
					  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading JetSamplingCollection from persistent state..."
//       << endreq;


  // elements are managed by DataPool
  trans->clear(SG::VIEW_ELEMENTS);

  DataPool<JetSampling> pool( 20 );
  const std::size_t nJetSampling = pers->size();
  if ( pool.capacity() - pool.allocated() < nJetSampling ) {
    pool.reserve( pool.allocated() + nJetSampling );
  }

  trans->reserve( nJetSampling );
  for ( JetSamplingCollection_p2::const_iterator 
          itr    = pers->begin(),
          itrEnd = pers->end();
        itr != itrEnd;
        ++itr ) {
    JetSampling * jetSampling = pool.nextElementPtr();
    jetSamplingCnv.persToTrans( &(*itr), jetSampling, msg );
    trans->push_back( jetSampling );
  }
//   msg << MSG::DEBUG << "Loading JetSamplingCollection from persistent state [OK]"
//       << endreq;
  return;
}

void 
JetSamplingCollectionCnv_p2::transToPers( const JetSamplingCollection* trans, 
					  JetSamplingCollection_p2* pers, 
					  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of JetSamplingCollection..."
//       << endreq;

  std::size_t size = trans->size();     
  pers->resize(size);

  // convert vector entries one by one
  JetSamplingCollection::const_iterator transItr = trans->begin();
  JetSamplingCollection_p2::iterator    persItr  = pers->begin();
  while(size) {
    jetSamplingCnv.transToPers( *transItr, &(*persItr), msg );
    ++persItr; ++transItr; --size;
  }

//   msg << MSG::DEBUG << "Creating persistent state of JetSamplingCollection [OK]"
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
