/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDMixture.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDMixture &obj)
{
	os<<"- Material "<<obj.m_name<<" type=Mixture density="<<obj.m_density<<std::endl;
	os<<"\tcomponents"<<std::endl;
	for (unsigned int i=0;i<obj.m_theMaterials.size();i++)
	{
		const std::string ss=obj.m_theMaterials[i]->GetName();
		const double ii=obj.m_theComposition[i];
		os<<"\t\t material "<<ss<<" fraction="<<ii<<std::endl;
	}
	return os;
}

AGDDMixture::AGDDMixture(AGDDMaterialStore& ms, const std::string& n, double d):
	AGDDSimpleMaterial(ms,n,d)
{
	m_mType=Mixture;
}

void AGDDMixture::AddMaterial(AGDDMaterialStore& ms, const std::string& el)
{
	AGDDSimpleMaterial* material=ms.GetMaterial(el);
	m_theMaterials.push_back(material);
}
void AGDDMixture::Fraction(double d)
{
	m_theComposition.push_back(d);
}
