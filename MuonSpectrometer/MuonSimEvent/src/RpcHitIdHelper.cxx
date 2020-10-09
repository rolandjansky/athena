/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/RpcHitIdHelper.h"

#include <iomanip> // for std::array

RpcHitIdHelper* RpcHitIdHelper::m_help = nullptr;

namespace {
    const static std::array<char, 4> v1 = {'B','E','T','C'};
    const static std::array<char, 9> v2 = {'I','M','O','E','1','2','3','4','S'};
    const static std::array<char, 6> v3 = {'S','L','E','R','F','G'};
}

//private constructor
RpcHitIdHelper::RpcHitIdHelper() : HitIdHelper()
{
  InitializeStationName();
  Initialize();
}

RpcHitIdHelper::RpcHitIdHelper(unsigned int nGasGaps) : HitIdHelper()
{
  InitializeStationName();
  Initialize(nGasGaps);
}

RpcHitIdHelper* RpcHitIdHelper::GetHelper()
{
  if (!m_help) m_help = new RpcHitIdHelper();
  return m_help;
}

RpcHitIdHelper* RpcHitIdHelper::GetHelper(unsigned int nGasGaps)
{
  if (!m_help) m_help = new RpcHitIdHelper(nGasGaps);
  return m_help;
}

void RpcHitIdHelper::Initialize(unsigned int nGasGaps)
{
  InitializeField("PhiSector",1,8);
  InitializeField("ZSector",-8,8);
  InitializeField("DoubletR",1,2);
  InitializeField("GasGapLayer",1,nGasGaps);
  InitializeField("DoubletPhi",1,2);
  InitializeField("DoubletZ",1,3);
  InitializeField("MeasuresPhi",0,1);

}

void RpcHitIdHelper::InitializeStationName()
{
  InitializeField("Station[1]",0,sizeof(v1));
  InitializeField("Station[2]",0,sizeof(v2));
  InitializeField("Station[3]",0,sizeof(v3));
}

void RpcHitIdHelper::SetStationName(std::string name, int& hid) const
{
  for (unsigned int i=0;i<sizeof(v1);i++)
    if (v1[i]==name[0]) SetFieldValue("Station[1]",i,hid);
  for (unsigned int i=0;i<sizeof(v2);i++)
    if (v2[i]==name[1]) SetFieldValue("Station[2]",i,hid);
  for (unsigned int i=0;i<sizeof(v3);i++)
    if (v3[i]==name[2]) SetFieldValue("Station[3]",i,hid);
}

std::string RpcHitIdHelper::GetStationName(const int& hid) const
{
  char v[4];
  v[0]=v1[this->GetFieldValue("Station[1]", hid)];
  v[1]=v2[this->GetFieldValue("Station[2]", hid)];
  v[2]=v3[this->GetFieldValue("Station[3]", hid)];
  v[3]='\0';
  std::string temp=v;
  return temp;
}

int RpcHitIdHelper::GetPhiSector(const int& hid) const
{
  return this->GetFieldValue("PhiSector", hid);
}

int RpcHitIdHelper::GetZSector(const int& hid) const
{
  return this->GetFieldValue("ZSector", hid);
}

//----RPC
int RpcHitIdHelper::GetDoubletR(const int& hid) const
{
  return this->GetFieldValue("DoubletR", hid);
}
int RpcHitIdHelper::GetGasGapLayer(const int& hid) const
{
  return this->GetFieldValue("GasGapLayer", hid);
}
int RpcHitIdHelper::GetDoubletPhi(const int& hid) const
{
  return this->GetFieldValue("DoubletPhi", hid);
}
int RpcHitIdHelper::GetDoubletZ(const int& hid) const
{
  return this->GetFieldValue("DoubletZ", hid);
}
int RpcHitIdHelper::GetMeasuresPhi(const int& hid) const
{
  return this->GetFieldValue("MeasuresPhi", hid);
}

//packing method
int RpcHitIdHelper::BuildRpcHitId(const std::string statName, const int phiSect,
                                  const int zSect, const int doubZ, const int doubR, const int gasGap,
                                  const int doubPhi, const int measPhi) const
{

  int theID(0);
  this->SetStationName(statName, theID);
  this->SetFieldValue("PhiSector", phiSect, theID);
  this->SetFieldValue("ZSector", zSect, theID);
  this->SetFieldValue("DoubletZ", doubZ, theID);
  this->SetFieldValue("DoubletR", doubR, theID);
  this->SetFieldValue("GasGapLayer", gasGap, theID);
  this->SetFieldValue("DoubletPhi", doubPhi, theID);
  this->SetFieldValue("MeasuresPhi", measPhi, theID);
  return theID;
}
