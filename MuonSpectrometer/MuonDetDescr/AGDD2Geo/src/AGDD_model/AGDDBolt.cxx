/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AGDD2Geo/AGDDBolt.h"
#include "AGDD2Geo/AGDDController.h"
#include "AGDD2Geo/AGDDBuilder.h"

void AGDDBolt::CreateSolid() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	builder->CreateBolt(this);
}

void AGDDBolt::CreateVolume() 
{
	AGDDBuilder* builder=AGDDController::GetController()->GetBuilder();

	CreateSolid();

 	builder->CreateVolume(this);
}
