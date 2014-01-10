/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/TgcHitIdHelper.h"

TgcHitIdHelper* TgcHitIdHelper::m_help = 0;

//private constructor
TgcHitIdHelper::TgcHitIdHelper() : HitIdHelper(){
	InitializeStationName();
  	Initialize();
}

TgcHitIdHelper* TgcHitIdHelper::GetHelper(){
	if (m_help==0) m_help = new TgcHitIdHelper();
	return m_help;
}

static char v1[] = {'B','E','T','C'};
static char v2[] = {'I','M','O','E','1','2','3','4','S'};
static char v3[] = {'S','L','E','R','F','G'};

	
void TgcHitIdHelper::Initialize(){
 
 	InitializeField("StationPhi",1,48);
 	InitializeField("StationEta",-5,5);
	InitializeField("GasGap",1,3);
	
}
void TgcHitIdHelper::InitializeStationName()
{
 	InitializeField("Station[1]",0,sizeof(v1));
 	InitializeField("Station[2]",0,sizeof(v2));
 	InitializeField("Station[3]",0,sizeof(v3));
}
void TgcHitIdHelper::SetStationName(std::string name)
{
	for (unsigned int i=0;i<sizeof(v1);i++)
		if (v1[i]==name[0]) SetFieldValue("Station[1]",i);
	for (unsigned int i=0;i<sizeof(v2);i++)
		if (v2[i]==name[1]) SetFieldValue("Station[2]",i);
	for (unsigned int i=0;i<sizeof(v3);i++)

	if (v3[i]==name[2]) SetFieldValue("Station[3]",i);
}
std::string TgcHitIdHelper::GetStationName(const int& hid)
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

int TgcHitIdHelper::GetStationPhi(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("StationPhi");
}

int TgcHitIdHelper::GetStationEta(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("StationEta");
}

int TgcHitIdHelper::GetGasGap(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("GasGap");
}


//packing method
int TgcHitIdHelper::BuildTgcHitId(const std::string statName, const int statPhi,
	const int statEta, const int gasG){
	
	this->SetID(0);
	this->SetStationName(statName);
	this->SetFieldValue("StationPhi", statPhi);
	this->SetFieldValue("StationEta", statEta);
	this->SetFieldValue("GasGap", gasG);
	return this->GetID();
}

