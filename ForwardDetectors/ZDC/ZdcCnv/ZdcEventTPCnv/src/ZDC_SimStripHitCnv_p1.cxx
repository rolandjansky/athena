/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#undef private
#undef protected

#include "Identifier/Identifier.h"
#include "ZdcEventTPCnv/ZDC_SimStripHitCnv_p1.h"

void ZDC_SimStripHitCnv_p1::persToTrans(const ZDC_SimStripHit_p1* persObj, ZDC_SimStripHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimStripHitCnv_p1::persToTrans " << endreq;

  transObj->Side            = persObj->Side;
  transObj->ModNo           = persObj->ModNo;
  transObj->Edep            = persObj->Edep;
  transObj->Nphotons        = persObj->Nphotons;

}

void ZDC_SimStripHitCnv_p1::transToPers(const ZDC_SimStripHit* transObj, ZDC_SimStripHit_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimStripHitCnv_p1::transToPers " << endreq;

  persObj->Side 	   = transObj->Side;
  persObj->ModNo	   = transObj->ModNo;
  persObj->Edep 	   = transObj->Edep;
  persObj->Nphotons	   = transObj->Nphotons;
  
}
