/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDPcon.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDPcon::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreatePcon(this);
}

void AGDDPcon::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
