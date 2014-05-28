/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDElement.h"
#include "AGDD2Geo/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDElement &obj)
{
	os<<"- Element "<<obj._name<<" symbol="<<obj._symbol<<
		" Z="<<obj._z<<" A="<<obj._a<<std::endl;
	return os;
}

void AGDDElement::RegisterToStore(AGDDElement *e)
{
	AGDDMaterialStore::GetMaterialStore()->RegisterElement(e);
}

std::string AGDDElement::GetName() const
{
	return _name;
}

std::string AGDDElement::GetSymbol() const
{
	return _symbol;
}
