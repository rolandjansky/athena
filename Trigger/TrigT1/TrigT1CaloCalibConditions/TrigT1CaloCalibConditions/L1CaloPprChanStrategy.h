// -*- C++ -*-
#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANSTRATEGY_H
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANSTRATEGY_H

#include <iostream>
#include <string>
/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V2/Configuration/PprChanStrategy .
 * Automatically created using:
 *
 * ../athena/Trigger/TrigT1/TrigT1CaloCalibConditions/python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V2/Configuration/PprChanStrategy
 */
class L1CaloPprChanStrategy
{
  friend std::ostream& operator<<(std::ostream& output, const L1CaloPprChanStrategy& r);
public:
  L1CaloPprChanStrategy() {}
  L1CaloPprChanStrategy(unsigned int channelId, const std::string& strategy, unsigned int code, const std::string& timingRegime, const std::string& description);

  unsigned int channelId() const { return m_channelId; }
  std::string strategy() const { return m_strategy; }
  unsigned int code() const { return m_code; }
  std::string  timingRegime() const { return m_timingRegime; }
  std::string  description() const { return m_description; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setStrategy(const std::string&  strategy) { m_strategy = strategy; }
  void setCode(unsigned int code) { m_code = code; }
  void setTimingRegime(const std::string&  timingRegime) { m_timingRegime = timingRegime; }
  void setDescription(const std::string& description) { m_description = description; }

private:
  unsigned int m_channelId = 0;
  std::string  m_strategy = 0;
  unsigned int m_code = 0;
  std::string  m_timingRegime = 0;
  std::string  m_description = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCHANSTRATEGY_H
