/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcEventTPCnv/ZDC_SimStripHitCnv_p1.h"
#include "ZdcEventTPCnv/ZDC_SimStripHit_CollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

static ZDC_SimStripHitCnv_p1 ZdcStripHitConv;

void ZDC_SimStripHit_CollectionCnv_p1::transToPers(const ZDC_SimStripHit_Collection* transObj, ZDC_SimStripHit_Collection_p1* persObj, MsgStream& log) {
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    ZDC_SimStripHit_p1& striphit = (*persObj)[i];
    const ZDC_SimStripHit& striphit_t = (*transObj)[i];
    
    ZdcStripHitConv.transToPers(&striphit_t, &striphit, log);
  }    
}

void ZDC_SimStripHit_CollectionCnv_p1::persToTrans(const ZDC_SimStripHit_Collection_p1* persObj, ZDC_SimStripHit_Collection* transObj, MsgStream& log) {

  //log << MSG::INFO << " size = " << persObj->size() << endmsg;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {
    
    //log << MSG::INFO << " i = " << i << endmsg;
    const ZDC_SimStripHit_p1* striphit = &((*persObj)[i]);

    std::unique_ptr<ZDC_SimStripHit> ptr (ZdcStripHitConv.createTransient(striphit, log));
    transObj->push_back(*ptr);
  }    
}
