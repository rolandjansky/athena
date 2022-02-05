/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDPgon.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDPgon::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreatePgon(this);
}

void AGDDPgon::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
