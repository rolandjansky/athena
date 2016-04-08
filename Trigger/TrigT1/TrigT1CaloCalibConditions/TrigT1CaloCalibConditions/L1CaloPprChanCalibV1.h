// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBV1_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBV1_H

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib .
 * Automatically created using:
 *
 * python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude ModuleId,PprDacScanResultsTimeStamp,PprPedestalRunResultsTimeStamp,PprNoiseRunResultsTimeStamp,PprTimingResultsTimeStamp,PprSatBcidResultsTimeStamp,PprFirFilterResultsTimeStamp,PprLutValuesResultsTimeStamp,DacOffset,DacSlope,FullDelayData,SyncDelayBcid,InBcidNegedge,rateCpEtThresh,rateJepEtThresh,LutParameters /TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib --objectName L1CaloPprChanCalibV1 --containerName L1CaloPprChanCalibV1Container
 */
class L1CaloPprChanCalibV1
{
public:
  L1CaloPprChanCalibV1() {}
  L1CaloPprChanCalibV1(unsigned int channelId, unsigned int errorCode, double pedMean, unsigned int pedValue, unsigned short extBcidThreshold, unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel, unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned short firStartBit, short firCoeff1, short firCoeff2, short firCoeff3, short firCoeff4, short firCoeff5, unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutNoiseCut, unsigned short lutSlope);

  unsigned int channelId() const { return m_channelId; }
  unsigned int errorCode() const { return m_errorCode; }
  double pedMean() const { return m_pedMean; }
  unsigned int pedValue() const { return m_pedValue; }
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
  unsigned short lutStrategy() const { return m_lutStrategy; }
  unsigned short lutOffset() const { return m_lutOffset; }
  unsigned short lutNoiseCut() const { return m_lutNoiseCut; }
  unsigned short lutSlope() const { return m_lutSlope; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setErrorCode(unsigned int errorCode) { m_errorCode = errorCode; }
  void setPedMean(double pedMean) { m_pedMean = pedMean; }
  void setPedValue(unsigned int pedValue) { m_pedValue = pedValue; }
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
  void setLutStrategy(unsigned short lutStrategy) { m_lutStrategy = lutStrategy; }
  void setLutOffset(unsigned short lutOffset) { m_lutOffset = lutOffset; }
  void setLutNoiseCut(unsigned short lutNoiseCut) { m_lutNoiseCut = lutNoiseCut; }
  void setLutSlope(unsigned short lutSlope) { m_lutSlope = lutSlope; }

private:
  unsigned int m_channelId = 0;
  unsigned int m_errorCode = 0;
  double m_pedMean = 0;
  unsigned int m_pedValue = 0;
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
  unsigned short m_lutStrategy = 0;
  unsigned short m_lutOffset = 0;
  unsigned short m_lutNoiseCut = 0;
  unsigned short m_lutSlope = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIBV1_H
