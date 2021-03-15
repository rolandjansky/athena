/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_crc8_H
#define TrigT1RPChardware_crc8_H

class crc8 {
public:
      crc8();
      ~crc8();
      int calc(int dato);
      int current();
private:
      int m_crci;
      int m_pol;
      int m_ma;
}; 
#endif
