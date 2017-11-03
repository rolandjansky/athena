/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldManagerStore.h" 
#include "FadsField/FadsFieldManager.h"

namespace FADS {

FieldManagerStore* FieldManagerStore::s_fieldManagerStore=0;

FieldManagerStore::FieldManagerStore()
{
}

FieldManagerStore::FieldManagerStore(const FieldManagerStore& )
{
}

FieldManagerStore* FieldManagerStore::GetFMStore()
{
	if (!s_fieldManagerStore) s_fieldManagerStore=new FieldManagerStore;
	return s_fieldManagerStore;
}

FieldManagerStore* FieldManagerStore::GetFieldManagerStore()
{
	return s_fieldManagerStore;
}

void FieldManagerStore::RegisterFieldManager(FadsFieldManager* fm)
{
	std::string name=fm->GetName();
	if (m_fieldManagerList.find(name) != m_fieldManagerList.end()) 
		std::cout <<"FieldManagerStore: FieldManager "<<name<<
				  " already registered"<<std::endl;
	else
		m_fieldManagerList[name]=fm;
}

FadsFieldManager* FieldManagerStore::GetFieldManager(std::string managerName)
{
	if (m_fieldManagerList.find(managerName) != m_fieldManagerList.end())
		return m_fieldManagerList[managerName];
	else
	{
		std::cout<<" field manager "<<managerName<<" not found!"<<std::endl;
		return 0;
	}	
}

}
