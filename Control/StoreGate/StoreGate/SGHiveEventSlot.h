/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_SGHIVEEVENTSLOT
#define STOREGATE_SGHIVEEVENTSLOT 1
/** @class HiveEventSlot
 *  @brief A structure created by HiveMgrSvc and used by SGHiveStoreSvc
 *  to forward the StoreGateSvc method to a thread-specific SGImplSvc
 *
 *  $Id: SGHiveEventSlot.h 637398 2014-12-23 20:45:22Z calaf $
 **/
#include "tbb/recursive_mutex.h"

class SGImplSvc;

namespace SG {
  struct HiveEventSlot {
    typedef tbb::recursive_mutex mutex_t;
    friend class TestSGHiveMgrSvc;
    HiveEventSlot(SGImplSvc* pSG=0, int evt=-1) : 
      pEvtStore(pSG), eventNumber(evt) {}
    HiveEventSlot(const HiveEventSlot& parent) :
      pEvtStore(parent.pEvtStore), eventNumber(parent.eventNumber) {}
    SGImplSvc* pEvtStore;
    int eventNumber;
    mutex_t storeMutex;
  };
}
#endif // STOREGATE_SGHIVEEVENTSLOT





