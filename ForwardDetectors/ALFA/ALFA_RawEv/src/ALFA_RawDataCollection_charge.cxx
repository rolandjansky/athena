/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
//#include "GaudiKernel/MsgStream.h"

// Default constructor
ALFA_RawDataCollection_charge::ALFA_RawDataCollection_charge() :
  DataVector<ALFA_RawData_charge>(),
  m_MBId_POT(0),
  m_MrodId_POT(0),
  m_EventCount_POT(0),
  m_POT_DATA(),
  m_TimeStamp_POT(0),
  m_BCId_POT(0)
{ }

// Full constructor
ALFA_RawDataCollection_charge::ALFA_RawDataCollection_charge(uint16_t MB_number) :
  DataVector<ALFA_RawData_charge>(),
  m_MBId_POT(MB_number),
  m_MrodId_POT(0),
  m_EventCount_POT(0),
  m_POT_DATA(),
  m_TimeStamp_POT(0),
  m_BCId_POT(0)
{ }

// Full constructor
ALFA_RawDataCollection_charge::ALFA_RawDataCollection_charge( uint16_t Mrod_number, uint16_t MB_number) :
  DataVector<ALFA_RawData_charge>(),
  m_MBId_POT(MB_number),
  m_MrodId_POT(Mrod_number),
  m_EventCount_POT(0),
  m_POT_DATA(),
  m_TimeStamp_POT(0),
  m_BCId_POT(0)
{ }



void ALFA_RawDataCollection_charge::SetMBId_POT(uint16_t MB_number)
{
  m_MBId_POT=MB_number;
}  
  
  
void ALFA_RawDataCollection_charge::SetMrodId_POT(uint16_t Mrod_number)
{
  m_MrodId_POT=Mrod_number;
  }  


void ALFA_RawDataCollection_charge::SetEventCount_POT(uint32_t Event_number)
{
  m_EventCount_POT=Event_number;
}

void ALFA_RawDataCollection_charge::SetTimeStamp_POT(uint32_t TimeStamp)
{
  m_TimeStamp_POT=TimeStamp;
}

void ALFA_RawDataCollection_charge::SetBCId_POT(uint32_t BCId)
{
  m_BCId_POT=BCId;
}

void ALFA_RawDataCollection_charge::PushBack_POT(const ALFA_RawData_charge& PMF_HIT)
{
  m_POT_DATA.push_back(PMF_HIT);
}  


void ALFA_RawDataCollection_charge::SetZero_POT()
{

  m_MBId_POT = 0;    // MB number
   
  m_MrodId_POT = 0;    // Mrodnumber

  m_EventCount_POT=0; 
  m_TimeStamp_POT=0; 
  m_BCId_POT=0;

  m_POT_DATA.clear();
}

