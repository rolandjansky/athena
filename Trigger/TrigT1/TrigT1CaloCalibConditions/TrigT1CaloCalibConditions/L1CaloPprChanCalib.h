// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIB_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIB_H

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib .
 * Automatically created using:
 *
 * python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude ModuleId,PprDacScanResultsTimeStamp,PprPedestalRunResultsTimeStamp,PprNoiseRunResultsTimeStamp,PprTimingResultsTimeStamp,PprSatBcidResultsTimeStamp,PprFirFilterResultsTimeStamp,PprLutValuesResultsTimeStamp,DacOffset,DacSlope,FullDelayData,SyncDelayBcid,InBcidNegedge,rateCpEtThresh,rateJepEtThresh /TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib
 */
class L1CaloPprChanCalib
{
public:
  L1CaloPprChanCalib() {}
  L1CaloPprChanCalib(unsigned int channelId, unsigned int errorCode, double pedMean, unsigned int pedValue, unsigned int pedFirSum, unsigned short extBcidThreshold, unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned short firStartBit, short firCoeff1, short firCoeff2, short firCoeff3, short firCoeff4, short firCoeff5, unsigned short lutCpStrategy, unsigned short lutCpOffset, unsigned short lutCpNoiseCut, unsigned short lutCpSlope, short lutCpPar1, short lutCpPar2, short lutCpPar3, short lutCpPar4, unsigned short lutCpScale, unsigned short lutJepStrategy, unsigned short lutJepOffset, unsigned short lutJepNoiseCut, unsigned short lutJepSlope, short lutJepPar1, short lutJepPar2, short lutJepPar3, short lutJepPar4, unsigned short lutJepScale);

  unsigned int channelId() const { return m_channelId; }
  unsigned int errorCode() const { return m_errorCode; }
  double pedMean() const { return m_pedMean; }
  unsigned int pedValue() const { return m_pedValue; }
  unsigned int pedFirSum() const { return m_pedFirSum; }
  unsigned short extBcidThreshold() const { return m_extBcidThreshold; }
  unsigned short satBcidThreshLow() const { return m_satBcidThreshLow; }
  unsigned short satBcidThreshHigh() const { return m_satBcidThreshHigh; }
  unsigned short satBcidLevel() const { return m_satBcidLevel; }
  unsigned short bcidEnergyRangeLow() const { return m_bcidEnergyRangeLow; }
  unsigned short bcidEnergyRangeHigh() const { return m_bcidEnergyRangeHigh; }
  unsigned short firStartBit() const { return m_firStartBit; }
  short firCoeff1() const { return m_firCoeff1; }
  short firCoeff2() const { return m_firCoeff2; }
  short firCoeff3() const { return m_firCoeff3; }
  short firCoeff4() const { return m_firCoeff4; }
  short firCoeff5() const { return m_firCoeff5; }
  unsigned short lutCpStrategy() const { return m_lutCpStrategy; }
  unsigned short lutCpOffset() const { return m_lutCpOffset; }
  unsigned short lutCpNoiseCut() const { return m_lutCpNoiseCut; }
  unsigned short lutCpSlope() const { return m_lutCpSlope; }
  short lutCpPar1() const { return m_lutCpPar1; }
  short lutCpPar2() const { return m_lutCpPar2; }
  short lutCpPar3() const { return m_lutCpPar3; }
  short lutCpPar4() const { return m_lutCpPar4; }
  unsigned short lutCpScale() const { return m_lutCpScale; }
  unsigned short lutJepStrategy() const { return m_lutJepStrategy; }
  unsigned short lutJepOffset() const { return m_lutJepOffset; }
  unsigned short lutJepNoiseCut() const { return m_lutJepNoiseCut; }
  unsigned short lutJepSlope() const { return m_lutJepSlope; }
  short lutJepPar1() const { return m_lutJepPar1; }
  short lutJepPar2() const { return m_lutJepPar2; }
  short lutJepPar3() const { return m_lutJepPar3; }
  short lutJepPar4() const { return m_lutJepPar4; }
  unsigned short lutJepScale() const { return m_lutJepScale; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setErrorCode(unsigned int errorCode) { m_errorCode = errorCode; }
  void setPedMean(double pedMean) { m_pedMean = pedMean; }
  void setPedValue(unsigned int pedValue) { m_pedValue = pedValue; }
  void setPedFirSum(unsigned int pedFirSum) { m_pedFirSum = pedFirSum; }
  void setExtBcidThreshold(unsigned short extBcidThreshold) { m_extBcidThreshold = extBcidThreshold; }
  void setSatBcidThreshLow(unsigned short satBcidThreshLow) { m_satBcidThreshLow = satBcidThreshLow; }
  void setSatBcidThreshHigh(unsigned short satBcidThreshHigh) { m_satBcidThreshHigh = satBcidThreshHigh; }
  void setSatBcidLevel(unsigned short satBcidLevel) { m_satBcidLevel = satBcidLevel; }
  void setBcidEnergyRangeLow(unsigned short bcidEnergyRangeLow) { m_bcidEnergyRangeLow = bcidEnergyRangeLow; }
  void setBcidEnergyRangeHigh(unsigned short bcidEnergyRangeHigh) { m_bcidEnergyRangeHigh = bcidEnergyRangeHigh; }
  void setFirStartBit(unsigned short firStartBit) { m_firStartBit = firStartBit; }
  void setFirCoeff1(short firCoeff1) { m_firCoeff1 = firCoeff1; }
  void setFirCoeff2(short firCoeff2) { m_firCoeff2 = firCoeff2; }
  void setFirCoeff3(short firCoeff3) { m_firCoeff3 = firCoeff3; }
  void setFirCoeff4(short firCoeff4) { m_firCoeff4 = firCoeff4; }
  void setFirCoeff5(short firCoeff5) { m_firCoeff5 = firCoeff5; }
  void setLutCpStrategy(unsigned short lutCpStrategy) { m_lutCpStrategy = lutCpStrategy; }
  void setLutCpOffset(unsigned short lutCpOffset) { m_lutCpOffset = lutCpOffset; }
  void setLutCpNoiseCut(unsigned short lutCpNoiseCut) { m_lutCpNoiseCut = lutCpNoiseCut; }
  void setLutCpSlope(unsigned short lutCpSlope) { m_lutCpSlope = lutCpSlope; }
  void setLutCpPar1(short lutCpPar1) { m_lutCpPar1 = lutCpPar1; }
  void setLutCpPar2(short lutCpPar2) { m_lutCpPar2 = lutCpPar2; }
  void setLutCpPar3(short lutCpPar3) { m_lutCpPar3 = lutCpPar3; }
  void setLutCpPar4(short lutCpPar4) { m_lutCpPar4 = lutCpPar4; }
  void setLutCpScale(unsigned short lutCpScale) { m_lutCpScale = lutCpScale; }
  void setLutJepStrategy(unsigned short lutJepStrategy) { m_lutJepStrategy = lutJepStrategy; }
  void setLutJepOffset(unsigned short lutJepOffset) { m_lutJepOffset = lutJepOffset; }
  void setLutJepNoiseCut(unsigned short lutJepNoiseCut) { m_lutJepNoiseCut = lutJepNoiseCut; }
  void setLutJepSlope(unsigned short lutJepSlope) { m_lutJepSlope = lutJepSlope; }
  void setLutJepPar1(short lutJepPar1) { m_lutJepPar1 = lutJepPar1; }
  void setLutJepPar2(short lutJepPar2) { m_lutJepPar2 = lutJepPar2; }
  void setLutJepPar3(short lutJepPar3) { m_lutJepPar3 = lutJepPar3; }
  void setLutJepPar4(short lutJepPar4) { m_lutJepPar4 = lutJepPar4; }
  void setLutJepScale(unsigned short lutJepScale) { m_lutJepScale = lutJepScale; }

private:
  unsigned int m_channelId = 0;
  unsigned int m_errorCode = 0;
  double m_pedMean = 0;
  unsigned int m_pedValue = 0;
  unsigned int m_pedFirSum = 0;
  unsigned short m_extBcidThreshold = 0;
  unsigned short m_satBcidThreshLow = 0;
  unsigned short m_satBcidThreshHigh = 0;
  unsigned short m_satBcidLevel = 0;
  unsigned short m_bcidEnergyRangeLow = 0;
  unsigned short m_bcidEnergyRangeHigh = 0;
  unsigned short m_firStartBit = 0;
  short m_firCoeff1 = 0;
  short m_firCoeff2 = 0;
  short m_firCoeff3 = 0;
  short m_firCoeff4 = 0;
  short m_firCoeff5 = 0;
  unsigned short m_lutCpStrategy = 0;
  unsigned short m_lutCpOffset = 0;
  unsigned short m_lutCpNoiseCut = 0;
  unsigned short m_lutCpSlope = 0;
  short m_lutCpPar1 = 0;
  short m_lutCpPar2 = 0;
  short m_lutCpPar3 = 0;
  short m_lutCpPar4 = 0;
  unsigned short m_lutCpScale = 0;
  unsigned short m_lutJepStrategy = 0;
  unsigned short m_lutJepOffset = 0;
  unsigned short m_lutJepNoiseCut = 0;
  unsigned short m_lutJepSlope = 0;
  short m_lutJepPar1 = 0;
  short m_lutJepPar2 = 0;
  short m_lutJepPar3 = 0;
  short m_lutJepPar4 = 0;
  unsigned short m_lutJepScale = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIB_H
