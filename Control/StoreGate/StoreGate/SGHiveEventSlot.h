/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_SGHIVEEVENTSLOT
#define STOREGATE_SGHIVEEVENTSLOT 1
/** @class HiveEventSlot
 *  @brief A structure created by HiveMgrSvc and used by SGHiveStoreSvc
 *  to forward the StoreGateSvc method to a thread-specific SGImplSvc
 *
 *  $Id: SGHiveEventSlot.h 797595 2017-02-16 18:36:10Z ssnyder $
 **/
#include <mutex>
#include <memory>

class SGImplSvc;

namespace SG {
  struct HiveEventSlot {
    friend class TestSGHiveMgrSvc;
    HiveEventSlot(SGImplSvc* pSG=0, int evt=-1) : 
      pEvtStore(pSG), eventNumber(evt) {}
    SGImplSvc* pEvtStore;
    int eventNumber;
  };
}
#endif // STOREGATE_SGHIVEEVENTSLOT





