///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p3.cxx 
// Implementation file for class JetCollectionCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// DataModel includes
#include "AthAllocators/DataPool.h"

// JetEvent includes
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"


// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p3.h"
#include "JetEventTPCnv/JetCollection_p3.h"
#include "JetEventTPCnv/JetCollectionCnv_p3.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include <sstream>

// preallocate converters
static const JetCnv_p3 jetCnv;
static const JetKeyDescriptorCnv_p1 jetkeyCnv;


void 
JetCollectionCnv_p3::persToTrans( const JetCollection_p3* pers, 
                                  JetCollection* trans, 
                                  MsgStream& msg ) const
{
  msg << MSG::DEBUG << "Loading JetCollection from persistent state..."
      << endmsg;
  
  // make sure to first read the JetKeyDescriptor
  DataLinkCnv_p1<DataLink<JetKeyDescriptor> > JetKeyStoreCnv;
  JetKeyStoreCnv.persToTrans( &pers->m_keyStore, &trans->m_keyStore, msg );
  // link the JetKeyDescriptorInstance to the store:
  if( trans->m_keyStore.isValid() ){
    trans->keyDesc()->m_Stores  = nullptr;
    trans->keyDesc()->m_ConstStores  = trans->m_keyStore.cptr();
    // make sure the global instance is pointing to this jetkey store
    JetKeyDescriptorInstance::instance()->m_Stores =  nullptr;
    JetKeyDescriptorInstance::instance()->m_ConstStores =  trans->keyDesc()->m_ConstStores;
  }
  else if (trans->m_keyStore.isDefault()) {
    DataLink<JetKeyDescriptor> dl ("JetKeyMap");
    if (dl.isValid()) {
      trans->keyDesc()->m_Stores  = nullptr;
      trans->keyDesc()->m_ConstStores  = dl.cptr();
      // make sure the global instance is pointing to this jetkey store
      JetKeyDescriptorInstance::instance()->m_Stores =  nullptr;
      JetKeyDescriptorInstance::instance()->m_ConstStores =  trans->keyDesc()->m_ConstStores;
    }
    else {
      trans->keyDesc()->m_Stores  =
        JetKeyDescriptorInstance::instance()->m_Stores;
      trans->keyDesc()->m_ConstStores  =
        JetKeyDescriptorInstance::instance()->m_ConstStores;
    }
  }
  else {
    trans->keyDesc()->m_Stores  = JetKeyDescriptorInstance::instance()->m_Stores;
    trans->keyDesc()->m_ConstStores  = JetKeyDescriptorInstance::instance()->m_ConstStores;
  }
  msg <<  MSG::DEBUG << "attached JetKeyDescriptor to its instance" << endmsg;
  
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

  msg << MSG::DEBUG << "persistant JetCollection  has size " << nJets <<endmsg;

  trans->reserve( nJets );
  for ( JetCollection_p3::const_iterator 
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
  
  msg << MSG::DEBUG << "Loading JetCollection from persistent state [OK]"
      << endmsg;
  return;
}

void 
JetCollectionCnv_p3::transToPers( const JetCollection* trans, 
                                  JetCollection_p3* pers, 
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
  JetCollection_p3::iterator    persItr  = pers->begin();
  while(size) {
      jetCnv.transToPers( *transItr, &(*persItr), msg );
      ++persItr; ++transItr; --size;
  }
  
  // RS now deal with the JetKeyDescriptor
  DataLinkCnv_p1<DataLink<JetKeyDescriptor> > JetKeyStoreCnv;
  JetKeyStoreCnv.transToPers( &trans->m_keyStore, &pers->m_keyStore, msg );
  
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection [OK]"
//       << endmsg;
  return;
}
