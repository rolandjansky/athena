///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p6.cxx
// Implementation file for class JetCollectionCnv_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// DataModel includes
#include "AthAllocators/DataPool.h"

// JetEvent includes
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"


// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p6.h"
#include "JetEventTPCnv/JetCollection_p6.h"
#include "JetEventTPCnv/JetCollectionCnv_p6.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

// preallocate converters
static const JetCnv_p6 jetCnv;
static const JetKeyDescriptorCnv_p1 jetkeyCnv;


void 
JetCollectionCnv_p6::persToTrans( const JetCollection_p6* pers, 
                                  JetCollection* trans, 
                                  MsgStream& msg ) const
{
  msg << MSG::DEBUG << "Loading JetCollection from persistent state..."
      << endmsg;
  
  // make sure to first read the JetKeyDescriptor
  DataLinkCnv_p2<DataLink<JetKeyDescriptor> > JetKeyStoreCnv;
  JetKeyStoreCnv.persToTrans( &pers->m_keyStore, &trans->m_keyStore, msg );
  // link the JetKeyDescriptorInstance to the store:
  if( trans->m_keyStore.isValid() ){
    trans->keyDesc()->m_Stores  = trans->m_keyStore.getDataNonConstPtr();
    // make sure the global instance is pointing to this jetkey store
    JetKeyDescriptorInstance::instance()->m_Stores =  trans->keyDesc()->m_Stores;
  }
  else if (trans->m_keyStore.isDefault()) {
    DataLink<JetKeyDescriptor> dl ("JetKeyMap");
    if (dl.isValid()) {
      trans->keyDesc()->m_Stores  = dl.getDataNonConstPtr();
      // make sure the global instance is pointing to this jetkey store
      JetKeyDescriptorInstance::instance()->m_Stores =  trans->keyDesc()->m_Stores;
    }
    else
      trans->keyDesc()->m_Stores  =
        JetKeyDescriptorInstance::instance()->m_Stores;
  }
  else
    trans->keyDesc()->m_Stores  = JetKeyDescriptorInstance::instance()->m_Stores;

  msg <<  MSG::DEBUG << "attached JetKeyDescriptor to its instance" << endmsg;
  
  /// Individual items

  trans->setOrdered (static_cast<JetCollection::OrderedVar>(pers->m_ordered));
  // not used any more ... trans->m_ROIauthor = //pers->m_roiAuthor;

  /// The jets themselves. Taken care of behind our backs.

  for (Jet* jet : *trans) {
    jet->setJetId(Jet::s_defaultJetId);
  }
    
  trans->clear(); 
  trans->reserve(pers->size());

  trans->m_jetAuthor =  pers->m_jetAuthor;
  trans->m_ownMap = 0;

  // std::cout << " JetCollectionCnv_p6  Converting "<< trans->author() << std::endl;
  // std::cout << " JetCollectionCnv_p6  dalink "<< trans->m_momentMapLink.isValid() << "  "<< trans->m_momentMapLink.dataID() << std::endl;

  for (const TPObjRef& ref : *pers) {
    Jet* j = createTransFromPStore((ITPConverterFor<Jet>**)0, ref, msg);
    size_t id = j->m_jetId;
    trans->push_back(j);
    //std::cout << " reading jet "<< id << "  "<< pers->m_jetIdBugFixed << std::endl;
    if(pers->m_jetIdBugFixed) j->setJetId(id); // when the bug wasn't fixed id is wrong, so don't set it.
    // associate this collection to its jets :
    trans->back()->m_collection = trans ;
  }
  //std::cout << " JetCollectionCnv_p6  dalink "<< trans->m_momentMapLink.isValid() << "  "<< trans->m_momentMapLink.dataID() << std::endl;


  // Force PseudoJet pointer to null
  trans->setFastjetClustSeq(NULL);

  msg << MSG::DEBUG << "Loading JetCollection from persistent state [OK]"
      << endmsg;
  return;
}

void 
JetCollectionCnv_p6::transToPers( const JetCollection* trans, 
                                  JetCollection_p6* pers, 
                                  MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection..."
//       << endmsg;

  pers->m_ordered   = static_cast<short>(trans->ordered());
  // not used any more ... //pers->m_roiAuthor = trans->m_ROIauthor;
  pers->m_jetAuthor = trans->m_jetAuthor ;

  pers->m_jetIdBugFixed = true; // the bug is now fixed

  pers->clear();
  pers->reserve(trans->size());

  for (const Jet* jet : *trans) {
    pers->push_back(toPersistent((ITPConverterFor<Jet>**)0, jet, msg));
  }

  // RS now deal with the JetKeyDescriptor
  DataLinkCnv_p2<DataLink<JetKeyDescriptor> > JetKeyStoreCnv;
  JetKeyStoreCnv.transToPers( &trans->m_keyStore, &pers->m_keyStore, msg );
  
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection [OK]"
//       << endmsg;
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
