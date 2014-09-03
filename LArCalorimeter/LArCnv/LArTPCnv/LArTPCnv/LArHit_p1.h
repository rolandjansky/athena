// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHIT_P1_H
#define LARHIT_P1_H
/** @class LArHit_p1
    @brief Persistent LArHit class, version p1.
*/

class LArHit_p1 {
 public:
  LArHit_p1()
    : m_channelID(0),
      m_energy(0),
      m_time(0)
  {};
  friend class LArHitCnv_p1;
  friend class LArHitFloatCnv_p1;
  
 private:
  /** identifier of the cell in which this hit occured. */
  unsigned int m_channelID;
  
  /** energy (in MeV) deposited in this hit */
  float m_energy;
  
  // time (in ns)
  float m_time;             
};

#endif
