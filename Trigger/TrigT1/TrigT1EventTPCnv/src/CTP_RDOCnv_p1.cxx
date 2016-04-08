/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/CTP_RDOCnv_p1.h"

/**
 * Default constructor.
 */
CTP_RDOCnv_p1::CTP_RDOCnv_p1()
   : T_AthenaPoolTPCnvBase< CTP_RDO, CTP_RDO_p1 >() {

}

/**
 * Function transferring the information from a persistent CTP_RDO_p1 object
 * to a transient CTP_RDO object.
 */
void CTP_RDOCnv_p1::persToTrans( const CTP_RDO_p1* persObj, CTP_RDO* transObj,
                                 MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting CTP_RDO from persistent state..." << endreq;

   *transObj = CTP_RDO (0, persObj->m_dataWords);

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting CTP_RDO from persistent state [OK]" << endreq;

   return;

}

/**
 * Function transferring the information from a transient CTP_RDO object
 * to a persistent CTP_RDO_p1 object.
 */
void CTP_RDOCnv_p1::transToPers( const CTP_RDO* transObj, CTP_RDO_p1* persObj,
                                 MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of CTP_RDO..." << endreq;

   // not needed, will be recalculated
   // persObj->m_numberOfBunches = transObj->m_numberOfBunches;
   // not needed, will be taken as default
   // persObj->m_activeBunch     = transObj->m_activeBunch;

   persObj->m_dataWords        = transObj->getDataWords();

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of CTP_RDO [OK]" << endreq;

   return;

}
