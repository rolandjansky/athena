/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawDataCollection.h"
//#include "GaudiKernel/MsgStream.h"

// Default constructor
ALFA_RawDataCollection::ALFA_RawDataCollection() :
  DataVector<ALFA_RawData>(),
  m_MBId_POT(0),
  m_MrodId_POT(0),
  m_EventCount_POT(0),
  m_POT_DATA(),
  m_patternId(),
  m_scalerId(0),
  m_ADC1Id(0),
  m_ADC2Id(0),
  m_TrigSyncErrId(0)
{ }

// Full constructor
ALFA_RawDataCollection::ALFA_RawDataCollection(uint16_t MB_number) :
  DataVector<ALFA_RawData>(),
  m_MBId_POT(MB_number),
  m_MrodId_POT(0),
  m_EventCount_POT(0),
  m_POT_DATA(),
  m_patternId(),
  m_scalerId(0),
  m_ADC1Id(0),
  m_ADC2Id(0),
  m_TrigSyncErrId(0)
{ }

// Full constructor
ALFA_RawDataCollection::ALFA_RawDataCollection( uint16_t Mrod_number, uint16_t MB_number) :
  DataVector<ALFA_RawData>(),
  m_MBId_POT(MB_number),
  m_MrodId_POT(Mrod_number),
  m_EventCount_POT(0),
  m_POT_DATA(),
  m_patternId(),
  m_scalerId(0),
  m_ADC1Id(0),
  m_ADC2Id(0),
  m_TrigSyncErrId(0)
{ }



void ALFA_RawDataCollection::SetMBId_POT(uint16_t MB_number)
{
  m_MBId_POT=MB_number;
}  
  
  
void ALFA_RawDataCollection::SetMrodId_POT(uint16_t Mrod_number)
{
  m_MrodId_POT=Mrod_number;
}  


void ALFA_RawDataCollection::SetEventCount_POT(uint32_t Event_number)
{
  m_EventCount_POT=Event_number;
}

void ALFA_RawDataCollection::Set_pattern_POT(const std::vector<bool>& pattern_number)
{
  m_patternId=pattern_number;
}

void ALFA_RawDataCollection::Set_scaler_POT(uint16_t scaler_number)
{
  m_scalerId=scaler_number;
}

void ALFA_RawDataCollection::Set_ADC1_POT(uint16_t ADC1_number)
{
  m_ADC1Id=ADC1_number;
}

void ALFA_RawDataCollection::Set_ADC2_POT(uint16_t ADC2_number)
{
  m_ADC2Id=ADC2_number;
}

void ALFA_RawDataCollection::SetTrigSyncErr(bool bit)
{
  m_TrigSyncErrId=bit;
}

void ALFA_RawDataCollection::PushBack_POT(const ALFA_RawData& PMF_HIT)
{
  m_POT_DATA.push_back(PMF_HIT);
}  


void ALFA_RawDataCollection::SetZero_POT()
{

  m_MBId_POT = 0;    // MB number
   
  m_MrodId_POT = 0;    // Mrodnumber

  m_EventCount_POT=0; 

  m_patternId.clear(); 
  m_scalerId=0; 
  m_ADC1Id=0; 
  m_ADC2Id=0;
  m_TrigSyncErrId=false;

  m_POT_DATA.clear();
}

