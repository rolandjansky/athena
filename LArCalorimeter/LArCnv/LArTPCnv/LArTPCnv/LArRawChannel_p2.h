// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARRAWCHANNEL_P2_H
#define LARTPCNV_LARRAWCHANNEL_P2_H

/** @class LArRawChannel_p2
@brief persistent class for LArRawChannel, version p2.
*/

class LArRawChannel_p2 {
  
 public:
  
  LArRawChannel_p2()
    : m_channelID(0),
      m_energy(0),
      m_time(0),
      m_qualityandgain(0)
  { };
  
  friend class LArRawChannelCnv_p2;
  friend class LArRawChannelContainerCnv_p4;
    
 private:
  
  unsigned int m_channelID;
  int m_energy;
  int m_time;
  
  unsigned int m_qualityandgain;
};

#endif
