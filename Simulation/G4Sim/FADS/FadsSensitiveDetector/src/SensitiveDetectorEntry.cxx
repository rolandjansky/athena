/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsSensitiveDetector/SensitiveDetectorEntry.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

namespace FADS {

std::string SensitiveDetectorEntry::GetName()
{
	return name;
}

SensitiveDetectorEntry::SensitiveDetectorEntry(std::string n): name(n)
{
	SensitiveDetectorCatalog *c=SensitiveDetectorCatalog::
				    GetSensitiveDetectorCatalog();
	c->RegisterEntry(this);
}

}	// end namespace
