/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDSnake.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDSnake::CreateSolid (const AGDDBuilder& builder) 
{
	builder.CreateSnake(this);
}


void AGDDSnake::CreateVolume (AGDDBuilder& builder) 
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
