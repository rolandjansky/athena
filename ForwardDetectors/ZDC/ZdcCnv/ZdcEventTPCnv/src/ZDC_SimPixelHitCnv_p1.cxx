/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ZDC_SimEvent/ZDC_SimPixelHit.h"
#undef private
#undef protected

#include "Identifier/Identifier.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHitCnv_p1.h"

void ZDC_SimPixelHitCnv_p1::persToTrans(const ZDC_SimPixelHit_p1* persObj, ZDC_SimPixelHit* transObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimPixelHitCnv_p1::persToTrans " << endreq;

  transObj->Side            = persObj->Side;
  transObj->ModNo           = persObj->ModNo;
  transObj->PixNo           = persObj->PixNo;
  transObj->Edep            = persObj->Edep;
  transObj->Nphotons        = persObj->Nphotons;
  
}

void ZDC_SimPixelHitCnv_p1::transToPers(const ZDC_SimPixelHit* transObj, ZDC_SimPixelHit_p1* persObj, MsgStream& log) {

  log << MSG::DEBUG << " In ZDC_SimPixelHitCnv_p1::transToPers " << endreq;

  persObj->Side 	   = transObj->Side;
  persObj->ModNo	   = transObj->ModNo;
  persObj->PixNo	   = transObj->PixNo;
  persObj->Edep 	   = transObj->Edep;
  persObj->Nphotons	   = transObj->Nphotons;
  
}
