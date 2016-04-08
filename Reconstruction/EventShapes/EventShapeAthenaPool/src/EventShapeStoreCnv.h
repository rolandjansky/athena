/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPEATHENAPOOL_EVENTSHAPESTORECNV_H
#define EVENTSHAPEATHENAPOOL_EVENTSHAPESTORECNV_H

#include "EventShapeTPCnv/EventShapeStoreCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "EventShapeTPCnv/EventShapeStore_p1.h"

#include "EventShapeEvent/EventShapeStore.h"

#include <string>
#include <vector>

// the latest persistent representation type of EventShapeStore_p1
typedef  EventShapeStore_p1  EventShapeStore_PERS;
typedef  T_AthenaPoolCustomCnv<EventShapeStore, EventShapeStore_PERS>  EventShapeStoreCnvBase;

class EventShapeStoreCnv : public EventShapeStoreCnvBase
{
  friend class CnvFactory<EventShapeStoreCnv>;
  
 protected:

  EventShapeStoreCnv (ISvcLocator* svcloc) : EventShapeStoreCnvBase(svcloc) {};
  virtual EventShapeStore_PERS*   createPersistent (EventShapeStore* transObj);
  virtual EventShapeStore*        createTransient ();
  
};

#endif // EVENTSHAPEATHENAPOOL_EVENTSHAPESTORECNV_H
