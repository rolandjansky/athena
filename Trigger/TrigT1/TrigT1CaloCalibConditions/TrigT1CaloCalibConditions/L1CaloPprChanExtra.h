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
class L1CaloPprChanExtra
{
public:
  L1CaloPprChanExtra() {}
  L1CaloPprChanExtra(unsigned int channelId, unsigned int errorCode, unsigned short pedCorrEnable, unsigned short sat80BcidThreshHigh, unsigned short sat80BcidThreshLow, unsigned short threshNumSamplesPeak, unsigned short threshNumSamplesSat, unsigned short satOverride1, unsigned short satOverride2, unsigned short SatOverride3, short lutCpPar5, short lutCpPar6, short lutCpPar7, short lutJepPar5, short lutJepPar6, short lutJepPar7);

  unsigned int channelId() const { return m_channelId; }
  unsigned int errorCode() const { return m_errorCode; }
  unsigned short pedCorrEnable() const { return m_pedCorrEnable; }
  unsigned short sat80BcidThreshHigh() const { return m_sat80BcidThreshHigh; }
  unsigned short sat80BcidThreshLow() const { return m_sat80BcidThreshLow; }
  unsigned short threshNumSamplesPeak() const { return m_threshNumSamplesPeak; }
  unsigned short threshNumSamplesSat() const { return m_threshNumSamplesSat; }
  unsigned short satOverride1() const { return m_satOverride1; }
  unsigned short satOverride2() const { return m_satOverride2; }
  unsigned short satOverride3() const { return m_satOverride3; }
  short lutCpPar5() const { return m_lutCpPar5; }
  short lutCpPar6() const { return m_lutCpPar6; }
  short lutCpPar7() const { return m_lutCpPar7; }
  short lutJepPar5() const { return m_lutJepPar5; }
  short lutJepPar6() const { return m_lutJepPar6; }
  short lutJepPar7() const { return m_lutJepPar7; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setErrorCode(unsigned int errorCode) { m_errorCode = errorCode; }
  void setPedCorrEnable(unsigned short pedCorrEnable) { m_pedCorrEnable = pedCorrEnable; }
  void setSat80BcidThreshHigh(unsigned short sat80BcidThreshHigh) { m_sat80BcidThreshHigh = sat80BcidThreshHigh; }
  void setSat80BcidThreshLow(unsigned short sat80BcidThreshLow) { m_sat80BcidThreshLow = sat80BcidThreshLow; }
  void setThreshNumSamplesPeak(unsigned short threshNumSamplesPeak) { m_threshNumSamplesPeak = threshNumSamplesPeak; }
  void setThreshNumSamplesSat(unsigned short threshNumSamplesSat) { m_threshNumSamplesSat = threshNumSamplesSat; }
  void setSatOverride1(unsigned short satOverride1) { m_satOverride1 = satOverride1; }
  void setSatOverride2(unsigned short satOverride2) { m_satOverride2 = satOverride2; }
  void setSatOverride3(unsigned short satOverride3) { m_satOverride3 = satOverride3; }
  void setLutCpPar5(short lutCpPar5) { m_lutCpPar5 = lutCpPar5; }
  void setLutCpPar6(short lutCpPar6) { m_lutCpPar6 = lutCpPar6; }
  void setLutCpPar7(short lutCpPar7) { m_lutCpPar7 = lutCpPar7; }
  void setLutJepPar5(short lutJepPar5) { m_lutJepPar5 = lutJepPar5; }
  void setLutJepPar6(short lutJepPar6) { m_lutJepPar6 = lutJepPar6; }
  void setLutJepPar7(short lutJepPar7) { m_lutJepPar7 = lutJepPar7; }

private:
  unsigned int m_channelId = 0;
  unsigned int m_errorCode = 0;
  unsigned short m_pedCorrEnable = 0;
  unsigned short m_sat80BcidThreshHigh = 0;
  unsigned short m_sat80BcidThreshLow = 0;
  unsigned short m_threshNumSamplesPeak = 0;
  unsigned short m_threshNumSamplesSat = 0;
  unsigned short m_satOverride1 = 0;
  unsigned short m_satOverride2 = 0;
  unsigned short m_satOverride3 = 0;
  short m_lutCpPar5 = 0;
  short m_lutCpPar6 = 0;
  short m_lutCpPar7 = 0;
  short m_lutJepPar5 = 0;
  short m_lutJepPar6 = 0;
  short m_lutJepPar7 = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANCALIB_H
