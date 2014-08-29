/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Geo2G4/Geo2G4Svc.h"

DECLARE_SERVICE_FACTORY(Geo2G4Svc) 

DECLARE_FACTORY_ENTRIES(Geo2G4) 
{
		DECLARE_SERVICE(Geo2G4Svc)
}

