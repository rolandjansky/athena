/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDSection.h"
#include "AGDDKernel/AGDDVolume.h"

AGDDVolume::AGDDVolume(std::string n,bool v):name(n),theSolid(0),theVolume(0),isSensitive(v)
{
	AGDDVolumeStore::GetVolumeStore()->RegisterVolume(this);
	AGDDSectionStore *ss=AGDDSectionStore::GetSectionStore();
	AGDDSection* curr=ss->GetCurrentSection();
	if (curr) curr->AddVolume(this);
}
void AGDDVolume::AddDaughter(AGDDPositioner* v)
{
	theDaughters.push_back(v);
	AGDDVolumeStore::GetVolumeStore()->GetVolume(v->Volume())->AddParent(this);
}

void AGDDVolume::IsSensitiveVolume(bool v)
{	
	isSensitive=v;
}
