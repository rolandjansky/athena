/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILESIMEVENT_TILEHIT_P1_H
#define TILESIMEVENT_TILEHIT_P1_H
#include <vector>

class TileHit_p1 {
 public:
  TileHit_p1() : m_channelID(0), m_energy(), m_time() {};
  friend class TileHitCnv_p1;
  
 private:
  unsigned int m_channelID;            // identifier of the cell in which this hit occured.
  std::vector<float> m_energy;         // energy deposited in scint, as seen by pmt 
  std::vector<float> m_time;           // time of the energy deposition relative to BC
};

#endif
