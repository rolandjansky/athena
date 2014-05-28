/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDTrd.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"

void AGDDTrd::CreateSolid() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	builder->CreateTrd(this);
}

void AGDDTrd::CreateVolume() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
