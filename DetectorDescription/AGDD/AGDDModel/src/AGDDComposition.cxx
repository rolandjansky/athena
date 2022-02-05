/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDComposition.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDComposition::CreateVolume (AGDDBuilder& builder)
{
	builder.CreateComposition(this);
}
