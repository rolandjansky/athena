/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawEvent/LArRawChannel.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArTPCnv/LArRawChannelCnv_p1.h"

// LArRawChannelCnv_p1, used for T/P separation
// author R.Seuster <seuster@cern.ch>

void LArRawChannelCnv_p1::transToPers(const LArRawChannel* /*trans*/, LArRawChannel_p1* /*pers*/, MsgStream &/*log*/) const
{
    // No longer used
}

void LArRawChannelCnv_p1::persToTrans(const LArRawChannel_p1* pers, LArRawChannel* trans, MsgStream &/*log*/) const
{
  union {
    int  m_quality ; 
    uint16_t m_qualProv[2];
  } qprov{};

  // FIXME: persistent data is machine-dependent!
  int q              = pers->m_qualityandgain & 0xFFFF;
  if (q==65535) {
    qprov.m_quality = 0xA50000;
  }
  else {
    qprov.m_quality = 0x20A50000 | q;
  }

  *trans = LArRawChannel (HWIdentifier(Identifier32(pers->m_channelID)),
                          pers->m_energy,
                          pers->m_time,
                          qprov.m_qualProv[0],  // quality
                          qprov.m_qualProv[1],  // provenance
                          (CaloGain::CaloGain)((pers->m_qualityandgain>>16)&0xF)
                          );
}

