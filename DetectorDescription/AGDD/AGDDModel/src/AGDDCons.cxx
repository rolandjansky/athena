/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDCons.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDCons::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateCons(this);
}
void AGDDCons::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
