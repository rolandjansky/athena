/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDTrd.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDTrd::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateTrd(this);
}

void AGDDTrd::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
