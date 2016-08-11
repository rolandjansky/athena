/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATA_P1_H
#define AFP_RAWDATA_P1_H

#include <inttypes.h>

#include<stdint.h>
#include<vector>
#include <iostream>
class AFP_RawData_p1 {
	 
  private:
 
   // Data info TSM:
  uint16_t HitDiscConfig;
  uint16_t Link; 
  uint16_t Column; 
  uint32_t Row;
  uint16_t ToT; // sostituisce il p_hit di prima
  uint16_t lvl1;
  bool m_error_bit17;
 
  // All the datawords (no headers and footers) coming from this channel
  // to be decoded on demand using AFP_ReadOut methods
 std::vector<uint32_t> m_dataWords; 
 
 
 public:
 
  friend class AFP_RawDataCnv_p1;
 

  AFP_RawData_p1():
   
    HitDiscConfig(0), 
    Link(0),
    Column(0),
    Row(0),
    ToT(0),
    m_error_bit17(0),
    lvl1(0)
      {}
 
 
};

#endif
