/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldManager.h"
#include "FadsField/MagneticFieldMap.h"

#include <string>

namespace FADS {

MagneticFieldMap::MagneticFieldMap()
{
	name="GenericFieldMap";
	initialized=false;
}

MagneticFieldMap::MagneticFieldMap(std::string n): name(n)
{
	initialized=false;
}

void MagneticFieldMap::GetFieldValue(const double *xyzPos, double *xyzField) const {
  FieldValue(xyzPos, xyzField);
}

void MagneticFieldMap::SetInitialValues()
{
	Initialize();
	initialized=true;
}

}	// end namespace
