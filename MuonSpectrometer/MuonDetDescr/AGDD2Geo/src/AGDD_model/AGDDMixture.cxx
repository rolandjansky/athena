/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDMixture.h"
#include "AGDD2Geo/AGDDElement.h"
#include "AGDD2Geo/AGDDSimpleMaterial.h"
#include "AGDD2Geo/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDMixture &obj)
{
	os<<"- Material "<<obj._name<<" type=Mixture density="<<obj._density<<std::endl;
	os<<"\tcomponents"<<std::endl;
	for (unsigned int i=0;i<obj.theMaterials.size();i++)
	{
		const std::string ss=obj.theMaterials[i]->GetName();
		const double ii=obj.theComposition[i];
		os<<"\t\t material "<<ss<<" fraction="<<ii<<std::endl;
	}
	return os;
}

AGDDMixture::AGDDMixture(std::string n, double d):
	AGDDSimpleMaterial(n,d)
{
	mType=Mixture;
}

void AGDDMixture::AddMaterial(std::string el)
{
	AGDDMaterialStore* ms=AGDDMaterialStore::GetMaterialStore();
	AGDDSimpleMaterial* material=ms->GetMaterial(el);
	theMaterials.push_back(material);
}
void AGDDMixture::Fraction(double d)
{
	theComposition.push_back(d);
}
