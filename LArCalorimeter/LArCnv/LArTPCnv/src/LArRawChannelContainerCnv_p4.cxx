/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArRawChannel.h"
#include "LArTPCnv/LArRawChannel_p2.h"
#include "LArRawEvent/LArRawChannelContainer.h"


// LArRawChannelContainerCnv_p4, used for T/P separation with new LArRawChannel_p2
// author G.Unal

#include "LArTPCnv/LArRawChannelContainerCnv_p4.h"

void LArRawChannelContainerCnv_p4::transToPers(const LArRawChannelContainer* transCont, LArRawChannelContainer_p4* persCont, MsgStream &log) 
{
//  std::cout << " *** Write LArRawChannelContainer_p4 " << std::endl;
  std::size_t nChannels=transCont->size();
  persCont->m_channels.resize(nChannels);
  for (std::size_t i=0;i<nChannels;++i)
    m_larRawChannelCnv_p2.transToPers(&(transCont->at(i)),&(persCont->m_channels[i]),log);

  log << MSG::DEBUG  << " ***  Writing LArRawChannelContainer_p4" << endreq;
}

void LArRawChannelContainerCnv_p4::persToTrans(const LArRawChannelContainer_p4* persCont, LArRawChannelContainer* transCont, MsgStream &log) 
{
//  std::cout << " *** Read LArRawChannelContainer_p4 " << std::endl;
  std::size_t nChannels=persCont->m_channels.size();
  transCont->resize(nChannels);
  for (std::size_t i=0;i<nChannels;++i)
    m_larRawChannelCnv_p2.persToTrans(&(persCont->m_channels[i]),&(transCont->at(i)),log);

  log << MSG::DEBUG  << " ***  Reading LArRawChannelContainer" << endreq;
}
 
