/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDVolumeStore.h"
#include "AGDD2Geo/AGDDVolume.h"
#include <iostream>

AGDDVolumeStore::AGDDVolumeStore()
{
}

void AGDDVolumeStore::RegisterVolume(AGDDVolume *v)
{
	std::string name=v->GetName();
	(*this)[name]=v;
}

bool AGDDVolumeStore::Exist(std::string n)
{
	return ((*this).find(n) != (*this).end());
}

AGDDVolume* AGDDVolumeStore::GetVolume(std::string name)
{
	if ((*this).find(name) != (*this).end())
		return (*this)[name];
	else
	{
		std::cout << " Volume "<<name<<" not found: returning 0"<<std::endl;
		return 0;
	}
}

AGDDVolumeStore* AGDDVolumeStore::GetVolumeStore()
{
	static AGDDVolumeStore* thePointer=new AGDDVolumeStore();
	return thePointer;
}
