// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTS_H

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V2/Configuration/PprChanDefaults .
 * Automatically created using:
 *
 * python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --include CR12_BcidDecision1,CR12_SatOverride1,CR13_BcidDecision2,CR13_SatOverride2,CR14_BcidDecision3,CR14_SatOverride3,CR15_PeakFinderCond,CR15_DecisionSource /TRIGGER/L1Calo/V2/Configuration/PprChanDefaults
 */
class L1CaloPprChanDefaults
{
public:
  L1CaloPprChanDefaults() {}
  L1CaloPprChanDefaults(unsigned int channelId, int bcidDecision1, int satOverride1, int bcidDecision2, int satOverride2, int bcidDecision3, int satOverride3, int peakFinderCond, int decisionSource, int bypassLutCp = 0, int bypassLutJep = 0, int inDataSample = 0, int readOut80Active = 0, int firCoeff1 = 0, int firCoeff2 = 0, int firCoeff3 = 1, int firCoeff4 = 0, int firCoeff5 = 0, int satHigh = 767, int satLow = 255, int startBit = 0, int energyLevelHigh = 895, int energyLevelLow = 511, int satLevel = 1023, int saturationValue = 255, int numBcRaw = 5, int numBcBcid = 1, int pedCorrEnable = 1, int sat80High = 767, int sat80Low = 255);

  unsigned int channelId() const { return m_channelId; }
  int bcidDecision1() const { return m_bcidDecision1; }
  int satOverride1() const { return m_satOverride1; }
  int bcidDecision2() const { return m_bcidDecision2; }
  int satOverride2() const { return m_satOverride2; }
  int bcidDecision3() const { return m_bcidDecision3; }
  int satOverride3() const { return m_satOverride3; }
  int peakFinderCond() const { return m_peakFinderCond; }
  int decisionSource() const { return m_decisionSource; }

  int bypassLutCp() const { return m_bypassLutCp; }
  int bypassLutJep() const { return m_bypassLutJep; }
  int inDataSample() const { return m_inDataSample; }
  int readOut80Active() const { return m_readOut80Active; }
  int firCoeff1() const { return m_firCoeff1; }
  int firCoeff2() const { return m_firCoeff2; }
  int firCoeff3() const { return m_firCoeff3; }
  int firCoeff4() const { return m_firCoeff4; }
  int firCoeff5() const { return m_firCoeff5; }
  int satHigh() const { return m_satHigh; }
  int satLow() const { return m_satLow; }
  int startBit() const { return m_startBit; }
  int energyLevelHigh() const { return m_energyLevelHigh; }
  int energyLevelLow() const { return m_energyLevelLow; }
  int satLevel() const { return m_satLevel; }
  int saturationValue() const { return m_saturationValue; }
  int numBcRaw() const { return m_numBcRaw; }
  int numBcBcid() const { return m_numBcBcid; }
  int pedCorrEnable() const { return m_pedCorrEnable; }
  int sat80High() const { return m_sat80High; }
  int sat80Low() const { return m_sat80Low; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setBcidDecision1(int bcidDecision1) { m_bcidDecision1 = bcidDecision1; }
  void setSatOverride1(int satOverride1) { m_satOverride1 = satOverride1; }
  void setBcidDecision2(int bcidDecision2) { m_bcidDecision2 = bcidDecision2; }
  void setSatOverride2(int satOverride2) { m_satOverride2 = satOverride2; }
  void setBcidDecision3(int bcidDecision3) { m_bcidDecision3 = bcidDecision3; }
  void setSatOverride3(int satOverride3) { m_satOverride3 = satOverride3; }
  void setPeakFinderCond(int peakFinderCond) { m_peakFinderCond = peakFinderCond; }
  void setDecisionSource(int decisionSource) { m_decisionSource = decisionSource; }

  void setBypassLutCp(int bypassLutCp) { m_bypassLutCp = bypassLutCp; }
  void setBypassLutJep(int bypassLutJep) { m_bypassLutJep = bypassLutJep; }
  void setInDataSample(int inDataSample) { m_inDataSample = inDataSample; }
  void setReadOut80Active(int readOut80Active) { m_readOut80Active = readOut80Active; }
  void setFirCoeff1(int firCoeff1) { m_firCoeff1 = firCoeff1; }
  void setFirCoeff2(int firCoeff2) { m_firCoeff2 = firCoeff2; }
  void setFirCoeff3(int firCoeff3) { m_firCoeff3 = firCoeff3; }
  void setFirCoeff4(int firCoeff4) { m_firCoeff4 = firCoeff4; }
  void setFirCoeff5(int firCoeff5) { m_firCoeff5 = firCoeff5; }
  void setSatHigh(int satHigh) { m_satHigh = satHigh; }
  void setSatLow(int satLow) { m_satLow = satLow; }
  void setStartBit(int startBit) { m_startBit = startBit; }
  void setEnergyLevelHigh(int energyLevelHigh) { m_energyLevelHigh = energyLevelHigh; }
  void setEnergyLevelLow(int energyLevelLow) { m_energyLevelLow = energyLevelLow; }
  void setSatLevel(int satLevel) { m_satLevel = satLevel; }
  void setSaturationValue(int saturationValue) { m_saturationValue = saturationValue; }
  void setNumBcRaw(int numBcRaw) { m_numBcRaw = numBcRaw; }
  void setNumBcBcid(int numBcBcid) { m_numBcBcid = numBcBcid; }
  void setPedCorrEnable(int pedCorrEnable) { m_pedCorrEnable = pedCorrEnable; }
  void setSat80High(int sat80High) { m_sat80High = sat80High; }
  void setSat80Low(int sat80Low) { m_sat80Low = sat80Low; }

private:
  unsigned int m_channelId = 0;
  int m_bcidDecision1 = 252;
  int m_satOverride1 = 1;
  int m_bcidDecision2 = 240;
  int m_satOverride2 = 0;
  int m_bcidDecision3 = 240;
  int m_satOverride3 = 0;
  int m_peakFinderCond = 0;
  int m_decisionSource = 0;

  int m_bypassLutCp = 0;
  int m_bypassLutJep = 0;
  int m_inDataSample = 0;
  int m_readOut80Active = 0;
  int m_firCoeff1 = 0;
  int m_firCoeff2 = 0;
  int m_firCoeff3 = 1;
  int m_firCoeff4 = 0;
  int m_firCoeff5 = 0;
  int m_satHigh = 767;
  int m_satLow = 255;
  int m_startBit = 0;
  int m_energyLevelHigh = 895;
  int m_energyLevelLow = 511;
  int m_satLevel = 1023;
  int m_saturationValue = 255;
  int m_numBcRaw = 5;
  int m_numBcBcid = 1;
  int m_pedCorrEnable = 1;
  int m_sat80High = 767;
  int m_sat80Low = 255;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTS_H
