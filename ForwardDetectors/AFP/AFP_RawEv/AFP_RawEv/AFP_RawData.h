/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATA_H
#define AFP_RAWDATA_H

#include <stdint.h>
#include<vector>
#include <iostream>

class MsgStream;

class AFP_RawData
{

public:

 AFP_RawData();

 AFP_RawData(uint16_t PMF_number);
 ~AFP_RawData();

  void Set_DiscConf(uint16_t Word_number);
  void Set_link(uint16_t PMF_number);
  void Set_column(uint16_t MB_number);
  void Set_row(uint32_t Event_number);
  void Set_ToT(uint16_t ToT);
  void PushBack_PMF(uint16_t fiber_hit);
  void Set_error_bit17(bool bit);
  void Set_lvl1(uint16_t tmplvl1);
  void SetZero_PMF();
  void addData(uint32_t dataWord);



  uint16_t Get_DiscConf() const {return m_HitDiscConfig;};
  uint16_t Get_link() const {return m_Link;};
  uint16_t Get_column() const {return m_Column;};
  uint32_t Get_row() const {return m_Row;};
  uint16_t Get_ToT() const {return m_ToT;};
//  uint32_t Get_Chan(uint16_t Numchan) const {return 0;}; 
  uint32_t GetSize() const {return 0;};
  uint16_t Get_bit26_27() const {return m_bit26_27;};
  uint16_t Get_bit24_27() const  {return m_bit24_27;}; 
  uint16_t Get_lvl1() const {return m_lvl1;};
  bool Get_bit16() const {return m_bit16;};
  bool Get_bit18() const {return m_bit18;};
  bool Get_error_bit17() const {return m_error_bit17;};



 const std::vector<uint32_t>* dataWords() const {return &m_dataWords;}


private:

  uint16_t m_HitDiscConfig;
  uint16_t m_Link; 
  uint16_t m_Column; 
  uint32_t m_Row;
  uint16_t m_ToT;
  bool     m_bit16;
  bool     m_bit18;
  bool     m_error_bit17;
  uint16_t     m_bit26_27;
  uint16_t     m_bit24_27;      
  uint16_t m_lvl1;

std::vector<uint32_t> m_dataWords;
};
#endif
