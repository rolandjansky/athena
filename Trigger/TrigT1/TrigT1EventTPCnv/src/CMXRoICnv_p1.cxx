/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMXRoI.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMXRoI_p1.h"
#include "TrigT1EventTPCnv/CMXRoICnv_p1.h"

using namespace LVL1;

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
  *transObj = CMXRoI (persObj->m_roiWords[0],
                      persObj->m_roiWords[1],
                      persObj->m_roiWords[2],
                      persObj->m_roiWords[3],
                      persObj->m_roiWords[4],
                      persObj->m_roiWords[5]);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMXRoI from persistent state [OK]" << endreq;

  return;

}

void CMXRoICnv_p1::transToPers( const CMXRoI* transObj, CMXRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMXRoI..." << endreq;

  persObj->m_roiWords.resize(6);
  for (int i=0; i < 6; i++)
    persObj->m_roiWords[i] = transObj->roiWord(i);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMXRoI [OK]" << endreq;

  return;

}
