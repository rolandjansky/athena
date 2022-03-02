/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDUbeam.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDUbeam::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateUbeam(this);
}

void AGDDUbeam::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
