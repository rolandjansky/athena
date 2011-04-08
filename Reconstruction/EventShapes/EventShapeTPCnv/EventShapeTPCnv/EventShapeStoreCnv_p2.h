/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETPCNV_EVENTSHAPESTORECNV_P2_H
#define EVENTSHAPETPCNV_EVENTSHAPESTORECNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventShapeTPCnv/EventShapeStoreConverterBase.h"
#include "EventShapeTPCnv/EventShapeStore_p2.h"
#include "EventShapeEvent/EventShapeStore.h"

#include <string>
#include <vector>

template <>
class EventShapeStoreConverterBase<EventShapeStore_p2>
: public T_AthenaPoolTPCnvBase<EventShapeStore, EventShapeStore_p2>
{
 public: 
  
  EventShapeStoreConverterBase() { };
  virtual ~EventShapeStoreConverterBase() { };
  
  void persToTrans( const EventShapeStore_p2* persObj, 
		    EventShapeStore* transObj, 
		    MsgStream& reporter );
  
  /** Method creating the persistent representation @c EventShapeStore_p2
   *  from its transient representation @c EventShapeStore
   */
  void transToPers( const EventShapeStore* transObj,
		    EventShapeStore_p2* persObj, 
		    MsgStream& reporter );
  
 protected: 
};

typedef EventShapeStoreConverterBase<EventShapeStore_p2> EventShapeStoreCnv_p2;

template<>
class T_TPCnv<EventShapeStore, EventShapeStore_p2>
  : public EventShapeStoreCnv_p2
{ };

#endif
