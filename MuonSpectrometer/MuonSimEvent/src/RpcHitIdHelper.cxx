/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/RpcHitIdHelper.h"

RpcHitIdHelper* RpcHitIdHelper::m_help = 0;

//private constructor
RpcHitIdHelper::RpcHitIdHelper() : HitIdHelper(){
	InitializeStationName();
  	Initialize();
}

RpcHitIdHelper* RpcHitIdHelper::GetHelper(){
	if (m_help==0) m_help = new RpcHitIdHelper();
	return m_help;
}

static char v1[] = {'B','E','T','C'};
static char v2[] = {'I','M','O','E','1','2','3','4','S'};
static char v3[] = {'S','L','E','R','F','G'};

	
void RpcHitIdHelper::Initialize(){
 
 	InitializeField("PhiSector",1,8);
 	InitializeField("ZSector",-8,8);
 	InitializeField("DoubletR",1,2);
 	InitializeField("GasGapLayer",1,2);
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
void RpcHitIdHelper::SetStationName(std::string name)
{
	for (unsigned int i=0;i<sizeof(v1);i++)
		if (v1[i]==name[0]) SetFieldValue("Station[1]",i);
	for (unsigned int i=0;i<sizeof(v2);i++)
		if (v2[i]==name[1]) SetFieldValue("Station[2]",i);
	for (unsigned int i=0;i<sizeof(v3);i++)

	if (v3[i]==name[2]) SetFieldValue("Station[3]",i);
}
std::string RpcHitIdHelper::GetStationName(const int& hid)
{
	this->SetID(hid);
	char v[4];
	v[0]=v1[this->GetFieldValue("Station[1]")];
	v[1]=v2[this->GetFieldValue("Station[2]")];
	v[2]=v3[this->GetFieldValue("Station[3]")];
	v[3]='\0';
	std::string temp=v;
	return temp;
}

int RpcHitIdHelper::GetPhiSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("PhiSector");
}

int RpcHitIdHelper::GetZSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("ZSector");
}

//----RPC
int RpcHitIdHelper::GetDoubletR(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("DoubletR");
}
int RpcHitIdHelper::GetGasGapLayer(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("GasGapLayer");
}
int RpcHitIdHelper::GetDoubletPhi(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("DoubletPhi");
}
int RpcHitIdHelper::GetDoubletZ(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("DoubletZ");
}
int RpcHitIdHelper::GetMeasuresPhi(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("MeasuresPhi");
}

//packing method
int RpcHitIdHelper::BuildRpcHitId(const std::string statName, const int phiSect,
	const int zSect, const int doubZ, const int doubR, const int gasGap,
	const int doubPhi, const int measPhi){
	
	this->SetID(0);
	this->SetStationName(statName);
	this->SetFieldValue("PhiSector", phiSect);
	this->SetFieldValue("ZSector", zSect);
	this->SetFieldValue("DoubletZ", doubZ);
	this->SetFieldValue("DoubletR", doubR);
	this->SetFieldValue("GasGapLayer", gasGap);
	this->SetFieldValue("DoubletPhi", doubPhi);
	this->SetFieldValue("MeasuresPhi", measPhi);
	return this->GetID();
}
