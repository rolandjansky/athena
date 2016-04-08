/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/JEMTobRoI.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/JEMTobRoI_p1.h"
#include "TrigT1EventTPCnv/JEMTobRoICnv_p1.h"

using namespace LVL1;

/*
JEMTobRoICnv_p1::JEMTobRoICnv_p1()
  : T_AthenaPoolTPCnvBase< JEMTobRoI, JEMTobRoI_p1 >() {

}
*/

void JEMTobRoICnv_p1::persToTrans( const JEMTobRoI_p1* persObj, JEMTobRoI* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting JEMTobRoI from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the JEMTobRoI
  // 
  *transObj = JEMTobRoI (persObj->m_roiWord);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JEMTobRoI from persistent state [OK]" << endreq;

  return;

}

void JEMTobRoICnv_p1::transToPers( const JEMTobRoI* transObj, JEMTobRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of JEMTobRoI..." << endreq;

  persObj->m_roiWord    = transObj->roiWord();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JEMTobRoI [OK]" << endreq;

  return;

}
