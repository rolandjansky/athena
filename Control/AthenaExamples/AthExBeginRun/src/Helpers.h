/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXBEGINRUN_HELPERS
#define ATHEXBEGINRUN_HELPERS

// Test conditions
#include "IOVDbTestConditions/IOVDbTestAmdbCorrection.h"

/// Helper to print conditions object
std::ostream& operator <<(std::ostream &os, const IOVDbTestAmdbCorrection& obj)
{
  HepGeom::Point3D<double> trans = obj.getTranslation();
  HepGeom::Point3D<double> rot   = obj.getRotation();
  os << "trans = (" << trans.x() << "," << trans.y() << "," << trans.z()
     << ") rot = (" <<  rot.x() << "," << rot.y() << "," << rot.z() << ")";
  return os;
}

#endif
