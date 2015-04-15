/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
//#include "GaudiKernel/MsgStream.h"

// Default constructor
ALFA_RawDataCollection_charge::ALFA_RawDataCollection_charge() :
  DataVector<ALFA_RawData_charge>(),
  MBId_POT(0),
  MrodId_POT(0),
  EventCount_POT(0),
  POT_DATA(),
  TimeStamp_POT(0),
  BCId_POT(0)
{ }

// Full constructor
ALFA_RawDataCollection_charge::ALFA_RawDataCollection_charge(uint16_t MB_number) :
  DataVector<ALFA_RawData_charge>(),
  MBId_POT(MB_number),
  MrodId_POT(0),
  EventCount_POT(0),
  POT_DATA(),
  TimeStamp_POT(0),
  BCId_POT(0)
{ }

// Full constructor
ALFA_RawDataCollection_charge::ALFA_RawDataCollection_charge( uint16_t Mrod_number, uint16_t MB_number) :
  DataVector<ALFA_RawData_charge>(),
  MBId_POT(MB_number),
  MrodId_POT(Mrod_number),
  EventCount_POT(0),
  POT_DATA(),
  TimeStamp_POT(0),
  BCId_POT(0)
{ }



void ALFA_RawDataCollection_charge::SetMBId_POT(uint16_t MB_number)
{
  MBId_POT=MB_number;
}  
  
  
void ALFA_RawDataCollection_charge::SetMrodId_POT(uint16_t Mrod_number)
{
  MrodId_POT=Mrod_number;
  }  


void ALFA_RawDataCollection_charge::SetEventCount_POT(uint32_t Event_number)
{
  EventCount_POT=Event_number;
}

void ALFA_RawDataCollection_charge::SetTimeStamp_POT(uint32_t TimeStamp)
{
  TimeStamp_POT=TimeStamp;
}

void ALFA_RawDataCollection_charge::SetBCId_POT(uint32_t BCId)
{
  BCId_POT=BCId;
}

void ALFA_RawDataCollection_charge::PushBack_POT(ALFA_RawData_charge PMF_HIT)
{
  POT_DATA.push_back(PMF_HIT);
}  


void ALFA_RawDataCollection_charge::SetZero_POT()
{

  MBId_POT = 0;    // MB number
   
  MrodId_POT = 0;    // Mrodnumber

  EventCount_POT=0; 
  TimeStamp_POT=0; 
  BCId_POT=0;

  POT_DATA.clear();
}

