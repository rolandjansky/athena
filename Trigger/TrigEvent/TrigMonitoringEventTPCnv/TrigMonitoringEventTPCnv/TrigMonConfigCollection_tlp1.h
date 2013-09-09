/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_COLLECTION_TLP1_H
#define TRIGMON_CONFIG_COLLECTION_TLP1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent "top level" copy of TrigMonConfigCollection;
*/

#include <vector>
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfig_p1.h"

class TrigMonConfigCollection_tlp1
{  
 public:

  TrigMonConfigCollection_tlp1() {}
  ~TrigMonConfigCollection_tlp1() {}

  friend class TrigMonConfigCollectionCnv_tlp1;

 private:
  
  std::vector<TrigMonConfigCollection_p1>   m_TrigMonConfigCollection;
  std::vector<TrigMonConfig_p1>             m_TrigMonConfig;

};

#endif
