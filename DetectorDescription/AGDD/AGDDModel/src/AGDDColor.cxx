/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDColor.h"
#include "AGDDModel/AGDDColorStore.h"

void AGDDColor::RegisterToStore() 
{
	AGDDColorStore::GetColorStore()->Register(this);
}
