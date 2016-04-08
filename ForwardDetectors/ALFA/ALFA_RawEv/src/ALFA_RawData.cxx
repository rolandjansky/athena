/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawData.h"
#include "GaudiKernel/MsgStream.h"

ALFA_RawData::ALFA_RawData():
  m_WordId(0),
  m_PMFId(0),  
  m_MBId(0), 
  m_EventCount(0),
  m_MarocChan(),
  m_bit16(0),
  m_bit18(0),
  m_error_bit17(0),
  m_bit26_27(0),
  m_bit24_27(0)
{
}

ALFA_RawData::ALFA_RawData(uint16_t PMFId):
  m_WordId(0),
  m_PMFId(PMFId),  
  m_MBId(0), 
  m_EventCount(0),
  m_MarocChan(),
  m_bit16(0),
  m_bit18(0),
  m_error_bit17(0),
  m_bit26_27(0),
  m_bit24_27(0)
{
}

////////////////////////
// destructor
////////////////////////
ALFA_RawData::~ALFA_RawData()
{
}


void ALFA_RawData::SetWordId_PMF(uint16_t Word_number)
{
  m_WordId=Word_number;
}

void ALFA_RawData::SetPMFId_PMF(uint16_t PMF_number)
{
  m_PMFId=PMF_number;
}

void ALFA_RawData::SetMBId_PMF(uint16_t MB_number)
{
  m_MBId=MB_number;
}

void ALFA_RawData::SetEventCount_PMF(uint32_t Event_number)
{
  m_EventCount=Event_number;
}


void ALFA_RawData::SetMarocChan_PMF(const std::vector<uint16_t>& HitChan)
{
  m_MarocChan=HitChan;


}

void ALFA_RawData::PushBack_PMF(uint16_t fiber_hit){
  m_MarocChan.push_back(fiber_hit);
}

void ALFA_RawData::addData(uint32_t dataWord) 
{
  m_dataWords.push_back(dataWord);
}

void ALFA_RawData::Set_error_bit17(bool bit)
{
  m_error_bit17=bit;
}


void ALFA_RawData::SetZero_PMF() 
{
  m_WordId=0;
  m_PMFId=0; 
  m_MBId=0;
  m_EventCount=0; 
  m_bit26_27=0;
  m_bit24_27=0;
  m_bit16=false;
  m_bit18=false;
  m_error_bit17=false;
  m_MarocChan.clear();
  m_dataWords.clear();
}


