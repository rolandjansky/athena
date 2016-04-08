/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloEvent/CMMRoI.h"


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
//#include "TrigT1EventTPCnv/CMMRoI_p1.h"
#include "TrigT1EventTPCnv/CMMRoICnv_p1.h"

using namespace LVL1;

/*
CMMRoICnv_p1::CMMRoICnv_p1()
  : T_AthenaPoolTPCnvBase< CMMRoI, CMMRoI_p1 >() {

}
*/

void CMMRoICnv_p1::persToTrans( const CMMRoI_p1* persObj, CMMRoI* transObj, MsgStream &log ) {

  //log << MSG::INFO << "Converting CMMRoI from persistent state..." << endreq;

  //transObj->clear(); // see if I actually need one of these

  //
  // Translate the CMMRoI
  //
  *transObj = CMMRoI (persObj->m_jetEtRoiWord,
                      persObj->m_energyRoiWord0,
                      persObj->m_energyRoiWord1,
                      persObj->m_energyRoiWord2);

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converted CMMRoI from persistent state [OK]" << endreq;

  return;

}

void CMMRoICnv_p1::transToPers( const CMMRoI* transObj, CMMRoI_p1* persObj, MsgStream &log ) {

  //log << MSG::INFO << "Creating persistent state of CMMRoI..." << endreq;

  persObj->m_jetEtRoiWord    = transObj->jetEtRoiWord();
  persObj->m_energyRoiWord0   = transObj->energyRoiWord0();
  persObj->m_energyRoiWord1     = transObj->energyRoiWord1();
  persObj->m_energyRoiWord2    = transObj->energyRoiWord2();

  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Created persistent state of CMMRoI [OK]" << endreq;

  return;

}
