/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelSCTDBHelper/SCTGeneralDBHelper.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

using std::abs;

const double SCT_SAFETY = 0.01 * CLHEP::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.



SCTGeneralDBHelper::SCTGeneralDBHelper(const OraclePixGeoAccessor* geoAccessor):
  m_geoAccessor(geoAccessor)
{
  generalTable= geoAccessor->getTable("SctBrlGeneral");
}

//
// General
//
double 
SCTGeneralDBHelper::safety() const
{
  return SCT_SAFETY;
}
