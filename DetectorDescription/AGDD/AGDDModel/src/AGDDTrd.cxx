/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDTrd.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDTrd::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateTrd(this);
}

void AGDDTrd::CreateVolume (const AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
