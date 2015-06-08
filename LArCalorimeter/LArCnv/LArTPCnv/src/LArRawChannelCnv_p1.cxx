/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "LArRawEvent/LArRawChannel.h"
#undef private
#undef protected

#include "CaloIdentifier/CaloGain.h"
#include "LArTPCnv/LArRawChannelCnv_p1.h"

// LArRawChannelCnv_p1, used for T/P separation
// author R.Seuster <seuster@cern.ch>

void LArRawChannelCnv_p1::transToPers(const LArRawChannel* /*trans*/, LArRawChannel_p1* /*pers*/, MsgStream &/*log*/) 
{
    // No longer used
}

void LArRawChannelCnv_p1::persToTrans(const LArRawChannel_p1* pers, LArRawChannel* trans, MsgStream &/*log*/) 
{
  trans->m_channelID = HWIdentifier(Identifier32(pers->m_channelID));
  trans->m_energy    = pers->m_energy;
  trans->m_time      = pers->m_time;
  int q              = pers->m_qualityandgain & 0xFFFF;
  if (q==65535) {
    trans->m_quality = 0xA50000;
  }
  else {
    trans->m_quality = 0x20A50000 | q;
  }
  trans->m_gain      = (CaloGain::CaloGain)((pers->m_qualityandgain>>16)&0xF);
  // log << MSG::VERBOSE  << " ***  Reading LArRawChannel" << endreq;
}

