/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDSection.h"

#include <iostream>

AGDDSectionStore::AGDDSectionStore():m_currentSection(0)
{
}
AGDDSection* AGDDSectionStore::GetSection(const std::string& n)
{
	if (m_theSections.find(n) != m_theSections.end())
		return m_theSections[n];
	else 
	{
		std::cout<<" Section "<<n<<" not found in store!"<<std::endl;
		return 0;
	}
}
void AGDDSectionStore::RegisterSection(AGDDSection *s)
{
	std::string n=s->Name();
	if (m_theSections.find(n) != m_theSections.end())
		std::cout<<" Section "<<n<<" already in store "<<std::endl;
	else
		m_theSections[n]=s;
	m_currentSection=s;
}
void AGDDSectionStore::PrintAllSections() const
{
	sectionList::const_iterator it;
	for (it=m_theSections.begin();it!=m_theSections.end();++it)
	{
		(*it).second->Print();
	}
}

void AGDDSectionStore::Clean()
{
	sectionList::const_iterator it;
	for (it=m_theSections.begin();it!=m_theSections.end();++it)
	{
		delete (*it).second;
	}
	m_theSections.clear();
}

