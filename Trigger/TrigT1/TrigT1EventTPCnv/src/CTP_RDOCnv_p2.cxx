/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigT1EventTPCnv/CTP_RDOCnv_p2.h"

/**
 * Default constructor.
 */
CTP_RDOCnv_p2::CTP_RDOCnv_p2()
   : T_AthenaPoolTPCnvBase< CTP_RDO, CTP_RDO_p2 >() {

}

/**
 * Function transferring the information from a persistent CTP_RDO_p2 object
 * to a transient CTP_RDO object.
 */
void CTP_RDOCnv_p2::persToTrans( const CTP_RDO_p2* persObj, CTP_RDO* transObj,
                                 MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting CTP_RDO from persistent state..." << endreq;

   transObj->m_dataWords        = persObj->m_dataWords;
   transObj->m_l1AcceptPosition = persObj->m_l1AcceptPosition;

   // set m_activeBunch to default value (see constructor)

   // calculate from data size (CTP_RDO::TIME_WORDS = 2, CTP_RDO::DAQ_WORDS_PER_BUNCH = 6+8+8+8)
   transObj->m_numberOfBunches = (transObj->m_dataWords.size() - 2)/(6+8+8+8);

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converting CTP_RDO from persistent state [OK]" << endreq;

   return;

}

/**
 * Function transferring the information from a transient CTP_RDO object
 * to a persistent CTP_RDO_p2 object.
 */
void CTP_RDOCnv_p2::transToPers( const CTP_RDO* transObj, CTP_RDO_p2* persObj,
                                 MsgStream& log ) {

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of CTP_RDO..." << endreq;

   // not needed, will be recalculated
   // persObj->m_numberOfBunches = transObj->m_numberOfBunches;
   // not needed, will be taken as default
   // persObj->m_activeBunch     = transObj->m_activeBunch;

   persObj->m_l1AcceptPosition = transObj->m_l1AcceptPosition;
   persObj->m_dataWords        = transObj->m_dataWords;

   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Creating persistent state of CTP_RDO [OK]" << endreq;

   return;

}
