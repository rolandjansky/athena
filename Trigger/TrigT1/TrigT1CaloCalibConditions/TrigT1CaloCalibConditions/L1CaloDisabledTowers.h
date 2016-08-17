// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALODISABLEDTOWERS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALODISABLEDTOWERS_H

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V1/Conditions/DisabledTowers .
 * Automatically created using:
 *
 * python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 /TRIGGER/L1Calo/V1/Conditions/DisabledTowers
 */
class L1CaloDisabledTowers
{
public:
  L1CaloDisabledTowers() {}
  L1CaloDisabledTowers(unsigned int channelId, unsigned int disabledBits);

  unsigned int channelId() const { return m_channelId; }
  unsigned int disabledBits() const { return m_disabledBits; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setdisabledBits(unsigned int disabledBits) { m_disabledBits = disabledBits; }

private:
  unsigned int m_channelId = 0;
  unsigned int m_disabledBits = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALODISABLEDTOWERS_H
