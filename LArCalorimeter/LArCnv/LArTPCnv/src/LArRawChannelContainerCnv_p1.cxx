/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  LArRawChannelContainer_p1::const_iterator it1=pers->begin();
  LArRawChannelContainer_p1::const_iterator it1_e=pers->end();
  for (;it1!=it1_e;it1++) {
    const LArRawChannelCollection_p1& coll=*it1;
    trans->resize(trans->size()+coll.size());
    LArRawChannelCollection_p1::const_iterator it2=coll.begin();
    LArRawChannelCollection_p1::const_iterator it2_e=coll.end();
    for(;it2!=it2_e;it2++) {
      const LArRawChannel_p1& rc=*it2;
      // virtual void   persToTrans(const LArRawChannel_p1* pers, LArRawChannel* trans, MsgStream &log) ;
      m_larRawChannelCnv_p1.persToTrans(&rc,&trans->at(index),log);
      ++index;
    }
  }
  log << MSG::DEBUG  << " ***  Reading LArRawChannelContainer" << endmsg;
}
