/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2Geo/AGDDColor.h"
#include "AGDD2Geo/AGDDColorStore.h"

void AGDDColor::RegisterToStore() 
{
	AGDDColorStore::GetColorStore()->Register(this);
}
