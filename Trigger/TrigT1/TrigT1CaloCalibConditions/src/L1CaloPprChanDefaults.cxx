/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanDefaults.h"

L1CaloPprChanDefaults::L1CaloPprChanDefaults(unsigned int channelId, int bcidDecision1, int satOverride1, int bcidDecision2, int satOverride2, int bcidDecision3, int satOverride3, int peakFinderCond, int decisionSource, int bypassLutCp, int bypassLutJep, int inDataSample, int readOut80Active, int firCoeff1, int firCoeff2, int firCoeff3, int firCoeff4, int firCoeff5, int satHigh, int satLow, int startBit, int energyLevelHigh, int energyLevelLow, int satLevel, int saturationValue, int numBcRaw, int numBcBcid, int pedCorrEnable, int sat80High, int sat80Low)
 : m_channelId(channelId)
 , m_bcidDecision1(bcidDecision1)
 , m_satOverride1(satOverride1)
 , m_bcidDecision2(bcidDecision2)
 , m_satOverride2(satOverride2)
 , m_bcidDecision3(bcidDecision3)
 , m_satOverride3(satOverride3)
 , m_peakFinderCond(peakFinderCond)
 , m_decisionSource(decisionSource)
 , m_bypassLutCp(bypassLutCp)
 , m_bypassLutJep(bypassLutJep)
 , m_inDataSample(inDataSample)
 , m_readOut80Active(readOut80Active)
 , m_firCoeff1(firCoeff1)
 , m_firCoeff2(firCoeff2)
 , m_firCoeff3(firCoeff3)
 , m_firCoeff4(firCoeff4)
 , m_firCoeff5(firCoeff5)
 , m_satHigh(satHigh)
 , m_satLow(satLow)
 , m_startBit(startBit)
 , m_energyLevelHigh(energyLevelHigh)
 , m_energyLevelLow(energyLevelLow)
 , m_satLevel(satLevel)
 , m_saturationValue(saturationValue)
 , m_numBcRaw(numBcRaw)
 , m_numBcBcid(numBcBcid)
 , m_pedCorrEnable(pedCorrEnable)
 , m_sat80High(sat80High)
 , m_sat80Low(sat80Low)
{
}
