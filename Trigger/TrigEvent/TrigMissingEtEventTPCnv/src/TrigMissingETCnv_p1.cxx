/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"


void TrigMissingETCnv_p1::persToTrans(const TrigMissingET_p1* persObj, 
				    TrigMissingET* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p1::persToTrans called " << endreq;

   // Clear auxiliary info.
   *transObj = TrigMissingET(0);

   transObj->setEx       (persObj->m_ex)    ; 
   transObj->setEy       (persObj->m_ey) ;
   transObj->setSumEt	 (persObj->m_sum_et)       ;
   transObj->set_RoIword (persObj->m_roiWord)   ; 

   // basic info only present in new version
   transObj->setEz       (0);
   transObj->setSumE 	 (0);
   transObj->setFlag  	 (0);
}


void TrigMissingETCnv_p1::transToPers(const TrigMissingET* /*transObj*/, 
                                      TrigMissingET_p1* /*persObj*/, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p1::transToPers called " << endreq;
   log << MSG::WARNING << "should call new converter!  Skipping..." << endreq;

//    persObj->m_ex        = transObj->m_ex       ; 
//    persObj->m_ey        = transObj->m_ey       ;
//    persObj->m_sum_et	= transObj->m_sum_et   ;
//    persObj->m_roiWord   = transObj->m_roiWord  ; 
  
}
