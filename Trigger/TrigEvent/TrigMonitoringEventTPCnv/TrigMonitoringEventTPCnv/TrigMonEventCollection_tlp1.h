/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_COLLECTION_TLP1_H
#define TRIGMON_EVENT_COLLECTION_TLP1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent "top level" copy of TrigMonEventCollection;
*/

#include <vector>
#include "TrigMonitoringEventTPCnv/TrigMonEventCollection_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEvent_p1.h"

class TrigMonEventCollection_tlp1
{  
 public:

  TrigMonEventCollection_tlp1() {}
  ~TrigMonEventCollection_tlp1() {}

  friend class TrigMonEventCollectionCnv_tlp1;

 private:
  
  std::vector<TrigMonEventCollection_p1>   m_TrigMonEventCollection;
  std::vector<TrigMonEvent_p1>             m_TrigMonEvent;

};

#endif
