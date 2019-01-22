/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//$Id: TGCTimingManager.cxx,v 1.4 2009-05-13 02:53:51 isaya Exp $

#include "TrigT1TGC/TGCSector.hh"
#include "TrigT1TGC/TGCPatchPanel.hh"
#include "TrigT1TGC/TGCSlaveBoard.hh"
#include "TrigT1TGC/TGCDoubletSB.hh"
#include "TrigT1TGC/TGCWireTripletSB.hh"
#include "TrigT1TGC/TGCStripTripletSB.hh"
#include "TrigT1TGC/TGCHighPtBoard.hh"
#include "TrigT1TGC/TGCWireHighPtBoard.hh"
#include "TrigT1TGC/TGCStripHighPtBoard.hh"
#include "TrigT1TGC/TGCSectorLogic.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"

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
  int i,j;

#ifdef TGCDEBUG
  std::cout << " Start PatchPanel " 
	    << "or sector# " << sector->getSideId() << ":"
	    << sector->getOctantId() << ":"
	    << sector->getModuleId() 
	    << std::endl;
#endif

  // start BID by first clockIn, start OrLogic by second clockIn.
  for( int iClk=0; iClk<2; iClk+=1) {
    for( j=0; j<NumberOfPatchPanelType; j+=1) {
      for ( i=0; i<sector->getNumberOfPP(j); i+=1) {
	TGCPatchPanel *pp = sector->getPP(j,i);
	if(pp) pp->clockIn(m_bunchCounter, db);
      }
    }
  }
}

void TGCTimingManager::startSlaveBoard(TGCSector* sector)
{
  int i,j;

#ifdef TGCDEBUG
  std::cout << " Start SlaveBoard " 
	    << "for sector# " << sector->getSideId() << ":"
	    << sector->getOctantId() << ":"
	    << sector->getModuleId() 
	    << std::endl;
#endif
  
  for( j=0; j<NumberOfSlaveBoardType; j+=1){
    for( i=0; i<sector->getNumberOfSB(j); i+=1){
      TGCSlaveBoard* sb = sector->getSB(j,i);
      if(sb)sb->clockIn(m_bunchCounter);
    }
  }
}

void TGCTimingManager::startHighPtBoard(TGCSector* sector)
{
#ifdef TGCDEBUG
  std::cout << "Start  HighPt Board" <<std::endl; 
#endif

  int i,j;
  // collect input signals by 1st clockIn,
  // perform coincidence by 2nd clockIn.
  for( int iClk=0; iClk<2; iClk+=1) 
    for( j=0; j<NumberOfHighPtBoardType; j+=1)
      for ( i=0; i<sector->getNumberOfHPB(j); i+=1) 
        sector->getHPB(j,i)->clockIn(m_bunchCounter);
}

void TGCTimingManager::startSectorLogic(TGCSector* sector)
{
#ifdef TGCDEBUG
  std::cout << "Start  Sector Logic" <<std::endl; 
#endif

  TGCSectorLogic* sl= sector->getSL();
  if(sl!=0) sl->clockIn(m_readCondKey, m_bunchCounter);
}

} //end of namespace bracket
