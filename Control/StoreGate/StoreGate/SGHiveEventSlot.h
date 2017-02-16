/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_SGHIVEEVENTSLOT
#define STOREGATE_SGHIVEEVENTSLOT 1
/** @class HiveEventSlot
 *  @brief A structure created by HiveMgrSvc and used by SGHiveStoreSvc
 *  to forward the StoreGateSvc method to a thread-specific SGImplSvc
 *
 *  $Id: SGHiveEventSlot.h 794852 2017-01-31 23:24:04Z leggett $
 **/
#include <mutex>
#include <memory>

class SGImplSvc;

namespace SG {
  struct HiveEventSlot {
    typedef std::recursive_mutex mutex_t;
    friend class TestSGHiveMgrSvc;
    HiveEventSlot(SGImplSvc* pSG=0, int evt=-1) : 
      pEvtStore(pSG), eventNumber(evt), storeMutex(new mutex_t) {}
    SGImplSvc* pEvtStore;
    int eventNumber;
    std::unique_ptr<mutex_t> storeMutex;
  };
}
#endif // STOREGATE_SGHIVEEVENTSLOT





