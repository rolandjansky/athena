/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDBolt.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDBolt::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateBolt(this);
}

void AGDDBolt::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
