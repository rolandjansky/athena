/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDModel/AGDDColor.h"
#include "AGDDModel/AGDDColorStore.h"

void AGDDColor::RegisterToStore(AGDDColorStore& cs) 
{
	cs.Register(this);
}
