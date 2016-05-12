/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSimEvent/LArHitFloatContainer.h"
#include "LArSimEventTPCnv/LArHitFloatContainerCnv_p1.h"

void LArHitFloatContainerCnv_p1::transToPers(const LArHitFloatContainer* /*transCont*/, LArHitContainer_p1* /*persCont*/, MsgStream &log)
{
  log << MSG::ERROR << "Writing LArHitContainer_p1 not supported any more. Current version is LArHitChannelContainer_p2" << endreq;
}

void LArHitFloatContainerCnv_p1::persToTrans(const LArHitContainer_p1* persCont, LArHitFloatContainer* transCont, MsgStream &log)
{
  log << MSG::DEBUG << " reading LArHitContaner_p1 " << endreq;
  std::size_t nChannels = persCont->m_cont.size();
  transCont->resize(nChannels);
  for (std::size_t i=0;i<nChannels;i++) 
    m_LArHitFloatCnv_p1.persToTrans(&(persCont->m_cont[i]),&(transCont->at(i)),log);

}
