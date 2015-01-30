/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigMissingEtEventTPCnv/TrigMissingET_p1.h"
#undef private
#undef protected

#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"


void TrigMissingETCnv_p1::persToTrans(const TrigMissingET_p1* persObj, 
				    TrigMissingET* transObj, 
				    MsgStream &log)
{
   log << MSG::DEBUG << "TrigMissingETCnv_p1::persToTrans called " << endreq;

   transObj->m_ex        = persObj->m_ex    ; 
   transObj->m_ey        = persObj->m_ey ;
   transObj->m_sum_et	 = persObj->m_sum_et       ;
   transObj->m_roiWord   = persObj->m_roiWord   ; 

   // basic info only present in new version
   transObj->m_ez        = 0;
   transObj->m_sum_e 	 = 0;
   transObj->m_flag  	 = 0;

   // auxiliary info: only present in new version
   transObj->m_compVec.clear();
   
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
