/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDSection.h"

#include <iostream>

AGDDSectionStore::AGDDSectionStore():currentSection(0)
{
}
AGDDSectionStore* AGDDSectionStore::GetSectionStore()
{
	static AGDDSectionStore* theStore=new AGDDSectionStore;
	return theStore;
}
AGDDSection* AGDDSectionStore::GetSection(std::string n)
{
	if (theSections.find(n) != theSections.end())
		return theSections[n];
	else 
	{
		std::cout<<" Section "<<n<<" not found in store!"<<std::endl;
		return 0;
	}
}
void AGDDSectionStore::RegisterSection(AGDDSection *s)
{
	std::string n=s->Name();
	if (theSections.find(n) != theSections.end())
		std::cout<<" Section "<<n<<" already in store "<<std::endl;
	else
		theSections[n]=s;
	currentSection=s;
}
void AGDDSectionStore::PrintAllSections()
{
	sectionList::const_iterator it;
	for (it=theSections.begin();it!=theSections.end();it++)
	{
		(*it).second->Print();
	}
}

void AGDDSectionStore::Clean()
{
	sectionList::const_iterator it;
	for (it=theSections.begin();it!=theSections.end();it++)
	{
		delete (*it).second;
	}
	theSections.clear();
}

