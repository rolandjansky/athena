/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArRawChannel.h"
#include "LArTPCnv/LArRawChannel_p1.h"
#include "LArRawEvent/LArRawChannelContainer.h"


// LArRawChannelContainerCnv_p3, used for T/P separation
// author Walter Lampl

#include "LArTPCnv/LArRawChannelContainerCnv_p3.h"

void LArRawChannelContainerCnv_p3::transToPers(const LArRawChannelContainer* transCont, LArRawChannelContainer_p3* persCont, MsgStream &log) const
{
  std::size_t nChannels=transCont->size();
  persCont->m_channels.resize(nChannels);
  for (std::size_t i=0;i<nChannels;++i)
    m_larRawChannelCnv_p1.transToPers(&(transCont->at(i)),&(persCont->m_channels[i]),log);

  log << MSG::DEBUG  << " ***  Writing LArRawChannelContainer_p3" << endmsg;
}

void LArRawChannelContainerCnv_p3::persToTrans(const LArRawChannelContainer_p3* persCont, LArRawChannelContainer* transCont, MsgStream &log) const
{
  std::size_t nChannels=persCont->m_channels.size();
  transCont->resize(nChannels);
  for (std::size_t i=0;i<nChannels;++i)
    m_larRawChannelCnv_p1.persToTrans(&(persCont->m_channels[i]),&(transCont->at(i)),log);

  log << MSG::DEBUG  << " ***  Reading LArRawChannelContainer" << endmsg;
}
 
