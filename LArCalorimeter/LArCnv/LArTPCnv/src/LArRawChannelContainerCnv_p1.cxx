/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArRawChannelContainer.h"

// LArRawChannelContainerCnv_p1, used for T/P separation
// author R.Seuster <seuster@cern.ch>

//#include "LArTPCnv/LArRawChannelCollectionCnv_p1.h"
#include "LArTPCnv/LArRawChannelContainerCnv_p1.h"

void LArRawChannelContainerCnv_p1::transToPers(const LArRawChannelContainer*, LArRawChannelContainer_p1*, MsgStream &log) const
{
  //LArRawChannelContainer_Cnvp1_base_t::transToPers(trans,pers,log);
  log << MSG::ERROR  << " ***  Writing LArRawChannelContainer_p1 not supported any more. Current version is LArRawChannelContainer_p3" << endmsg;
}

void LArRawChannelContainerCnv_p1::persToTrans(const LArRawChannelContainer_p1* pers, LArRawChannelContainer* trans, MsgStream &log) const
{
  //LArRawChannelContainer_Cnvp1_base_t::persToTrans(pers,trans,log);
  std::size_t index=0;
  for (const LArRawChannelCollection_p1& coll : *pers) {
    trans->resize(trans->size()+coll.size());
    for (const LArRawChannel_p1& rc : coll) {
      // virtual void   persToTrans(const LArRawChannel_p1* pers, LArRawChannel* trans, MsgStream &log) ;
      m_larRawChannelCnv_p1.persToTrans(&rc,&trans->at(index),log);
      ++index;
    }
  }
  log << MSG::DEBUG  << " ***  Reading LArRawChannelContainer" << endmsg;
}
