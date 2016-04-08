/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprChanCalib.h"

L1CaloPprChanCalib::L1CaloPprChanCalib(unsigned int channelId, unsigned int errorCode, double pedMean, unsigned int pedValue, unsigned int pedFirSum, unsigned short extBcidThreshold, unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned short firStartBit, short firCoeff1, short firCoeff2, short firCoeff3, short firCoeff4, short firCoeff5, unsigned short lutCpStrategy, unsigned short lutCpOffset, unsigned short lutCpNoiseCut, unsigned short lutCpSlope, short lutCpPar1, short lutCpPar2, short lutCpPar3, short lutCpPar4, unsigned short lutCpScale, unsigned short lutJepStrategy, unsigned short lutJepOffset, unsigned short lutJepNoiseCut, unsigned short lutJepSlope, short lutJepPar1, short lutJepPar2, short lutJepPar3, short lutJepPar4, unsigned short lutJepScale)
 : m_channelId(channelId)
 ,m_errorCode(errorCode)
 ,m_pedMean(pedMean)
 ,m_pedValue(pedValue)
 ,m_pedFirSum(pedFirSum)
 ,m_extBcidThreshold(extBcidThreshold)
 ,m_satBcidThreshLow(satBcidThreshLow)
 ,m_satBcidThreshHigh(satBcidThreshHigh)
 ,m_satBcidLevel(satBcidLevel)
 ,m_bcidEnergyRangeLow(bcidEnergyRangeLow)
 ,m_bcidEnergyRangeHigh(bcidEnergyRangeHigh)
 ,m_firStartBit(firStartBit)
 ,m_firCoeff1(firCoeff1)
 ,m_firCoeff2(firCoeff2)
 ,m_firCoeff3(firCoeff3)
 ,m_firCoeff4(firCoeff4)
 ,m_firCoeff5(firCoeff5)
 ,m_lutCpStrategy(lutCpStrategy)
 ,m_lutCpOffset(lutCpOffset)
 ,m_lutCpNoiseCut(lutCpNoiseCut)
 ,m_lutCpSlope(lutCpSlope)
 ,m_lutCpPar1(lutCpPar1)
 ,m_lutCpPar2(lutCpPar2)
 ,m_lutCpPar3(lutCpPar3)
 ,m_lutCpPar4(lutCpPar4)
 ,m_lutCpScale(lutCpScale)
 ,m_lutJepStrategy(lutJepStrategy)
 ,m_lutJepOffset(lutJepOffset)
 ,m_lutJepNoiseCut(lutJepNoiseCut)
 ,m_lutJepSlope(lutJepSlope)
 ,m_lutJepPar1(lutJepPar1)
 ,m_lutJepPar2(lutJepPar2)
 ,m_lutJepPar3(lutJepPar3)
 ,m_lutJepPar4(lutJepPar4)
 ,m_lutJepScale(lutJepScale)
{
}
