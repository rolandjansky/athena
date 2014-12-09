/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDElcyl.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDElcyl::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateElcyl(this);
}
void AGDDElcyl::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

	builder->CreateVolume(this);
}
