/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcEventTPCnv/ZDC_SimPixelHitCnv_p1.h"
#include "ZdcEventTPCnv/ZDC_SimPixelHit_CollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

static ZDC_SimPixelHitCnv_p1 ZdcPixelHitConv;

void ZDC_SimPixelHit_CollectionCnv_p1::transToPers(const ZDC_SimPixelHit_Collection* transObj, ZDC_SimPixelHit_Collection_p1* persObj, MsgStream& log) {
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    ZDC_SimPixelHit_p1& pixelhit = (*persObj)[i];
    const ZDC_SimPixelHit& pixelhit_t = (*transObj)[i];
    
    ZdcPixelHitConv.transToPers(&pixelhit_t, &pixelhit, log);
  }    
}

void ZDC_SimPixelHit_CollectionCnv_p1::persToTrans(const ZDC_SimPixelHit_Collection_p1* persObj, ZDC_SimPixelHit_Collection* transObj, MsgStream& log) {

  //log << MSG::INFO << " size = " << persObj->size() << endmsg;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {
    
    //log << MSG::INFO << " i = " << i << endmsg;
    const ZDC_SimPixelHit_p1* pixelhit = &((*persObj)[i]);
    
    std::unique_ptr<ZDC_SimPixelHit> ptr (ZdcPixelHitConv.createTransient(pixelhit, log));
    transObj->push_back(*ptr);
  }    
}
