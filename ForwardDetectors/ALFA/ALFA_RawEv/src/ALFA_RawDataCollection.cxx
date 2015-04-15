/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawDataCollection.h"
//#include "GaudiKernel/MsgStream.h"

// Default constructor
ALFA_RawDataCollection::ALFA_RawDataCollection() :
  DataVector<ALFA_RawData>(),
  MBId_POT(0),
  MrodId_POT(0),
  EventCount_POT(0),
  POT_DATA(),
  patternId(),
  scalerId(0),
  ADC1Id(0),
  ADC2Id(0),
  TrigSyncErrId(0)
{ }

// Full constructor
ALFA_RawDataCollection::ALFA_RawDataCollection(uint16_t MB_number) :
  DataVector<ALFA_RawData>(),
  MBId_POT(MB_number),
  MrodId_POT(0),
  EventCount_POT(0),
  POT_DATA(),
  patternId(),
  scalerId(0),
  ADC1Id(0),
  ADC2Id(0),
  TrigSyncErrId(0)
{ }

// Full constructor
ALFA_RawDataCollection::ALFA_RawDataCollection( uint16_t Mrod_number, uint16_t MB_number) :
  DataVector<ALFA_RawData>(),
  MBId_POT(MB_number),
  MrodId_POT(Mrod_number),
  EventCount_POT(0),
  POT_DATA(),
  patternId(),
  scalerId(0),
  ADC1Id(0),
  ADC2Id(0),
  TrigSyncErrId(0)
{ }



void ALFA_RawDataCollection::SetMBId_POT(uint16_t MB_number)
{
  MBId_POT=MB_number;
}  
  
  
void ALFA_RawDataCollection::SetMrodId_POT(uint16_t Mrod_number)
{
  MrodId_POT=Mrod_number;
}  


void ALFA_RawDataCollection::SetEventCount_POT(uint32_t Event_number)
{
  EventCount_POT=Event_number;
}

void ALFA_RawDataCollection::Set_pattern_POT(std::vector<bool> pattern_number)
{
  patternId=pattern_number;
}

void ALFA_RawDataCollection::Set_scaler_POT(uint16_t scaler_number)
{
  scalerId=scaler_number;
}

void ALFA_RawDataCollection::Set_ADC1_POT(uint16_t ADC1_number)
{
  ADC1Id=ADC1_number;
}

void ALFA_RawDataCollection::Set_ADC2_POT(uint16_t ADC2_number)
{
  ADC2Id=ADC2_number;
}

void ALFA_RawDataCollection::SetTrigSyncErr(bool bit)
{
  TrigSyncErrId=bit;
}

void ALFA_RawDataCollection::PushBack_POT(ALFA_RawData PMF_HIT)
{
  POT_DATA.push_back(PMF_HIT);
}  


void ALFA_RawDataCollection::SetZero_POT()
{

  MBId_POT = 0;    // MB number
   
  MrodId_POT = 0;    // Mrodnumber

  EventCount_POT=0; 

  patternId.clear(); 
  scalerId=0; 
  ADC1Id=0; 
  ADC2Id=0;
  TrigSyncErrId=false;

  POT_DATA.clear();
}

