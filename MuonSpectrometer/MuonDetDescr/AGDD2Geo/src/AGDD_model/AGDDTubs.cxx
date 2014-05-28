/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDTubs.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"

void AGDDTubs::CreateSolid() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	builder->CreateTubs(this);
}
void AGDDTubs::CreateVolume() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	CreateSolid();

	builder->CreateVolume(this);
}
