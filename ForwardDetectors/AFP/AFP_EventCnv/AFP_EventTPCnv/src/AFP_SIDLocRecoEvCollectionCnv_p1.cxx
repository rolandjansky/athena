/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
 
static AFP_SIDLocRecoEventCnv_p1 recConv;
 
void AFP_SIDLocRecoEvCollectionCnv_p1::transToPers(const AFP_SIDLocRecoEvCollection* transObj, AFP_SIDLocRecoEvCollection_p1* persObj, MsgStream& log) const {
   
  persObj->resize(transObj->size());
   
  for (unsigned int i=0; i<transObj->size(); ++i) {
     
    AFP_SIDLocRecoEvent_p1& locRec = (*persObj)[i];
     
    recConv.transToPers((*transObj)[i], &locRec, log);
  }    
}
 
void AFP_SIDLocRecoEvCollectionCnv_p1::persToTrans(const AFP_SIDLocRecoEvCollection_p1* persObj, AFP_SIDLocRecoEvCollection* transObj, MsgStream& log) const {
 
  transObj->reserve(persObj->size());
   
  for (unsigned int i=0; i<persObj->size(); ++i) {
 
    const AFP_SIDLocRecoEvent_p1* locRec = &((*persObj)[i]);
     
    transObj->push_back(recConv.createTransient(locRec, log));
  }    
}
 
