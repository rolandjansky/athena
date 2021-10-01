/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDMaterialStore.h"

#include <iostream>

std::ostream& operator <<(std::ostream& os, const AGDDElement &obj)
{
	os<<"- Element "<<obj.m_name<<" symbol="<<obj.m_symbol<<
		" Z="<<obj.m_z<<" A="<<obj.m_a<<std::endl;
	return os;
}

void AGDDElement::RegisterToStore(AGDDMaterialStore& ms,
                                  AGDDElement *e)
{
        ms.RegisterElement(e);
}

std::string AGDDElement::GetName() const
{
	return m_name;
}

std::string AGDDElement::GetSymbol() const
{
	return m_symbol;
}
