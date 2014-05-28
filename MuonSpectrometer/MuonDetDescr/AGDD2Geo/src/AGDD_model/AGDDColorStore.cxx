/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDColorStore.h"
#include "AGDD2Geo/AGDDColor.h"
#include <iostream>

AGDDColorStore::AGDDColorStore()
{
}

void AGDDColorStore::Register(AGDDColor *v)
{
	std::string name=v->GetName();
	(*this)[name]=v;
}

bool AGDDColorStore::Exist(std::string n)
{
	return ((*this).find(n) != (*this).end());
}

AGDDColor* AGDDColorStore::GetColor(std::string name)
{
	if ((*this).find(name) != (*this).end())
		return (*this)[name];
	else
	{
		std::cout << " Color "<<name<<" not found: returning 0"<<std::endl;
		return 0;
	}
}

AGDDColorStore* AGDDColorStore::GetColorStore()
{
	static AGDDColorStore* thePointer=new AGDDColorStore();
	return thePointer;
}
