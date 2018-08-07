/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimEvent/ZDC_SimStripHit.h"
#include "Identifier/Identifier.h"
#include "ZdcEventTPCnv/ZDC_SimStripHitCnv_p1.h"

void ZDC_SimStripHitCnv_p1::persToTrans(const ZDC_SimStripHit_p1* persObj, ZDC_SimStripHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimStripHitCnv_p1::persToTrans " << endmsg;

  *transObj = ZDC_SimStripHit (persObj->Side,
                               persObj->ModNo,
                               persObj->Nphotons,
                               persObj->Edep);
}

void ZDC_SimStripHitCnv_p1::transToPers(const ZDC_SimStripHit* transObj, ZDC_SimStripHit_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimStripHitCnv_p1::transToPers " << endmsg;

  persObj->Side 	   = transObj->GetSide();
  persObj->ModNo	   = transObj->GetMod();
  persObj->Edep 	   = transObj->GetEdep();
  persObj->Nphotons	   = transObj->GetNPhotons();
  
}
