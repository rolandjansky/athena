///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv_p6.cxx
// Implementation file for class JetCollectionCnv_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// DataModel includes
#include "DataModel/DataPool.h"

// JetEvent includes
#define private public
#define protected public
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
#undef private
#undef protected


// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p6.h"
#include "JetEventTPCnv/JetCollection_p6.h"
#include "JetEventTPCnv/JetCollectionCnv_p6.h"
#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/DataLinkCnv_p2.h"

// preallocate converters
static JetCnv_p6 jetCnv;
static JetKeyDescriptorCnv_p1 jetkeyCnv;

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
JetCollectionCnv_p6::persToTrans( const JetCollection_p6* pers, 
                                  JetCollection* trans, 
                                  MsgStream& msg ) 
{
  msg << MSG::DEBUG << "Loading JetCollection from persistent state..."
      << endreq;
  
  // make sure to first read the JetKeyDescriptor
  DataLinkCnv_p2<DataLink<JetKeyDescriptor> > JetKeyStoreCnv;
  JetKeyStoreCnv.persToTrans( &pers->m_keyStore, &trans->m_keyStore, msg );
  // link the JetKeyDescriptorInstance to the store:
  if( trans->m_keyStore.isValid() ){
    trans->m_keyDescInstance.m_Stores  = trans->m_keyStore.getDataNonConstPtr();
    // make sure the global instance is pointing to this jetkey store
    JetKeyDescriptorInstance::instance()->m_Stores =  trans->m_keyDescInstance.m_Stores;
  }
  else if (trans->m_keyStore.isDefault()) {
    DataLink<JetKeyDescriptor> dl ("JetKeyMap");
    if (dl.isValid()) {
      trans->m_keyDescInstance.m_Stores  = dl.getDataNonConstPtr();
      // make sure the global instance is pointing to this jetkey store
      JetKeyDescriptorInstance::instance()->m_Stores =  trans->m_keyDescInstance.m_Stores;
    }
    else
      trans->m_keyDescInstance.m_Stores  =
        JetKeyDescriptorInstance::instance()->m_Stores;
  }
  else
    trans->m_keyDescInstance.m_Stores  = JetKeyDescriptorInstance::instance()->m_Stores;

  msg <<  MSG::DEBUG << "attached JetKeyDescriptor to its instance" << endreq;
  
  /// Individual items

  trans->m_ordered   = static_cast<JetCollection::OrderedVar>(pers->m_ordered);
  // not used any more ... trans->m_ROIauthor = //pers->m_roiAuthor;

  /// The jets them selves. Taken care of behind our backs.

  for (JetCollection::iterator it = trans->begin();
       it != trans->end();
       it++)
  {
    (*it)->setJetId(Jet::s_defaultJetId);
  }
    
  trans->clear(); 
  trans->reserve(pers->size());

  trans->m_jetAuthor =  pers->m_jetAuthor;
  trans->m_ownMap = 0;

  // std::cout << " JetCollectionCnv_p6  Converting "<< trans->author() << std::endl;
  // std::cout << " JetCollectionCnv_p6  dalink "<< trans->m_momentMapLink.isValid() << "  "<< trans->m_momentMapLink.dataID() << std::endl;

  for (JetCollection_p6::const_iterator itr = pers->begin();
       itr != pers->end();
       itr++) {
    Jet* j = createTransFromPStore((ITPConverterFor<Jet>**)0, *itr, msg);
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
      << endreq;
  return;
}

void 
JetCollectionCnv_p6::transToPers( const JetCollection* trans, 
                                  JetCollection_p6* pers, 
                                  MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection..."
//       << endreq;

  pers->m_ordered   = static_cast<short>(trans->m_ordered);
  // not used any more ... //pers->m_roiAuthor = trans->m_ROIauthor;
  pers->m_jetAuthor = trans->m_jetAuthor ;

  pers->m_jetIdBugFixed = true; // the bug is now fixed

  pers->clear();
  pers->reserve(trans->size());

  for (JetCollection::const_iterator itr = trans->begin();
       itr != trans->end();
       itr++) {
    pers->push_back(toPersistent((ITPConverterFor<Jet>**)0, *itr, msg));
  }

  // RS now deal with the JetKeyDescriptor
  DataLinkCnv_p2<DataLink<JetKeyDescriptor> > JetKeyStoreCnv;
  JetKeyStoreCnv.transToPers( &trans->m_keyStore, &pers->m_keyStore, msg );
  
//   msg << MSG::DEBUG << "Creating persistent state of JetCollection [OK]"
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
