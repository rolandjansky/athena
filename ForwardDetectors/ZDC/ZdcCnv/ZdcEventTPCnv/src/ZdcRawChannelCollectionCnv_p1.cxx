/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ZdcEventTPCnv/ZdcRawChannelCollectionCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

static ZdcRawChannelCnv_p1 rawchannelConv;

void ZdcRawChannelCollectionCnv_p1::transToPers(const ZdcRawChannelCollection* transObj, ZdcRawChannelCollection_p1* persObj, MsgStream& log) {
  
  persObj->resize(transObj->size());
  
  for (unsigned int i=0; i<transObj->size(); ++i) {
    
    ZdcRawChannel_p1& rawchannel = (*persObj)[i];
    
    rawchannelConv.transToPers((*transObj)[i], &rawchannel, log);
  }    
}

void ZdcRawChannelCollectionCnv_p1::persToTrans(const ZdcRawChannelCollection_p1* persObj, ZdcRawChannelCollection* transObj, MsgStream& log) {

  //log << MSG::INFO << " size = " << persObj->size() << endmsg;

  transObj->reserve(persObj->size());
  
  for (unsigned int i=0; i<persObj->size(); ++i) {
    
    //log << MSG::INFO << " i = " << i << endmsg;
    const ZdcRawChannel_p1* rawchannel = &((*persObj)[i]);
    
    transObj->push_back(rawchannelConv.createTransient(rawchannel,log));
  }    
}
