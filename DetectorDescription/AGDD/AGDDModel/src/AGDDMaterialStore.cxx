/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

AGDDMaterialStore::AGDDMaterialStore():m_nrOfMaterials(0),m_nrOfElements(0)
{
	m_theMaterials.clear();
	m_theElements.clear();
}

void AGDDMaterialStore::RegisterElement(AGDDElement *el)
{
	if (m_theElements.find(el->GetName()) != m_theElements.end())
	{
		std::cout<<"Element "<<el->GetName()<<" already defined:skipping"<<std::endl;
	}
	else
	{
		m_nrOfElements++;
		m_theElements[el->GetName()]=el;
	}
}
void AGDDMaterialStore::RegisterMaterial(AGDDSimpleMaterial *mat)
{
	if (m_theMaterials.find(mat->GetName()) != m_theMaterials.end())
	{
		std::cout<<"Material "<<mat->GetName()<<" already defined:skipping"<<std::endl;
	}
	else
	{
		m_nrOfMaterials++;
		m_theMaterials[mat->GetName()]=mat;
	}
}
AGDDSimpleMaterial* AGDDMaterialStore::GetMaterial(std::string mat)
{
	if (m_theMaterials.find(mat) != m_theMaterials.end())
		return m_theMaterials[mat];
	else
	{
		std::cout<<" Material "<<mat<<" not found!"<<std::endl;
		return 0;
	}
}
AGDDElement* AGDDMaterialStore::GetElement(std::string el)
{
	if (m_theElements.find(el) != m_theElements.end())
		return m_theElements[el];
	else
	{
		std::cout<<" Element "<<el<<" not found!"<<std::endl;
		return 0;
	}
}

void AGDDMaterialStore::PrintElementNames()
{
	std::cout<<"List of elements so far defined: "<<
			std::endl<<"-----> ";
	AGDDElementMap::const_iterator it;
	int i=0;
	for (it=m_theElements.begin();it!=m_theElements.end();++it)
	{
		i++;
		if (!(i%5)) std::cout<<std::endl<<"-----> ";
		std::cout<<(*it).first<<",";
	}
}
	
void AGDDMaterialStore::PrintMaterialNames()
{
	std::cout<<"List of materials so far defined: "<<
			std::endl<<"-----> ";
	AGDDMaterialMap::const_iterator it;
	int i=0;
	for (it=m_theMaterials.begin();it!=m_theMaterials.end();++it)
	{
		i++;
		if (!(i%5)) std::cout<<std::endl<<"-----> ";
		std::cout<<(*it).first<<",";
	}
}

void AGDDMaterialStore::PrintElement(std::string n)
{
	if (m_theElements.find(n)!=m_theElements.end())
		std::cout<<*(m_theElements[n]);
	else
		std::cout<<"Element "<<n<<" not found!"<<std::endl;
}

void AGDDMaterialStore::PrintMaterial(std::string n)
{
	if (m_theMaterials.find(n)!=m_theMaterials.end())
		std::cout<<*(m_theMaterials[n]);
	else
		std::cout<<"Material "<<n<<" not found!"<<std::endl;
}
