/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCTimingManager.hh,v 1.1.1.10 2003-11-03 10:18:18 mishino Exp $
#ifndef TGCTimingManager_hh
#define TGCTimingManager_hh

#include "StoreGate/ReadCondHandleKey.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {

class TGCSector;
class TGCDatabaseManager;

class TGCTimingManager {

public:
  TGCTimingManager (const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey);
  ~TGCTimingManager() = default;

  void startPatchPanel(TGCSector* sector, TGCDatabaseManager* db=0);
  void startSlaveBoard(TGCSector* sector);
  void startHighPtBoard(TGCSector* sector);
  void startSectorLogic(TGCSector* sector);
//  void startSectorLogic(TGCSector* sector,double eta);//for test

  void setBunchCounter(int bc){ m_bunchCounter = bc;};
  int getBunchCounter(){ return m_bunchCounter; }
  void increaseBunchCounter(){ m_bunchCounter++;};

private:
  int m_bunchCounter;
  const SG::ReadCondHandleKey<TGCTriggerData>& m_readCondKey;
};


} //end of namespace bracket

#endif
