/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "EventShapeTPCnv/EventShapeStoreCnv_p2.h"

#include "EventShapeEvent/EventShapeStore.h"

void
EventShapeStoreCnv_p2::transToPers(const EventShapeStore* transObj,
				   EventShapeStore_p2* persObj,
				   MsgStream &reporter )
{
  reporter << MSG::DEBUG << "EventShapeStoreCnv_p2 toPers()" << endreq;
  
  persObj->m_mvalM.resize(transObj->m_store.size());
  // for ( EventShapeStore::store_Citer it = transObj->m_store.begin();
  // it != transObj->m_store.end(); ++it )
  for ( unsigned int index(0); index < transObj->m_store.size(); ++index )
    {
      persObj->m_mtypV.push_back( transObj->m_store[index].getType() );
      persObj->m_mvalM[index].resize(transObj->m_store[index].m_values.size());
      for( unsigned int i=0; i < transObj->m_store[index].m_values.size(); ++i )
	persObj->m_mvalM[index][i] = transObj->m_store[index].getValue(i);
    }
}

void
EventShapeStoreCnv_p2::persToTrans(const EventShapeStore_p2* persObj,
				   EventShapeStore* transObj,
				   MsgStream &reporter )
{
  reporter << MSG::DEBUG << "EventShapeStoreCnv toTrans()" << endreq;
  
  // make coverity happy
  // if ( ! bool(transObj) )
  // transObj = new EventShapeStore();
  transObj->m_store.clear();
  
  // if ( persObj->m_mtypV.size() != persObj->m_mvalV.size() )
  // reporter << MSG::WARNING << "EventShapeStore_p2 different sizes !!" << endreq;
  std::vector<double> tmp;
  
  for ( unsigned int i = 0; i < persObj->m_mtypV.size(); ++i )	// loop through every ES type for type and value vectors
    {
      tmp.resize(persObj->m_mvalM[i].size());
      for (unsigned int j(0); j < persObj->m_mvalM[i].size(); ++j)
        tmp[j] = persObj->m_mvalM[i][j];
      transObj->insert(EventShapes::EventShapeType(persObj->m_mtypV.at(i)), tmp, true);
    } 
}
