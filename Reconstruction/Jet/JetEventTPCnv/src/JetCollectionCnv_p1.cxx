///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p1.cxx 
// Implementation file for class JetCollectionCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// DataModel includes
#include "AthAllocators/DataPool.h"

// JetEvent includes
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"


// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p1.h"
#include "JetEventTPCnv/JetCollection_p1.h"
#include "JetEventTPCnv/JetCollectionCnv_p1.h"

#include <sstream>

// preallocate converters
static const JetCnv_p1 jetCnv;


void 
JetCollectionCnv_p1::persToTrans( const JetCollection_p1* pers, 
                                  JetCollection* trans, 
                                  MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading JetCollection from persistent state..."
//       << endmsg;

  // elements are managed by DataPool
  trans->clear(SG::VIEW_ELEMENTS);

  DataPool<Jet> pool( 20 );
  const std::size_t nJets = pers->size();
  if ( pool.capacity() - pool.allocated() < nJets ) {
    pool.reserve( pool.allocated() + nJets );
  }

  trans->setOrdered (static_cast<JetCollection::OrderedVar>(pers->m_ordered));
  // the transient version does not have this data member any more,
  // each jet knows its ROI
  // trans->m_ROIauthor = //pers->m_roiAuthor;

  trans->reserve( nJets );
  for ( JetCollection_p1::const_iterator 
          itr    = pers->begin(),
          itrEnd = pers->end();
        itr != itrEnd;
        ++itr ) {
    Jet * jet = pool.nextElementPtr();
    jetCnv.persToTrans( &(*itr), jet, msg );
    trans->push_back( jet );
  }

  // now each jet knows its ROI
  unsigned int RoIWord;
  std::stringstream strm(pers->m_roiAuthor);
  strm >> RoIWord; 
  if( strm.good() )
    {
      msg <<  MSG::DEBUG << "Note: This jet collection uses RoIWords!" << endmsg;
      for( JetCollection::iterator itr = trans->begin(); itr != trans->end(); ++itr )
	(*itr)->set_RoIword( RoIWord );
    }
//   msg << MSG::DEBUG << "Loading JetCollection from persistent state [OK]"
//       << endmsg;
  return;
}

void 
JetCollectionCnv_p1::transToPers( const JetCollection* trans, 
                                  JetCollection_p1* pers, 
                                  MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection..."
//       << endmsg;

  pers->m_ordered   = static_cast<short>(trans->ordered());
  //pers->m_roiAuthor = trans->m_ROIauthor;

  std::size_t size = trans->size();     
  pers->resize(size);

  // convert vector entries one by one
  JetCollection::const_iterator transItr = trans->begin();
  JetCollection_p1::iterator    persItr  = pers->begin();
  while(size) {
      jetCnv.transToPers( *transItr, &(*persItr), msg );
      ++persItr; ++transItr; --size;
  }
  
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection [OK]"
//       << endmsg;
  return;
}

