/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Barrel/LArCoudes.h"

PhysicalVolumeAccessor* LArCoudes::s_theCoudes=0;

LArCoudes::LArCoudes(std::string strDetector) 
{
	if (s_theCoudes==0) 
	{
           if(strDetector=="")
           {
			s_theCoudes=
			  new PhysicalVolumeAccessor("LAr::EMB::STAC",
						     "LAr::EMB::Electrode::CornerDownFold");
			s_theCoudes->SetPhysicalVolumeList("LAr::EMB::Electrode::CornerUpFold");
           }
           else
           {
			s_theCoudes=
			  new PhysicalVolumeAccessor(strDetector+"::LAr::EMB::STAC",
						     strDetector+"::LAr::EMB::Electrode::CornerDownFold");
			s_theCoudes->SetPhysicalVolumeList(strDetector+"::LAr::EMB::Electrode::CornerUpFold");
	   }
        }				   
}
double LArCoudes::XCentCoude(int stackid, int cellid)
{
	int id=cellid+stackid*10000;
	const G4VPhysicalVolume *pv=s_theCoudes->GetPhysicalVolume(id);
        if (!pv) std::abort();
	const G4ThreeVector& tv=pv->GetTranslation();
	return tv.x();
}
double LArCoudes::YCentCoude(int stackid, int cellid)
{
	int id=cellid+stackid*10000;
	const G4VPhysicalVolume *pv=s_theCoudes->GetPhysicalVolume(id);
        if (!pv) std::abort();
	const G4ThreeVector& tv=pv->GetTranslation();
	return tv.y();
}
