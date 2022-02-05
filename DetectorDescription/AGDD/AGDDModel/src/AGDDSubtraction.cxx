/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDSubtraction.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDSubtraction::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateSubtraction(this);
}


void AGDDSubtraction::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
