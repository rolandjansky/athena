/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDSimpleMaterial.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDSimpleMaterial &obj)
{
	os<<"- Material "<<obj.m_name<<" type="<<obj.m_mType<<" density="<<obj.m_density<<std::endl;
	return os;
}


void AGDDSimpleMaterial::RegisterToStore(AGDDMaterialStore& ms,
                                         AGDDSimpleMaterial *e)
{
	ms.RegisterMaterial(e);
}

std::string AGDDSimpleMaterial::GetName() const
{
	return m_name;
}
