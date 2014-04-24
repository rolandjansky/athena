/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "G4AtlasControl/SenDetectorMenu.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"


SenDetectorMenu::SenDetectorMenu():SimpleMenu("SenDetectorMenu")
{;}
void SenDetectorMenu::printEntries()
{
	FADS::SensitiveDetectorCatalog *sdc=FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
	sdc->PrintEntries();
}
void SenDetectorMenu::printDetectors()
{
	FADS::SensitiveDetectorCatalog *sdc=FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
	sdc->PrintSensitiveDetectorList();
}
void SenDetectorMenu::useSensitiveDetector(const std::string senDet, const std::string name,
					   bool allowMods)
{
	FADS::SensitiveDetectorCatalog *sdc=FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
	sdc->GetSensitiveDetectorEntry(senDet)->Construct(name,allowMods);
}
void SenDetectorMenu::setAllowMods(const std::string senDet, bool allowMods)
{
        FADS::SensitiveDetectorCatalog *sdc=FADS::SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
	sdc->GetSensitiveDetector(senDet)->setAllowMods(allowMods);
}
