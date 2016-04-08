/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloDisabledTowers.h"

L1CaloDisabledTowers::L1CaloDisabledTowers(unsigned int channelId, unsigned int disabledBits)
 : m_channelId(channelId)
 , m_disabledBits(disabledBits)
{
}
