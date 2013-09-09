/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_COLLECTIONCNV_TLP1_H
#define TRIGMON_EVENT_COLLECTIONCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_tlp1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollectionCnv_p1.h"   

 
class TrigMonEventCollectionCnv_tlp1 
   : public AthenaPoolTopLevelTPConverter<TrigMonEventCollectionCnv_p1,
					  TrigMonEventCollection_tlp1>
{ 
 public:
 
  TrigMonEventCollectionCnv_tlp1();
  virtual ~TrigMonEventCollectionCnv_tlp1() {}
 
  void setPStorage(TrigMonEventCollection_tlp1 *storage);
  
 protected: 
  
  TrigMonEventCollectionCnv_p1    m_TrigMonEventCollectionCnv;
  TrigMonEventCnv_p1              m_TrigMonEventCnv; 
};

#endif
