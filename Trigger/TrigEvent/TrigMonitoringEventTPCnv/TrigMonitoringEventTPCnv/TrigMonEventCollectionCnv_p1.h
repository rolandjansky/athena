/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_COLLECTIONCNV_P1_H
#define TRIGMON_EVENT_COLLECTIONCNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMonitoringEvent/TrigMonEventCollection.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1.h"
 
class TrigMonEventCollectionCnv_p1 
   : public T_AthenaPoolTPPtrVectorCnv<TrigMonEventCollection, 
				       TrigMonEventCollection_p1,
				       TrigMonEventCnv_p1>
{ 
 public:

  TrigMonEventCollectionCnv_p1() {}
  virtual ~TrigMonEventCollectionCnv_p1() {} 
};
 
#endif
