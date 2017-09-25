/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_RawEv/AFP_RawData.h"
#include "GaudiKernel/MsgStream.h"

//svn is acting up weirdly, comment to make it pick this up again

AFP_RawData::AFP_RawData():

 m_HitDiscConfig(0),
 m_Link(0), 
 m_Column(0),
 m_Row(0),
 m_ToT(0),
  m_bit16(0),
  m_bit18(0),
  m_error_bit17(0),
  m_bit26_27(0),
  m_bit24_27(0),
  m_lvl1(0)
{}

AFP_RawData::AFP_RawData(uint16_t Link):
  m_HitDiscConfig(0),
  m_Link(Link), 
  m_Column(0),
  m_Row(0),
  m_ToT(0),
  m_bit16(0),
  m_bit18(0),
  m_error_bit17(0),
  m_bit26_27(0),
  m_bit24_27(0),
  m_lvl1(0)
{}


AFP_RawData::~AFP_RawData()
{}




void AFP_RawData::Set_DiscConf(uint16_t Word_number)
{
m_HitDiscConfig = Word_number;
}

void AFP_RawData::Set_link(uint16_t PMF_number)
{
m_Link = PMF_number;
}

void AFP_RawData::Set_column(uint16_t MB_number)
{
m_Column = MB_number;
}

void AFP_RawData::Set_row(uint32_t Event_number)
{
m_Row = Event_number;
}

void AFP_RawData::Set_ToT(uint16_t HitChan)
{
  m_ToT=HitChan;
}

void AFP_RawData::PushBack_PMF(uint16_t fiber_hit){
  m_ToT=fiber_hit;
}

void AFP_RawData::addData(uint32_t dataWord)
{
  m_dataWords.push_back(dataWord);
}

void AFP_RawData::Set_error_bit17(bool bit)
{
  m_error_bit17=bit;
}

void AFP_RawData::Set_lvl1(uint16_t tmplvl1)
{
  m_lvl1=tmplvl1;
}

void AFP_RawData::SetZero_PMF()

{
  m_HitDiscConfig=0;
  m_Link=0;
  m_Column=0;
  m_Row=0;
  m_bit26_27=0;
  m_bit24_27=0;
  m_bit16=false;
  m_bit18=false;
  m_error_bit17=false;
  m_ToT = 0;
  m_lvl1 = 0;
  m_dataWords.clear();
}













