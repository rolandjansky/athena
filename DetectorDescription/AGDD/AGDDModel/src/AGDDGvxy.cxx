/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDGvxy.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDGvxy::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateGvxy(this);
}


void AGDDGvxy::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
