/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanCalibV1.h"

L1CaloPprChanCalibV1::L1CaloPprChanCalibV1(unsigned int channelId, unsigned int errorCode, double pedMean, unsigned int pedValue, unsigned short extBcidThreshold, unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned short firStartBit, short firCoeff1, short firCoeff2, short firCoeff3, short firCoeff4, short firCoeff5, unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutNoiseCut, unsigned short lutSlope)
 : m_channelId(channelId)
 , m_errorCode(errorCode)
 , m_pedMean(pedMean)
 , m_pedValue(pedValue)
 , m_extBcidThreshold(extBcidThreshold)
 , m_satBcidThreshLow(satBcidThreshLow)
 , m_satBcidThreshHigh(satBcidThreshHigh)
 , m_satBcidLevel(satBcidLevel)
 , m_bcidEnergyRangeLow(bcidEnergyRangeLow)
 , m_bcidEnergyRangeHigh(bcidEnergyRangeHigh)
 , m_firStartBit(firStartBit)
 , m_firCoeff1(firCoeff1)
 , m_firCoeff2(firCoeff2)
 , m_firCoeff3(firCoeff3)
 , m_firCoeff4(firCoeff4)
 , m_firCoeff5(firCoeff5)
 , m_lutStrategy(lutStrategy)
 , m_lutOffset(lutOffset)
 , m_lutNoiseCut(lutNoiseCut)
 , m_lutSlope(lutSlope)
{
}
