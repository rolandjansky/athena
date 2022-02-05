/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDUnion.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDUnion::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateUnion(this);
}

void AGDDUnion::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
