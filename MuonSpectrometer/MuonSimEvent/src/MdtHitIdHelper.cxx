/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimEvent/MdtHitIdHelper.h"
#include <iostream>

MdtHitIdHelper* MdtHitIdHelper::m_help = 0;

//private constructor
MdtHitIdHelper::MdtHitIdHelper() : HitIdHelper(){
	InitializeStationName();
  	Initialize();
}

MdtHitIdHelper::MdtHitIdHelper(const unsigned int nTubes) : HitIdHelper()
{
  InitializeStationName();
  Initialize(nTubes);
}

MdtHitIdHelper* MdtHitIdHelper::GetHelper(){
	if (m_help==0) m_help = new MdtHitIdHelper();
	return m_help;
}

static char v1[] = {'B','E','T','C'};
static char v2[] = {'I','M','O','E','1','2','3','4','S'};
static char v3[] = {'S','L','E','R','F','G','M'};

	
MdtHitIdHelper* MdtHitIdHelper::GetHelper(const unsigned int nTubes) {
  if (!m_help) m_help = new MdtHitIdHelper(nTubes);
  return m_help;
}
	
void MdtHitIdHelper::Initialize(const unsigned int nTubes) {
 
 	InitializeField("PhiSector",1,8);
 	InitializeField("ZSector",-8,8);
 	InitializeField("MultiLayer",1,2);
 	InitializeField("Layer",1,4);
 	InitializeField("Tube",1,nTubes);
	
}
void MdtHitIdHelper::InitializeStationName()
{
 	InitializeField("Station[1]",0,sizeof(v1));
 	InitializeField("Station[2]",0,sizeof(v2));
 	InitializeField("Station[3]",0,sizeof(v3));
}
void MdtHitIdHelper::SetStationName(std::string name)
{   
	for (unsigned int i=0;i<sizeof(v1);i++)
	    if (v1[i]==name[0]) SetFieldValue("Station[1]",i);
	for (unsigned int i=0;i<sizeof(v2);i++)
	    if (v2[i]==name[1]) SetFieldValue("Station[2]",i);
	for (unsigned int i=0;i<sizeof(v3);i++)
	    if (v3[i]==name[2]) SetFieldValue("Station[3]",i);
	    
				
}
std::string MdtHitIdHelper::GetStationName(const int& hid)
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

int MdtHitIdHelper::GetPhiSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("PhiSector");
}

int MdtHitIdHelper::GetZSector(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("ZSector");
}

//----MDT
int MdtHitIdHelper::GetMultiLayer(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("MultiLayer");
}
int MdtHitIdHelper::GetLayer(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("Layer");
}
int MdtHitIdHelper::GetTube(const int& hid){
	this->SetID(hid);
	return this->GetFieldValue("Tube");
}

//packing method
int MdtHitIdHelper::BuildMdtHitId(const std::string statName, const int phiSect,
	const int zSect, const int multiLayer, const int layer, const int tube){
	
	this->SetID(0);
	this->SetStationName(statName);
	this->SetFieldValue("PhiSector", phiSect);
	this->SetFieldValue("ZSector", zSect);
	this->SetFieldValue("MultiLayer", multiLayer);
	this->SetFieldValue("Layer", layer);
	this->SetFieldValue("Tube", tube);
	return this->GetID();
}

