/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawData_charge.h"
#include "GaudiKernel/MsgStream.h"

ALFA_RawData_charge::ALFA_RawData_charge():
  m_ChannelNumId(0),
  m_PMFId(0),
  m_MBId(0),
  m_FiberFirmwareId(0),
  m_EventCount(0),
  m_ChargeChanId(0),
  m_bit12(0),
  m_ChargeChan() 
{
}

ALFA_RawData_charge::ALFA_RawData_charge(uint16_t PMFId):
  m_ChannelNumId(0),
  m_PMFId(PMFId),
  m_MBId(0),
  m_FiberFirmwareId(0),
  m_EventCount(0),
  m_ChargeChanId(0),
  m_bit12(0),
  m_ChargeChan() 
{
}

////////////////////////
// destructor
////////////////////////
ALFA_RawData_charge::~ALFA_RawData_charge()
{
}

void ALFA_RawData_charge::SetPMFId_PMF(uint16_t PMF_number)
{
  m_PMFId=PMF_number;
}

void ALFA_RawData_charge::SetChannelNum_PMF(uint16_t Channel_number)
{
  m_ChannelNumId=Channel_number;
}

void ALFA_RawData_charge::SetMBId_PMF(uint16_t MB_number)
{
  m_MBId=MB_number;
}

void ALFA_RawData_charge::SetFiberFirmware_PMF(uint16_t FiberFirmware_number)
{
  m_FiberFirmwareId=FiberFirmware_number;
}

void ALFA_RawData_charge::SetEventCount_PMF(uint32_t Event_number)
{
  m_EventCount=Event_number;
}


void ALFA_RawData_charge::SetChargeChan_PMF(uint16_t ChargeChan_number)
{
  m_ChargeChanId=ChargeChan_number;
}


void ALFA_RawData_charge::SetChargeChanVect_PMF(const std::vector<uint16_t>& ChargeChanVect_number)
{
  m_ChargeChan=ChargeChanVect_number;
}


void ALFA_RawData_charge::PushBack_PMF(uint16_t ChargeChan_number){
  m_ChargeChan.push_back(ChargeChan_number);
}


void ALFA_RawData_charge::addData(uint32_t dataWord) 
{
  m_dataWords.push_back(dataWord);
}


void ALFA_RawData_charge::SetZero_PMF() 
{
  m_PMFId=0;
  m_ChannelNumId=0;
  m_MBId=0;
  m_EventCount=0; 
  m_FiberFirmwareId=0; 
  m_bit12=false;
  m_ChargeChan.clear();
  m_dataWords.clear();
}


