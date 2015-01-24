/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCTimingManager.hh,v 1.1.1.10 2003-11-03 10:18:18 mishino Exp $
#ifndef TGCTimingManager_hh
#define TGCTimingManager_hh

namespace LVL1TGCTrigger {

class TGCSector;
class TGCDatabaseManager;

class TGCTimingManager {

public:
  TGCTimingManager();
  ~TGCTimingManager(){};

  void startPatchPanel(TGCSector* sector, TGCDatabaseManager* db=0);
  void startSlaveBoard(TGCSector* sector);
  void startHighPtBoard(TGCSector* sector);
  void startSectorLogic(TGCSector* sector);
//  void startSectorLogic(TGCSector* sector,double eta);//for test

  void setBunchCounter(int bc){ bunchCounter = bc;};
  int getBunchCounter(){ return bunchCounter; }
  void increaseBunchCounter(){ bunchCounter++;};

private:
  int bunchCounter;
};


} //end of namespace bracket

#endif
