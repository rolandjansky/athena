/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDSnake.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"

void AGDDSnake::CreateSolid() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	builder->CreateSnake(this);
}


void AGDDSnake::CreateVolume() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
