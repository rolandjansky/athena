// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELS_H

/**
 * Folder <-> Object mapping for /TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels .
 * Automatically created using:
 *
 * python/CreateClassesForFolder.py --db frontier://ATLF/();schema=ATLAS_COOLONL_TRIGGER;dbname=CONDBR2 --exclude Comment /TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels
 */
class L1CaloPpmDeadChannels
{
public:
  L1CaloPpmDeadChannels() {}
  L1CaloPpmDeadChannels(unsigned int channelId, unsigned int errorCode, unsigned int noiseCut);

  unsigned int channelId() const { return m_channelId; }
  unsigned int errorCode() const { return m_errorCode; }
  unsigned int noiseCut() const { return m_noiseCut; }

  void setChannelId(unsigned int channelId) { m_channelId = channelId; }
  void setErrorCode(unsigned int errorCode) { m_errorCode = errorCode; }
  void setNoiseCut(unsigned int noiseCut) { m_noiseCut = noiseCut; }

private:
  unsigned int m_channelId = 0;
  unsigned int m_errorCode = 0;
  unsigned int m_noiseCut = 0;
};

#endif // TRIGT1CALOCALIBCONDITIONS_L1CALOPPMDEADCHANNELS_H
