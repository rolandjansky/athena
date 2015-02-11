/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigT1CaloEvent/CMXRoI.h"
#undef private
#undef protected


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXRoI_p1.h"
#include "TrigT1EventTPCnv/CMXRoICnv_p1.h"

/*
CMXRoICnv_p1::CMXRoICnv_p1()
  : T_AthenaPoolTPCnvBase< CMXRoI, CMXRoI_p1 >() {

}
*/

void CMXRoICnv_p1::persToTrans( const CMXRoI_p1* persObj, CMXRoI* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMXRoI from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMXRoI
  // 
  transObj->m_roiWords = persObj->m_roiWords;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXRoI from persistent state [OK]" << endreq;

  return;

}

void CMXRoICnv_p1::transToPers( const CMXRoI* transObj, CMXRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXRoI..." << endreq;

  persObj->m_roiWords = transObj->m_roiWords;

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXRoI [OK]" << endreq;

  return;

}
