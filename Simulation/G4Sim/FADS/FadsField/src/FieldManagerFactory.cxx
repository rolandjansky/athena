/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldManagerFactory.h"
#include "FadsField/FieldManagerFactoryStore.h"

namespace FADS {

void FieldManagerFactory::RegisterToStore()
{
	FieldManagerFactoryStore *fStore=FieldManagerFactoryStore::GetStore();
	fStore->AddFieldManagerFactory(this);
}

}
