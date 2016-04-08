/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldManagerFactory.h"
#include "FadsField/FieldManagerFactoryStore.h"

namespace FADS {

FieldManagerFactoryStore::FieldManagerFactoryStore()
{
}

FieldManagerFactoryStore* FieldManagerFactoryStore::GetStore()
{
	static  FieldManagerFactoryStore* theStore=0;
	if (!theStore)
		theStore=new FieldManagerFactoryStore;
	return theStore;
}

void FieldManagerFactoryStore::AddFieldManagerFactory(FieldManagerFactory *f)
{
	theFactories[f->GetName()] = f;
}

FieldManagerFactory *FieldManagerFactoryStore::GetFieldManagerFactory(std::string n)
{
	if (theFactories.find(n)!=theFactories.end()) return theFactories[n];
	else return 0;
}
}
