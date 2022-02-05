/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDIntersection.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDIntersection::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateIntersection(this);
}


void AGDDIntersection::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
