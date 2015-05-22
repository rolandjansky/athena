/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

AGDDMaterialStore::AGDDMaterialStore():nrOfMaterials(0),nrOfElements(0)
{
	theMaterials.clear();
	theElements.clear();
}

AGDDMaterialStore* AGDDMaterialStore::GetMaterialStore()
{
	static AGDDMaterialStore* thePointer=new AGDDMaterialStore;
	return thePointer;
}
void AGDDMaterialStore::RegisterElement(AGDDElement *el)
{
	if (theElements.find(el->GetName()) != theElements.end())
	{
		std::cout<<"Element "<<el->GetName()<<" already defined:skipping"<<std::endl;
	}
	else
	{
		nrOfElements++;
		theElements[el->GetName()]=el;
	}
}
void AGDDMaterialStore::RegisterMaterial(AGDDSimpleMaterial *mat)
{
	if (theMaterials.find(mat->GetName()) != theMaterials.end())
	{
		std::cout<<"Material "<<mat->GetName()<<" already defined:skipping"<<std::endl;
	}
	else
	{
		nrOfMaterials++;
		theMaterials[mat->GetName()]=mat;
	}
}
AGDDSimpleMaterial* AGDDMaterialStore::GetMaterial(std::string mat)
{
	if (theMaterials.find(mat) != theMaterials.end())
		return theMaterials[mat];
	else
	{
		std::cout<<" Material "<<mat<<" not found!"<<std::endl;
		return 0;
	}
}
AGDDElement* AGDDMaterialStore::GetElement(std::string el)
{
	if (theElements.find(el) != theElements.end())
		return theElements[el];
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
	for (it=theElements.begin();it!=theElements.end();it++)
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
	for (it=theMaterials.begin();it!=theMaterials.end();it++)
	{
		i++;
		if (!(i%5)) std::cout<<std::endl<<"-----> ";
		std::cout<<(*it).first<<",";
	}
}

void AGDDMaterialStore::PrintElement(std::string n)
{
	if (theElements.find(n)!=theElements.end())
		std::cout<<*(theElements[n]);
	else
		std::cout<<"Element "<<n<<" not found!"<<std::endl;
}

void AGDDMaterialStore::PrintMaterial(std::string n)
{
	if (theMaterials.find(n)!=theMaterials.end())
		std::cout<<*(theMaterials[n]);
	else
		std::cout<<"Material "<<n<<" not found!"<<std::endl;
}
