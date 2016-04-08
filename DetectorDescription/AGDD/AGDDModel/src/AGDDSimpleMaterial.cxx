/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDSimpleMaterial &obj)
{
	os<<"- Material "<<obj.m_name<<" type="<<obj.m_mType<<" density="<<obj.m_density<<std::endl;
	return os;
}


void AGDDSimpleMaterial::RegisterToStore(AGDDSimpleMaterial *e)
{
	AGDDMaterialStore::GetMaterialStore()->RegisterMaterial(e);
}

std::string AGDDSimpleMaterial::GetName()
{
	return m_name;
}
