/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileG4DetDescr/DetectorDescription.h"
#include "TileG4DetDescr/DetectorDescriptionStore.h"

namespace FADS {

DetectorDescription::DetectorDescription(std::string n):name(n) 
{
	DetectorDescriptionStore *dd=
		DetectorDescriptionStore::GetDetectorDescriptionStore();
	dd->AddDetectorDescription(this);
	isPointed=false;
}

DetectorDescription::DetectorDescription(const DetectorDescription& d)
{
	DetectorDescriptionStore *dd=
		DetectorDescriptionStore::GetDetectorDescriptionStore();
	dd->ReplaceDetectorDescription(d.GetName(),this);
	isPointed=false;
}

std::string DetectorDescription::GetName() const
{
	return name;
}

}	// end namespace
