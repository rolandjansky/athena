// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSV1_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSV1_H

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V1/Configuration/PprChanDefaults .
 * Automatically created using:
 *
 * python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --include CR12_BcidDecision1,CR12_SatOverride1,CR13_BcidDecision2,CR13_SatOverride2,CR14_BcidDecision3,CR14_SatOverride3,CR15_PeakFinderCond,CR15_DecisionSource /TRIGGER/L1Calo/V1/Configuration/PprChanDefaults
 */
class L1CaloPprChanDefaultsV1
{
public:
  L1CaloPprChanDefaultsV1() {}
  L1CaloPprChanDefaultsV1(unsigned int channelId, unsigned int BcidDecision1, unsigned int SatOverride1, unsigned int BcidDecision2, unsigned int SatOverride2, unsigned int BcidDecision3, unsigned int SatOverride3, unsigned int PeakFinderCond, unsigned int DecisionSource);

  unsigned int channelId() const { return m_channelId; }
  unsigned int bcidDecision1() const { return m_bcidDecision1; }
  unsigned int satOverride1() const { return m_satOverride1; }
  unsigned int bcidDecision2() const { return m_bcidDecision2; }
  unsigned int satOverride2() const { return m_satOverride2; }
  unsigned int bcidDecision3() const { return m_bcidDecision3; }
  unsigned int satOverride3() const { return m_satOverride3; }
  unsigned int peakFinderCond() const { return m_peakFinderCond; }
  unsigned int decisionSource() const { return m_decisionSource; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setBcidDecision1(unsigned int BcidDecision1) { m_bcidDecision1 = BcidDecision1; }
  void setSatOverride1(unsigned int SatOverride1) { m_satOverride1 = SatOverride1; }
  void setBcidDecision2(unsigned int BcidDecision2) { m_bcidDecision2 = BcidDecision2; }
  void setSatOverride2(unsigned int SatOverride2) { m_satOverride2 = SatOverride2; }
  void setBcidDecision3(unsigned int BcidDecision3) { m_bcidDecision3 = BcidDecision3; }
  void setSatOverride3(unsigned int SatOverride3) { m_satOverride3 = SatOverride3; }
  void setPeakFinderCond(unsigned int PeakFinderCond) { m_peakFinderCond = PeakFinderCond; }
  void setDecisionSource(unsigned int DecisionSource) { m_decisionSource = DecisionSource; }

private:
  unsigned int m_channelId = 0;
  unsigned int m_bcidDecision1 = 0;
  unsigned int m_satOverride1 = 0;
  unsigned int m_bcidDecision2 = 0;
  unsigned int m_satOverride2 = 0;
  unsigned int m_bcidDecision3 = 0;
  unsigned int m_satOverride3 = 0;
  unsigned int m_peakFinderCond = 0;
  unsigned int m_decisionSource = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANDEFAULTSV1_H
