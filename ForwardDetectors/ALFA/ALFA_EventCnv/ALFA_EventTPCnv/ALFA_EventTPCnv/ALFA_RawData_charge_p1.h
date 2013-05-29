/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATA_CHARGE_P1_H
#define ALFA_RAWDATA_CHARGE_P1_H

#include <inttypes.h>

#include<stdint.h>
#include<vector>
#include <iostream>

class ALFA_RawData_charge_p1 {
  
  private:
  
   // Data info TSM: 
  uint16_t ChannelNumId;
  uint16_t PMFId;  
  uint16_t MBId; 
  uint32_t EventCount;
  std::vector<uint16_t> m_ChargeChan; 
  
  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using ALFA_ReadOut methods
  std::vector<uint32_t> m_dataWords;  
  
  
 public:
  
  friend class ALFA_RawDataCnv_charge_p1;
  
   
   ALFA_RawData_charge_p1():
  
  ChannelNumId(0), 
  PMFId(0),
  MBId(0),
  EventCount(0) 
  
  {}
  
  
  
 
};

#endif
