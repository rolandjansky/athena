/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static AFP_TDLocRecoEventCnv_p1 recConv;
 
void AFP_TDLocRecoEvCollectionCnv_p1::transToPers(const AFP_TDLocRecoEvCollection* transObj, AFP_TDLocRecoEvCollection_p1* persObj, MsgStream& log) const {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    AFP_TDLocRecoEvent_p1& locRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &locRec, log);
  }    
}
 
void AFP_TDLocRecoEvCollectionCnv_p1::persToTrans(const AFP_TDLocRecoEvCollection_p1* persObj, AFP_TDLocRecoEvCollection* transObj, MsgStream& log) const {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const AFP_TDLocRecoEvent_p1* locRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransient(locRec, log));
  }    
}
 
