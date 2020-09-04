/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/CscHitIdHelper.h"

#include <iomanip> // for std::array

CscHitIdHelper* CscHitIdHelper::m_help = nullptr;

namespace {
    const static std::array<char, 4> v1 = {'B','E','T','C'};
    const static std::array<char, 9> v2 = {'I','M','O','E','1','2','3','4','S'};
    const static std::array<char, 6> v3 = {'S','L','E','R','F','G'};
}

//private constructor
CscHitIdHelper::CscHitIdHelper() : HitIdHelper()
{
  InitializeStationName();
  Initialize();
}

CscHitIdHelper* CscHitIdHelper::GetHelper()
{
  if (!m_help) m_help = new CscHitIdHelper();
  return m_help;
}

void CscHitIdHelper::Initialize()
{
  InitializeField("PhiSector",1,8);
  InitializeField("ZSector",-1,1);
  InitializeField("ChamberLayer",1,2);
  InitializeField("WireLayer",1,4);

}

void CscHitIdHelper::InitializeStationName()
{
  InitializeField("Station[1]",0,sizeof(v1));
  InitializeField("Station[2]",0,sizeof(v2));
  InitializeField("Station[3]",0,sizeof(v3));
}

void CscHitIdHelper::SetStationName(std::string name, int &hid) const
{
  for (unsigned int i=0;i<sizeof(v1);i++)
    if (v1[i]==name[0]) SetFieldValue("Station[1]",i,hid);
  for (unsigned int i=0;i<sizeof(v2);i++)
    if (v2[i]==name[1]) SetFieldValue("Station[2]",i,hid);
  for (unsigned int i=0;i<sizeof(v3);i++)

    if (v3[i]==name[2]) SetFieldValue("Station[3]",i,hid);
}

std::string CscHitIdHelper::GetStationName(const int& hid) const
{
  char v[4];
  v[0]=v1[this->GetFieldValue("Station[1]",hid)];
  v[1]=v2[this->GetFieldValue("Station[2]",hid)];
  v[2]=v3[this->GetFieldValue("Station[3]",hid)];
  v[3]='\0';
  std::string temp=v;
  return temp;
}

int CscHitIdHelper::GetPhiSector(const int& hid) const
{
  return this->GetFieldValue("PhiSector",hid);
}

int CscHitIdHelper::GetZSector(const int& hid) const
{
  return this->GetFieldValue("ZSector",hid);
}

//----CSC
int CscHitIdHelper::GetChamberLayer(const int& hid) const
{
  return this->GetFieldValue("ChamberLayer",hid);
}

int CscHitIdHelper::GetWireLayer(const int& hid) const
{
  return this->GetFieldValue("WireLayer",hid);
}

//packing method
int CscHitIdHelper::BuildCscHitId(const std::string statName, const int phiSect,
                                  const int zSect, const int chamberLayer, int wireLayer) const
{
  int theID(0);
  this->SetStationName(statName, theID);
  this->SetFieldValue("PhiSector", phiSect, theID);
  this->SetFieldValue("ZSector", zSect, theID);
  this->SetFieldValue("ChamberLayer", chamberLayer, theID);
  this->SetFieldValue("WireLayer", wireLayer, theID);
  return theID;
}
