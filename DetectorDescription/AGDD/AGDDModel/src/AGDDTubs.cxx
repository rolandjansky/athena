/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDTubs.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDTubs::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateTubs(this);
}
void AGDDTubs::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

	builder->CreateVolume(this);
}
