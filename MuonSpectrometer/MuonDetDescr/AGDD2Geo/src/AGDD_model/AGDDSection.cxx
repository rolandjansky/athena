/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDSection.h"
#include "AGDD2Geo/AGDDSectionStore.h"
#include "AGDD2Geo/AGDDVolume.h"
#include "AGDD2Geo/AGDDController.h"

#include <iostream>

AGDDSection::AGDDSection(std::string a, std::string b, std::string c, std::string d, std::string e, bool bflag):
			name(a),version(b),author(c),date(d),topVolume(e),toBeBuilt(bflag)
{
	Register();
}

void AGDDSection::AddVolume(AGDDVolume* v)
{
	std::string temp=v->GetName();
	if (theVolumes.find(temp)!=theVolumes.end())
		std::cout<<" Volume "<<temp<<" already in section "<<name<<std::endl;
	else
		theVolumes[temp]=v;
}

AGDDVolume* AGDDSection::GetVolume(std::string n)
{
	if (theVolumes.find(n)!=theVolumes.end())
		return theVolumes[n];
	else
	{
		std::cout<<" Volume "<<n<<" not found in section "<<name<<std::endl;
		return 0;
	}
}

void AGDDSection::Register()
{
	AGDDSectionStore *ss=AGDDSectionStore::GetSectionStore();
	ss->RegisterSection(this);
}
void AGDDSection::Print()
{
	std::cout<<"+";
	for (unsigned int i=0;i<10;i++) std::cout<<"--------";
	std::cout<<"+"<<std::endl;
	std::cout<<"  section "<<name<<" "<<" version "<<version<<std::endl;
	std::cout<<"  author  "<<author<<" date "<<date<<std::endl;
	std::cout<<"  defines volumes: "<<std::endl<<std::endl;
	volumeList::const_iterator it;
	for (it=theVolumes.begin();it!=theVolumes.end();it++)
	{
		if (!(*it).second->HasParent())
			AGDDController::PrintVolumeHierarchy((*it).first,0);
	}
	std::cout<<"+";
	for (unsigned int i=0;i<10;i++) std::cout<<"--------";
	std::cout<<"+"<<std::endl<<std::endl;
}
