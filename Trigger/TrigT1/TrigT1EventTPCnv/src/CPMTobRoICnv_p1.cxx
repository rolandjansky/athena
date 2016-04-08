/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CPMTobRoI.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CPMTobRoI_p1.h"
#include "TrigT1EventTPCnv/CPMTobRoICnv_p1.h"

using namespace LVL1;

/*
CPMTobRoICnv_p1::CPMTobRoICnv_p1()
  : T_AthenaPoolTPCnvBase< CPMTobRoI, CPMTobRoI_p1 >() {

}
*/

void CPMTobRoICnv_p1::persToTrans( const CPMTobRoI_p1* persObj, CPMTobRoI* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CPMTobRoI from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CPMTobRoI
  // 
  *transObj = CPMTobRoI (persObj->m_roiWord);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CPMTobRoI from persistent state [OK]" << endreq;

  return;

}

void CPMTobRoICnv_p1::transToPers( const CPMTobRoI* transObj, CPMTobRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CPMTobRoI..." << endreq;

  persObj->m_roiWord    = transObj->roiWord();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CPMTobRoI [OK]" << endreq;

  return;

}
