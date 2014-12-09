/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDUnion.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDUnion::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateUnion(this);
}

void AGDDUnion::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
