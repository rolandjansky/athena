/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/CscHitIdHelper.h"

CscHitIdHelper* CscHitIdHelper::m_help = 0;

//private constructor
CscHitIdHelper::CscHitIdHelper() : HitIdHelper(){
	InitializeStationName();
  	Initialize();
}

CscHitIdHelper* CscHitIdHelper::GetHelper(){
	if (m_help==0) m_help = new CscHitIdHelper();
	return m_help;
}

static char v1[] = {'B','E','T','C'};
static char v2[] = {'I','M','O','E','1','2','3','4','S'};
static char v3[] = {'S','L','E','R','F','G'};

	
void CscHitIdHelper::Initialize(){
 
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

void CscHitIdHelper::SetStationName(std::string name)
{
	for (unsigned int i=0;i<sizeof(v1);i++)
		if (v1[i]==name[0]) SetFieldValue("Station[1]",i);
	for (unsigned int i=0;i<sizeof(v2);i++)
		if (v2[i]==name[1]) SetFieldValue("Station[2]",i);
	for (unsigned int i=0;i<sizeof(v3);i++)

	if (v3[i]==name[2]) SetFieldValue("Station[3]",i);
}

std::string CscHitIdHelper::GetStationName(const int& hid)
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

int CscHitIdHelper::GetPhiSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("PhiSector");
}

int CscHitIdHelper::GetZSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("ZSector");
}

//----CSC
int CscHitIdHelper::GetChamberLayer(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("ChamberLayer");
}

int CscHitIdHelper::GetWireLayer(const int& hid) {
	this->SetID(hid);
	return this->GetFieldValue("WireLayer");
}

//packing method
int CscHitIdHelper::BuildCscHitId(const std::string statName, const int phiSect,
	const int zSect, const int chamberLayer, int wireLayer){
	
	this->SetID(0);
	this->SetStationName(statName);
	this->SetFieldValue("PhiSector", phiSect);
	this->SetFieldValue("ZSector", zSect);
	this->SetFieldValue("ChamberLayer", chamberLayer);
	this->SetFieldValue("WireLayer", wireLayer);
	return this->GetID();
}

