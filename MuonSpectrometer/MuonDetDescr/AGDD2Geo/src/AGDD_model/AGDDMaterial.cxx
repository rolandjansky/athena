/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDMaterial.h"
#include "AGDD2Geo/AGDDElement.h"
#include "AGDD2Geo/AGDDSimpleMaterial.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDMaterial &obj)
{
	os<<"- Material "<<obj._name<<" type-Material density="<<obj._density<<
		" Z="<<obj._z<<" A="<<obj._a<<std::endl;
	return os;
}

AGDDMaterial::AGDDMaterial(std::string n,AGDDElement* e, double d):
	AGDDSimpleMaterial(n,d),_z(e->GetZ()),_a(e->GetA()),_element(e)
{
	mType=Material;
}

AGDDMaterial::AGDDMaterial(std::string n,int z, double a,double d):
	AGDDSimpleMaterial(n,d),_z(z),_a(a),_element(0)
{
	mType=Material;
}
