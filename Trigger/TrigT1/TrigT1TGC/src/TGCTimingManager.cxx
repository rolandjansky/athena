/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSector.h"
#include "TrigT1TGC/TGCPatchPanel.h"
#include "TrigT1TGC/TGCSlaveBoard.h"
#include "TrigT1TGC/TGCDoubletSB.h"
#include "TrigT1TGC/TGCWireTripletSB.h"
#include "TrigT1TGC/TGCStripTripletSB.h"
#include "TrigT1TGC/TGCHighPtBoard.h"
#include "TrigT1TGC/TGCWireHighPtBoard.h"
#include "TrigT1TGC/TGCStripHighPtBoard.h"
#include "TrigT1TGC/TGCSectorLogic.h"
#include "TrigT1TGC/TGCDatabaseManager.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include <iostream>

namespace LVL1TGCTrigger {

 
TGCTimingManager::TGCTimingManager(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey)
  : m_bunchCounter(0),
    m_readCondKey (readCondKey)
{
}

void TGCTimingManager::startPatchPanel(TGCSector* sector, TGCDatabaseManager* db)
{
#ifdef TGCDEBUG
  std::cout << " Start PatchPanel " 
	    << "or sector# " << sector->getSideId() << ":"
	    << sector->getOctantId() << ":"
	    << sector->getModuleId() 
	    << std::endl;
#endif

  // start BID by first clockIn, start OrLogic by second clockIn.
  for( int iClk=0; iClk<2; iClk+=1) {
    for(int j=0; j<TGCSector::NumberOfPatchPanelType; j+=1) {
      for (unsigned int i=0; i<sector->getNumberOfPP(j); i+=1) {
	TGCPatchPanel *pp = sector->getPP(j,i);
	if(pp) pp->clockIn(m_bunchCounter, db);
      }
    }
  }
}

void TGCTimingManager::startSlaveBoard(TGCSector* sector)
{
#ifdef TGCDEBUG
  std::cout << " Start SlaveBoard " 
	    << "for sector# " << sector->getSideId() << ":"
	    << sector->getOctantId() << ":"
	    << sector->getModuleId() 
	    << std::endl;
#endif
  
  for(int j=0; j<NumberOfSlaveBoardType; j+=1){
    for(unsigned int i=0; i<sector->getNumberOfSB(j); i+=1){
      TGCSlaveBoard* sb = sector->getSB(j,i);
      if(sb)sb->clockIn(m_bunchCounter);
    }
  }
}

void TGCTimingManager::startHighPtBoard(TGCSector* sector)
{
  if(sector->hasHit() == false) return;

  // collect input signals by 1st clockIn,
  // perform coincidence by 2nd clockIn.
  for(int iClk=0; iClk<2; iClk+=1) {
    for(int j=0; j<TGCSector::NumberOfHighPtBoardType; j+=1) {
      for(unsigned int i=0; i<sector->getNumberOfHPB(j); i+=1) {
        sector->getHPB(j,i)->clockIn(m_bunchCounter);
      }
    }
  }
}

void TGCTimingManager::startSectorLogic(TGCSector* sector)
{
  TGCSectorLogic* sl= sector->getSL();

  if(sl!=0) sl->clockIn(m_readCondKey, m_bunchCounter, sector->hasHit());
}

} //end of namespace bracket
