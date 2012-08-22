/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawData_charge.h"
#include "GaudiKernel/MsgStream.h"

ALFA_RawData_charge::ALFA_RawData_charge():
  ChannelNumId(0),
  PMFId(0),
  MBId(0),
  FiberFirmwareId(0),
  EventCount(0),
  ChargeChanId(0),
  m_bit12(0),
  ChargeChan() 
{
  p_dataWords = new std::vector<uint32_t>();
}

ALFA_RawData_charge::ALFA_RawData_charge(uint16_t PMFId):
  ChannelNumId(0),
  PMFId(PMFId),
  MBId(0),
  FiberFirmwareId(0),
  EventCount(0),
  ChargeChanId(0),
  m_bit12(0),
  ChargeChan() 
{
  p_dataWords = new std::vector<uint32_t>();
}

////////////////////////
// destructor
////////////////////////
ALFA_RawData_charge::~ALFA_RawData_charge()
{
  if (p_dataWords) delete p_dataWords;
}

void ALFA_RawData_charge::SetPMFId_PMF(uint16_t PMF_number)
{
  PMFId=PMF_number;
}

void ALFA_RawData_charge::SetChannelNum_PMF(uint16_t Channel_number)
{
  ChannelNumId=Channel_number;
}

void ALFA_RawData_charge::SetMBId_PMF(uint16_t MB_number)
{
  MBId=MB_number;
}

void ALFA_RawData_charge::SetFiberFirmware_PMF(uint16_t FiberFirmware_number)
{
  FiberFirmwareId=FiberFirmware_number;
}

void ALFA_RawData_charge::SetEventCount_PMF(uint32_t Event_number)
{
  EventCount=Event_number;
}


void ALFA_RawData_charge::SetChargeChan_PMF(uint16_t ChargeChan_number)
{
  ChargeChanId=ChargeChan_number;
}


void ALFA_RawData_charge::SetChargeChanVect_PMF(std::vector<uint16_t> ChargeChanVect_number)
{
  ChargeChan=ChargeChanVect_number;
}


void ALFA_RawData_charge::PushBack_PMF(uint16_t ChargeChan_number){
  ChargeChan.push_back(ChargeChan_number);
}


void ALFA_RawData_charge::addData(uint32_t dataWord) 
{
  p_dataWords->push_back(dataWord);
}


void ALFA_RawData_charge::SetZero_PMF() 
{
  PMFId=0;
  ChannelNumId=0;
  MBId=0;
  EventCount=0; 
  FiberFirmwareId=0; 
  m_bit12=false;
  ChargeChan.clear();
  p_dataWords->clear();
}


