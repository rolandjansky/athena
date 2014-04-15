/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Barrel/LArCoudes.h"

PhysicalVolumeAccessor* LArCoudes::theCoudes=0;

LArCoudes::LArCoudes(std::string strDetector) 
{
	if (theCoudes==0) 
	{
           if(strDetector=="")
           {
			theCoudes=
			  new PhysicalVolumeAccessor("LAr::EMB::STAC",
						     "LAr::EMB::Electrode::CornerDownFold");
			theCoudes->SetPhysicalVolumeList("LAr::EMB::Electrode::CornerUpFold");
           }
           else
           {
			theCoudes=
			  new PhysicalVolumeAccessor(strDetector+"::LAr::EMB::STAC",
						     strDetector+"::LAr::EMB::Electrode::CornerDownFold");
			theCoudes->SetPhysicalVolumeList(strDetector+"::LAr::EMB::Electrode::CornerUpFold");
	   }
        }				   
}
double LArCoudes::XCentCoude(int stackid, int cellid)
{
	int id=cellid+stackid*10000;
	const G4VPhysicalVolume *pv=theCoudes->GetPhysicalVolume(id);
	const G4ThreeVector& tv=pv->GetTranslation();
	return tv.x();
}
double LArCoudes::YCentCoude(int stackid, int cellid)
{
	int id=cellid+stackid*10000;
	const G4VPhysicalVolume *pv=theCoudes->GetPhysicalVolume(id);
	const G4ThreeVector& tv=pv->GetTranslation();
	return tv.y();
}
