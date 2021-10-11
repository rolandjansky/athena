/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDSection.h"
#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDVolume.h"

#include <iostream>

AGDDSection::AGDDSection(const std::string& a,
                         const std::string& b,
                         const std::string& c,
                         const std::string& d,
                         const std::string& e,
                         AGDDSectionStore& ss,
                         bool bflag):
			m_name(a),m_version(b),m_author(c),m_date(d),m_topVolume(e),m_toBeBuilt(bflag)
{
	Register(ss);
}

void AGDDSection::AddVolume(AGDDVolume* v)
{
	std::string temp=v->GetName();
	if (m_theVolumes.find(temp)!=m_theVolumes.end())
		std::cout<<" Volume "<<temp<<" already in section "<<m_name<<std::endl;
	else
		m_theVolumes[temp]=v;
}

AGDDVolume* AGDDSection::GetVolume(const std::string& n)
{
	if (m_theVolumes.find(n)!=m_theVolumes.end())
		return m_theVolumes[n];
	else
	{
		std::cout<<" Volume "<<n<<" not found in section "<<m_name<<std::endl;
		return 0;
	}
}

void AGDDSection::Register(AGDDSectionStore& ss)
{
	ss.RegisterSection(this);
}
void AGDDSection::Print()
{
	std::cout<<"+";
	for (unsigned int i=0;i<10;i++) std::cout<<"--------";
	std::cout<<"+"<<std::endl;
	std::cout<<"  section "<<m_name<<" "<<" version "<<m_version<<std::endl;
	std::cout<<"  author  "<<m_author<<" date "<<m_date<<std::endl;
	std::cout<<"  defines volumes: "<<std::endl<<std::endl;
	volumeList::const_iterator it;
//	for (it=theVolumes.begin();it!=theVolumes.end();it++)
//	{
//		if (!(*it).second->HasParent())
//			AGDDController::PrintVolumeHierarchy((*it).first,0);
//	}
	std::cout<<"+";
	for (unsigned int i=0;i<10;i++) std::cout<<"--------";
	std::cout<<"+"<<std::endl<<std::endl;
}
