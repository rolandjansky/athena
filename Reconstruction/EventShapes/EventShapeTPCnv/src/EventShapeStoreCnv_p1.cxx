/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "EventShapeTPCnv/EventShapeStoreCnv_p1.h"

#include "EventShapeEvent/EventShapeStore.h"

void
EventShapeStoreCnv_p1::transToPers(const EventShapeStore* transObj,
				   EventShapeStore_p1* persObj,
				   MsgStream &reporter )
{
  reporter << MSG::DEBUG << "EventShapeStoreCnv_p1 toPers()" << endreq;
  
  for ( EventShapeStore::store_Citer it = transObj->m_store.begin();
	it != transObj->m_store.end(); ++it )
    {
      persObj->m_mtypV.push_back(it->getType());
      persObj->m_mvalV.push_back(it->getValue());
    }
}

void
EventShapeStoreCnv_p1::persToTrans(const EventShapeStore_p1* persObj,
				   EventShapeStore* transObj,
				   MsgStream &reporter )
{
  reporter << MSG::DEBUG << "EventShapeStoreCnv toTrans()" << endreq;
  
  // make coverity happy
  // if ( ! bool(transObj) )
  // transObj = new EventShapeStore();
  transObj->m_store.clear();
  
  if ( persObj->m_mtypV.size() != persObj->m_mvalV.size() )
    reporter << MSG::WARNING << "EventShapeStore_p1 different sizes !!" << endreq;
  for ( unsigned int i = 0; i < persObj->m_mtypV.size(); ++i )
    transObj->insert(EventShapes::EventShapeType(persObj->m_mtypV.at(i)),
		     persObj->m_mvalV.at(i));
}
