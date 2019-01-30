/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALODERIVEDRUNPARS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALODERIVEDRUNPARS_H

#include <string>
#include <iostream>

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V1/Conditions/DerivedRunPars .
 * Automatically created using:
 *
 * ../python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V1/Conditions/DerivedRunPars
 */
class L1CaloDerivedRunPars
{
  friend std::ostream& operator<<(std::ostream& output, const L1CaloDerivedRunPars& r);
public:
  L1CaloDerivedRunPars() {}
  L1CaloDerivedRunPars(unsigned int channelId, const std::string& timingRegime, const std::string& tierZeroTag);

  unsigned int channelId() const { return m_channelId; }
  std::string timingRegime() const { return m_timingRegime; }
  std::string tierZeroTag() const { return m_tierZeroTag; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void settimingRegime(const std::string& timingRegime) { m_timingRegime = timingRegime; }
  void settierZeroTag(const std::string& tierZeroTag) { m_tierZeroTag = tierZeroTag; }

private:
  unsigned int m_channelId = 0;
  std::string m_timingRegime = 0;
  std::string m_tierZeroTag = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALODERIVEDRUNPARS_H
