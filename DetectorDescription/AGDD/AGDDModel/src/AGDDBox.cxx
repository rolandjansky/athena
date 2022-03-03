/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDBox.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDBox::CreateSolid (const AGDDBuilder& builder)
{
	builder.CreateBox(this);
}

void AGDDBox::CreateVolume (AGDDBuilder& builder)
{
	CreateSolid (builder);
 	builder.CreateVolume(this);
}
