/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDVolume.h"
#include "AGDDKernel/AGDDDetector.h"
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

void AGDDVolumeStore::Clean()
{
	for (auto volumeIt : *this)
	{
		if (!(volumeIt.second->IsSensitiveVolume()))
			delete (volumeIt.second);
	}
	this->clear();
}
