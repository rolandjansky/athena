/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_COLLECTIONCNV_P1_H
#define TRIGMON_CONFIG_COLLECTIONCNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1.h"
 
class TrigMonConfigCollectionCnv_p1 
   : public T_AthenaPoolTPPtrVectorCnv<TrigMonConfigCollection,
				       TrigMonConfigCollection_p1,
				       TrigMonConfigCnv_p1>
{
 public:

  TrigMonConfigCollectionCnv_p1() {}
  virtual ~TrigMonConfigCollectionCnv_p1() {} 
};
 
#endif
