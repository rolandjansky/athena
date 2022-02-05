/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDElcyl.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDElcyl::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateElcyl(this);
}
void AGDDElcyl::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
	builder.CreateVolume(this);
}
