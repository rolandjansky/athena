/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_RawEv/AFP_RawDataCollection.h"

AFP_RawDataCollection::AFP_RawDataCollection():
DataVector<AFP_RawData>(),
 m_L1Id(0),
 m_link_number_POT(0),
 m_frontend_flag(0),
 m_POT_DATA(),
 m_patternId(),
 m_BCIdId(0),
 m_header_numberId(0),
 m_ADC2Id(0),
  m_TrigSyncErrId(0),
  m_robID(0)
{ }




AFP_RawDataCollection::AFP_RawDataCollection(uint16_t MB_number) :
  DataVector<AFP_RawData>(),
  m_L1Id(MB_number),
  m_link_number_POT(0),
  m_frontend_flag(0),
  m_POT_DATA(),
  m_patternId(),
  m_BCIdId(0),
  m_header_numberId(0),
  m_ADC2Id(0),
  m_TrigSyncErrId(0),
    m_robID(0)
{ }


AFP_RawDataCollection::AFP_RawDataCollection( uint16_t Mrod_number, uint16_t MB_number) :
  DataVector<AFP_RawData>(),
  m_L1Id(MB_number),
  m_link_number_POT(Mrod_number),
  m_frontend_flag(0),
  m_POT_DATA(),
  m_patternId(),
   m_BCIdId(0),
  m_header_numberId(0),
  m_ADC2Id(0),
  m_TrigSyncErrId(0),
    m_robID(0)
{ }



void AFP_RawDataCollection::Set_lvl1Id(uint16_t MB_number)
{
  m_L1Id=MB_number;
} 
 
 
void AFP_RawDataCollection::Set_link_header(uint16_t Mrod_number)
{
  m_link_number_POT=Mrod_number;
} 


void AFP_RawDataCollection::Set_flag(uint32_t Event_number)
{
  m_frontend_flag=Event_number;
}

void AFP_RawDataCollection::Set_pattern_POT(const std::vector<bool>& pattern_number)
{
  m_patternId=pattern_number;
}

void AFP_RawDataCollection::Set_bcid(uint16_t BCId_number)
{
  m_BCIdId=BCId_number;
}

void AFP_RawDataCollection::Set_header(uint16_t header_number_number)
{
  m_header_numberId=header_number_number;
}

void AFP_RawDataCollection::Set_ADC2_POT(uint16_t ADC2_number)
{
  m_ADC2Id=ADC2_number;
}

void AFP_RawDataCollection::SetTrigSyncErr(bool bit)
{
  m_TrigSyncErrId=bit;
}

void AFP_RawDataCollection::PushBack_POT(const AFP_RawData& PMF_HIT)
{
  m_POT_DATA.push_back(PMF_HIT);
} 

void AFP_RawDataCollection::SetZero_POT()
{

  m_L1Id = 0;    // MB number
	   
  m_link_number_POT = 0;    // Mrodnumber

  m_frontend_flag=0;
	
  m_patternId.clear();
  m_BCIdId=0;
  m_header_numberId=0;
  m_ADC2Id=0;
  m_TrigSyncErrId=false;

  m_POT_DATA.clear();
}








