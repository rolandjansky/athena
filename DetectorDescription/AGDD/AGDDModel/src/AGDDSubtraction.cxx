/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDSubtraction.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDSubtraction::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateSubtraction(this);
}


void AGDDSubtraction::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
