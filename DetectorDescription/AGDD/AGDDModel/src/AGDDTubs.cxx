/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDTubs.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDTubs::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateTubs(this);
}
void AGDDTubs::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
	builder.CreateVolume(this);
}
