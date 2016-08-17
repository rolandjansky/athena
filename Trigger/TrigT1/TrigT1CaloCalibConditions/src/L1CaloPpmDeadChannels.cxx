/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPpmDeadChannels.h"

L1CaloPpmDeadChannels::L1CaloPpmDeadChannels(unsigned int channelId, unsigned int errorCode, unsigned int noiseCut)
 : m_channelId(channelId)
 , m_errorCode(errorCode)
 , m_noiseCut(noiseCut)
{
}
