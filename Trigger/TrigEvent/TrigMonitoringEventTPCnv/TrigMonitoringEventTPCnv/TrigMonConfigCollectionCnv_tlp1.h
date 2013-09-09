/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_COLLECTIONCNV_TLP1_H
#define TRIGMON_CONFIG_COLLECTIONCNV_TLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_tlp1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollectionCnv_p1.h"   

 
class TrigMonConfigCollectionCnv_tlp1 
   : public AthenaPoolTopLevelTPConverter<TrigMonConfigCollectionCnv_p1,
					  TrigMonConfigCollection_tlp1>
{ 
 public:
 
  TrigMonConfigCollectionCnv_tlp1();
  virtual ~TrigMonConfigCollectionCnv_tlp1() {}
 
  void setPStorage(TrigMonConfigCollection_tlp1 *storage);
  
 protected: 
  
  TrigMonConfigCollectionCnv_p1    m_TrigMonConfigCollectionCnv;
  TrigMonConfigCnv_p1              m_TrigMonConfigCnv; 
};
 
#endif
