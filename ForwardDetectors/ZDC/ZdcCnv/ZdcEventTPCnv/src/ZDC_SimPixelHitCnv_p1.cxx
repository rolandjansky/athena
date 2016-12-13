/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZDC_SimEvent/ZDC_SimPixelHit.h"
#include "Identifier/Identifier.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHitCnv_p1.h"

void ZDC_SimPixelHitCnv_p1::persToTrans(const ZDC_SimPixelHit_p1* persObj, ZDC_SimPixelHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimPixelHitCnv_p1::persToTrans " << endmsg;

  *transObj = ZDC_SimPixelHit (persObj->Side,
                               persObj->ModNo,
                               persObj->PixNo,
                               persObj->Nphotons,
                               persObj->Edep);
                               
}

void ZDC_SimPixelHitCnv_p1::transToPers(const ZDC_SimPixelHit* transObj, ZDC_SimPixelHit_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimPixelHitCnv_p1::transToPers " << endmsg;

  persObj->Side 	   = transObj->GetSide();
  persObj->ModNo	   = transObj->GetMod();
  persObj->PixNo	   = transObj->GetPix();
  persObj->Edep 	   = transObj->GetEdep();
  persObj->Nphotons	   = transObj->GetNPhotons();
  
}
