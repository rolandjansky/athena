/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/JEMRoI.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/JEMRoI_p1.h"
#include "TrigT1EventTPCnv/JEMRoICnv_p1.h"

/*
JEMRoICnv_p1::JEMRoICnv_p1()
  : T_AthenaPoolTPCnvBase< JEMRoI, JEMRoI_p1 >() {

}
*/

void JEMRoICnv_p1::persToTrans( const JEMRoI_p1* persObj, JEMRoI* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting JEMRoI from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the JEMRoI
  // 
  transObj->m_roiWord  = persObj->m_roiWord;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted JEMRoI from persistent state [OK]" << endreq;

  return;

}

void JEMRoICnv_p1::transToPers( const JEMRoI* transObj, JEMRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of JEMRoI..." << endreq;

  persObj->m_roiWord    = transObj->m_roiWord;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of JEMRoI [OK]" << endreq;

  return;

}
