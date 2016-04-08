/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArRawChannel.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArTPCnv/LArRawChannelCnv_p2.h"

// LArRawChannelCnv_p2, used for T/P separation
// author G.Unal

void LArRawChannelCnv_p2::transToPers(const LArRawChannel* trans, LArRawChannel_p2* pers, MsgStream &/*log*/) 
{
  union {
    int  m_quality ; 
    uint16_t m_qualProv[2];
  } qprov;

  pers->m_channelID      = trans->identify().get_identifier32().get_compact();
  pers->m_energy         = trans->energy();
  pers->m_time           = trans->time();

  qprov.m_qualProv[0] = trans->quality();
  qprov.m_qualProv[1] = trans->provenance();

  int tmpqual=qprov.m_quality  & 0x3FFFFFFF;
  int tmpgain=trans->gain() & 0x3;
  tmpgain<<=30;
  pers->m_qualityandgain = tmpqual | tmpgain;
}

void LArRawChannelCnv_p2::persToTrans(const LArRawChannel_p2* pers, LArRawChannel* trans, MsgStream &/*log*/) 
{
  union {
    int  m_quality ; 
    uint16_t m_qualProv[2];
  } qprov;

  qprov.m_quality  = pers->m_qualityandgain & 0x3FFFFFFF;

  *trans = LArRawChannel (HWIdentifier(Identifier32(pers->m_channelID)),
                          pers->m_energy,
                          pers->m_time,
                          qprov.m_qualProv[0],  // quality
                          qprov.m_qualProv[1],  // provenance
                          (CaloGain::CaloGain)((pers->m_qualityandgain>>30)&0x3)
                          );
}

