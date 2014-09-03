/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawEvent/LArRawChannel.h"
#undef private
#undef protected

#include "CaloIdentifier/CaloGain.h"
#include "LArTPCnv/LArRawChannelCnv_p2.h"

// LArRawChannelCnv_p2, used for T/P separation
// author G.Unal

void LArRawChannelCnv_p2::transToPers(const LArRawChannel* trans, LArRawChannel_p2* pers, MsgStream &/*log*/) 
{
  pers->m_channelID      = trans->m_channelID.get_identifier32().get_compact();
  pers->m_energy         = trans->m_energy;
  pers->m_time           = trans->m_time;
  int tmpqual=trans->m_quality  & 0x3FFFFFFF;
  int tmpgain=trans->m_gain & 0x3;
  tmpgain<<=30;
  pers->m_qualityandgain = tmpqual | tmpgain;
  //std::cout << " writing rawchannel " << trans->m_channelID << " " << trans->m_energy <<
  //  " " << trans->m_time << " " << std::hex << trans->m_quality << " " << std::dec << trans->m_gain << std::endl;
  /* log << MSG::VERBOSE << " ***  Writing LArRawChannel "
     << std::hex << pers->m_qualityandgain << " "
     << std::hex << trans->m_gain
     << std::dec
     << endreq;  */
}

void LArRawChannelCnv_p2::persToTrans(const LArRawChannel_p2* pers, LArRawChannel* trans, MsgStream &/*log*/) 
{
  trans->m_channelID = HWIdentifier(Identifier32(pers->m_channelID));
  trans->m_energy    = pers->m_energy;
  trans->m_time      = pers->m_time;
  int q              = pers->m_qualityandgain & 0x3FFFFFFF;
  trans->m_quality   = q;
  trans->m_gain      = (CaloGain::CaloGain)((pers->m_qualityandgain>>30)&0x3);
  //std::cout << " reading rawchannel " << trans->m_channelID << " " << trans->m_energy <<
  //   " " << trans->m_time << " " << std::hex << trans->m_quality << " " << std::dec << trans->m_gain << std::endl;
  // log << MSG::VERBOSE  << " ***  Reading LArRawChannel" << endreq;
}

