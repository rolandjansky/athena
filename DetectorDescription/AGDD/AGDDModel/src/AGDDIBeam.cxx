/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDIbeam.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDIbeam::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateIbeam(this);
}

void AGDDIbeam::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
