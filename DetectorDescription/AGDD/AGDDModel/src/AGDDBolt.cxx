/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDBolt.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDBolt::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateBolt(this);
}

void AGDDBolt::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
