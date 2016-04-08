/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDMaterial.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDSimpleMaterial.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDMaterial &obj)
{
	os<<"- Material "<<obj.m_name<<" type-Material density="<<obj.m_density<<
		" Z="<<obj.m_z<<" A="<<obj.m_a<<std::endl;
	return os;
}

AGDDMaterial::AGDDMaterial(std::string n,AGDDElement* e, double d):
	AGDDSimpleMaterial(n,d),m_z(e->GetZ()),m_a(e->GetA()),m_element(e),m_created(false)
{
	m_mType=Material;
}

AGDDMaterial::AGDDMaterial(std::string n,int z, double a,double d):
	AGDDSimpleMaterial(n,d),m_z(z),m_a(a),m_element(0),m_created(false)
{
	m_mType=Material;
}
