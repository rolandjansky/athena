/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanExtra.h"

L1CaloPprChanExtra::L1CaloPprChanExtra(unsigned int channelId, unsigned int errorCode, unsigned short pedCorrEnable, unsigned short sat80BcidThreshHigh, unsigned short sat80BcidThreshLow, unsigned short threshNumSamplesPeak, unsigned short threshNumSamplesSat, unsigned short satOverride1, unsigned short satOverride2, unsigned short satOverride3, short lutCpPar5, short lutCpPar6, short lutCpPar7, short lutJepPar5, short lutJepPar6, short lutJepPar7)
 : m_channelId(channelId)
 ,m_errorCode(errorCode)
 ,m_pedCorrEnable(pedCorrEnable)
 ,m_sat80BcidThreshHigh(sat80BcidThreshHigh)
 ,m_sat80BcidThreshLow(sat80BcidThreshLow)
 ,m_threshNumSamplesPeak(threshNumSamplesPeak)
 ,m_threshNumSamplesSat(threshNumSamplesSat)
 ,m_satOverride1(satOverride1)
 ,m_satOverride2(satOverride2)
 ,m_satOverride3(satOverride3)
 ,m_lutCpPar5(lutCpPar5)
 ,m_lutCpPar6(lutCpPar6)
 ,m_lutCpPar7(lutCpPar7)
 ,m_lutJepPar5(lutJepPar5)
 ,m_lutJepPar6(lutJepPar6)
 ,m_lutJepPar7(lutJepPar7)
{
}
