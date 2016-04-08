/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMRoI.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CPMRoI_p1.h"
#include "TrigT1EventTPCnv/CPMRoICnv_p1.h"

using namespace LVL1;

/*
CPMRoICnv_p1::CPMRoICnv_p1()
  : T_AthenaPoolTPCnvBase< CPMRoI, CPMRoI_p1 >() {

}
*/

void CPMRoICnv_p1::persToTrans( const CPMRoI_p1* persObj, CPMRoI* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CPMRoI from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CPMRoI
  // 
  *transObj = CPMRoI (persObj->m_roiWord);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CPMRoI from persistent state [OK]" << endreq;

  return;

}

void CPMRoICnv_p1::transToPers( const CPMRoI* transObj, CPMRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CPMRoI..." << endreq;

  persObj->m_roiWord    = transObj->roiWord();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CPMRoI [OK]" << endreq;

  return;

}
