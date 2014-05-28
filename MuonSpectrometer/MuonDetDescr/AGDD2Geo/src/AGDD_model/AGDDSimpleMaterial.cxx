/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDSimpleMaterial.h"
#include "AGDD2Geo/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDSimpleMaterial &obj)
{
	os<<"- Material "<<obj._name<<" type="<<obj.mType<<" density="<<obj._density<<std::endl;
	return os;
}


void AGDDSimpleMaterial::RegisterToStore(AGDDSimpleMaterial *e)
{
	AGDDMaterialStore::GetMaterialStore()->RegisterMaterial(e);
}

std::string AGDDSimpleMaterial::GetName()
{
	return _name;
}
