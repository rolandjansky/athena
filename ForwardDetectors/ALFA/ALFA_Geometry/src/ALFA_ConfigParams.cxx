/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DefaultParams.cpp: implementation of the ALFA_ConfigParams class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ALFA_Geometry/ALFA_ConfigParams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ALFA_ConfigParams::ALFA_ConfigParams()
{
	m_bIsValid=false;
	m_strSection="invalid";
}

ALFA_ConfigParams::~ALFA_ConfigParams()
{
	m_bIsValid=false;
	m_mapParams.clear();
}

void ALFA_ConfigParams::UnInitialize()
{
	m_bIsValid=false;

	m_strSection="invalid";
	m_mapParams.clear();
}

int ALFA_ConfigParams::Init(const char *szFile, const char *szSection)
{
	bool bRes=false;
	FILE* pfile=NULL;
	char szbuff[256],szsec[256];
	char* ppv;

	UnInitialize();

	if((pfile=fopen(szFile,"r"))==NULL) return false;
	
	//read section
	memset(szsec,0,sizeof(szsec));
	if(szSection) strncpy(szsec,szSection,sizeof(szsec)-1);
	//strlwr(szsec);
	m_strSection=szsec;

	while(!feof(pfile)){
		fgets(szbuff,sizeof(szbuff),pfile);
		if(*(szbuff+strlen(szbuff)-1)=='\n') *(szbuff+strlen(szbuff)-1)=0;
		if(strcmp(szbuff,szsec)) continue; //if(strcmp(strlwr(szbuff),szsec)) continue;
		else{ bRes=true; break;	}
	}
	
	if(bRes==false){
		fclose(pfile);
		return 0;
	}

	while(!feof(pfile)){
		fgets(szbuff,sizeof(szbuff),pfile);
		if(*szbuff==0 || *szbuff==' ' || *szbuff=='\n' || *szbuff==';') continue;
		if(*szbuff=='[') break;
		if((ppv=strchr(szbuff,'='))==NULL) continue;

		if(*(szbuff+strlen(szbuff)-1)=='\n') *(szbuff+strlen(szbuff)-1)=0;
		*ppv=0; ppv++;
		m_mapParams.insert(MAPSTR2STR::value_type(szbuff,ppv)); //m_mapParams.insert(MAPSTR2STR::value_type(strlwr(szbuff),ppv));
	}

	fclose(pfile);
	if(m_mapParams.size()) m_bIsValid=true;

	return m_mapParams.size();
}

const char* ALFA_ConfigParams::GetParameter(const char *szKey) const
{
	if(!m_bIsValid) return NULL;
	
	char szbuff[256];
	memset(szbuff,0,sizeof(szbuff));
	if(szKey) strncpy(szbuff,szKey,sizeof(szbuff)-1);
	//strlwr(szbuff);
	
	MAPSTR2STR::const_iterator iter;
	if((iter=m_mapParams.find(szbuff))!=m_mapParams.end()){
		return (*iter).second.c_str();
	}
	else return NULL;
}

bool ALFA_ConfigParams::IsKey(const char *szKey) const
{
	if(!m_bIsValid) return false;

	char szbuff[256];
	memset(szbuff,0,sizeof(szbuff));
	if(szKey) strncpy(szbuff,szKey,sizeof(szbuff)-1);
	//strlwr(szbuff);
	
	MAPSTR2STR::const_iterator iter;
	if((iter=m_mapParams.find(szbuff))!=m_mapParams.end()){
		return true;
	}
	else return false;
}

