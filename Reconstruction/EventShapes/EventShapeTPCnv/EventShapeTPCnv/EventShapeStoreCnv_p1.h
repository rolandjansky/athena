/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETPCNV_EVENTSHAPESTORECNV_P1_H
#define EVENTSHAPETPCNV_EVENTSHAPESTORECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "EventShapeTPCnv/EventShapeStoreConverterBase.h"
#include "EventShapeTPCnv/EventShapeStore_p1.h"
#include "EventShapeEvent/EventShapeStore.h"

#include <string>
#include <vector>

template <>
class EventShapeStoreConverterBase<EventShapeStore_p1>
: public T_AthenaPoolTPCnvBase<EventShapeStore, EventShapeStore_p1>
{
 public: 
  
  EventShapeStoreConverterBase() { };
  virtual ~EventShapeStoreConverterBase() { };
  
  void persToTrans( const EventShapeStore_p1* persObj, 
		    EventShapeStore* transObj, 
		    MsgStream& reporter );
  
  /** Method creating the persistent representation @c EventShapeStore_p1
   *  from its transient representation @c EventShapeStore
   */
  void transToPers( const EventShapeStore* transObj,
		    EventShapeStore_p1* persObj, 
		    MsgStream& reporter );
  
 protected: 
};

typedef EventShapeStoreConverterBase<EventShapeStore_p1> EventShapeStoreCnv_p1;

template<>
class T_TPCnv<EventShapeStore, EventShapeStore_p1>
  : public EventShapeStoreCnv_p1
{ };

#endif
