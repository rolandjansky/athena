/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDDModel/AGDDSnake.h"
#include "AGDDKernel/AGDDBuilder.h"

void AGDDSnake::CreateSolid() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	builder->CreateSnake(this);
}


void AGDDSnake::CreateVolume() 
{
	AGDDBuilder* builder=AGDDBuilder::CurrentBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
