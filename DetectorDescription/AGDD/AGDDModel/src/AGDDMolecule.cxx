/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDMolecule.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDMolecule &obj)
{
	os<<"- Material "<<obj.m_name<<" type=Molecule density="<<obj.m_density<<std::endl;
	os<<"\tcomponents"<<std::endl;
	for (unsigned int i=0;i<obj.m_theElements.size();i++)
	{
		const std::string ss=obj.m_theElements[i]->GetName();
		const int ii=obj.m_theComposition[i];
		os<<"\t\t element "<<ss<<" natoms="<<ii<<std::endl;
	}
	return os;
}

AGDDMolecule::AGDDMolecule(AGDDMaterialStore& ms, const std::string& n, double d):
	AGDDSimpleMaterial(ms,n,d)
{
        m_mType=Molecule;
}

void AGDDMolecule::AddElement(AGDDMaterialStore& ms, const std::string& el)
{
	AGDDElement* element=ms.GetElement(el);
	m_theElements.push_back(element);
}
void AGDDMolecule::NAtoms(int i)
{
	m_theComposition.push_back(i);
}
