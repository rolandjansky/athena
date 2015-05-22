/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDMolecule.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDMolecule &obj)
{
	os<<"- Material "<<obj._name<<" type=Molecule density="<<obj._density<<std::endl;
	os<<"\tcomponents"<<std::endl;
	for (unsigned int i=0;i<obj.theElements.size();i++)
	{
		const std::string ss=obj.theElements[i]->GetName();
		const int ii=obj.theComposition[i];
		os<<"\t\t element "<<ss<<" natoms="<<ii<<std::endl;
	}
	return os;
}

AGDDMolecule::AGDDMolecule(std::string n, double d):
	AGDDSimpleMaterial(n,d)
{
	mType=Molecule;
}

void AGDDMolecule::AddElement(std::string el)
{
	AGDDMaterialStore* ms=AGDDMaterialStore::GetMaterialStore();
	AGDDElement* element=ms->GetElement(el);
	theElements.push_back(element);
}
void AGDDMolecule::NAtoms(int i)
{
	theComposition.push_back(i);
}
