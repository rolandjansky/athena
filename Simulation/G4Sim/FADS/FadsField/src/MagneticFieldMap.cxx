/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsField/FieldManager.h"
#include "FadsField/MagneticFieldMap.h"

#include <string>

namespace FADS {

MagneticFieldMap::MagneticFieldMap()
{
	m_name="GenericFieldMap";
	m_initialized=false;
}

MagneticFieldMap::MagneticFieldMap(std::string n): m_name(n)
{
	m_initialized=false;
}

void MagneticFieldMap::GetFieldValue(const double *xyzPos, double *xyzField) const {
  FieldValue(xyzPos, xyzField);
}

void MagneticFieldMap::SetInitialValues()
{
	Initialize();
	m_initialized=true;
}

}	// end namespace
