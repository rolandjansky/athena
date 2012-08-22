/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawData.h"
#include "GaudiKernel/MsgStream.h"

ALFA_RawData::ALFA_RawData():
  WordId(0),
  PMFId(0),  
  MBId(0), 
  EventCount(0),
  MarocChan(),
  m_bit16(0),
  m_bit18(0),
  m_error_bit17(0),
  m_bit26_27(0),
  m_bit24_27(0)
{
  p_dataWords = new std::vector<uint32_t>();
}

ALFA_RawData::ALFA_RawData(uint16_t PMFId):
  WordId(0),
  PMFId(PMFId),  
  MBId(0), 
  EventCount(0),
  MarocChan(),
  m_bit16(0),
  m_bit18(0),
  m_error_bit17(0),
  m_bit26_27(0),
  m_bit24_27(0)
{
  p_dataWords = new std::vector<uint32_t>();
}

////////////////////////
// destructor
////////////////////////
ALFA_RawData::~ALFA_RawData()
{
  if (p_dataWords) delete p_dataWords;
}


void ALFA_RawData::SetWordId_PMF(uint16_t Word_number)
{
  WordId=Word_number;
}

void ALFA_RawData::SetPMFId_PMF(uint16_t PMF_number)
{
  PMFId=PMF_number;
}

void ALFA_RawData::SetMBId_PMF(uint16_t MB_number)
{
  MBId=MB_number;
}

void ALFA_RawData::SetEventCount_PMF(uint32_t Event_number)
{
  EventCount=Event_number;
}


void ALFA_RawData::SetMarocChan_PMF(std::vector<uint16_t> HitChan)
{
  MarocChan=HitChan;


}

void ALFA_RawData::PushBack_PMF(uint16_t fiber_hit){
  MarocChan.push_back(fiber_hit);
}

void ALFA_RawData::addData(uint32_t dataWord) 
{
  p_dataWords->push_back(dataWord);
}

void ALFA_RawData::Set_error_bit17(bool bit)
{
  m_error_bit17=bit;
}


void ALFA_RawData::SetZero_PMF() 
{
  WordId=0;
  PMFId=0; 
  MBId=0;
  EventCount=0; 
  m_bit26_27=0;
  m_bit24_27=0;
  m_bit16=false;
  m_bit18=false;
  m_error_bit17=false;
  MarocChan.clear();
  p_dataWords->clear();
}


