/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDUbeam.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"

void AGDDUbeam::CreateSolid() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	builder->CreateUbeam(this);
}

void AGDDUbeam::CreateVolume() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
