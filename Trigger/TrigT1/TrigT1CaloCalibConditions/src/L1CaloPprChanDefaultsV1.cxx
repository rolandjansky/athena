/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaultsV1.h"

L1CaloPprChanDefaultsV1::L1CaloPprChanDefaultsV1(unsigned int channelId, unsigned int BcidDecision1, unsigned int SatOverride1, unsigned int BcidDecision2, unsigned int SatOverride2, unsigned int BcidDecision3, unsigned int SatOverride3, unsigned int PeakFinderCond, unsigned int DecisionSource)
 : m_channelId(channelId)
 , m_bcidDecision1(BcidDecision1)
 , m_satOverride1(SatOverride1)
 , m_bcidDecision2(BcidDecision2)
 , m_satOverride2(SatOverride2)
 , m_bcidDecision3(BcidDecision3)
 , m_satOverride3(SatOverride3)
 , m_peakFinderCond(PeakFinderCond)
 , m_decisionSource(DecisionSource)
{
}
