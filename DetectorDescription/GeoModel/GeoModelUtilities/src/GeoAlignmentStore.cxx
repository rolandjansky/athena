/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelUtilities/GeoAlignmentStore.h"

void GeoAlignmentStore::setDelta(const GeoAlignableTransform* axf, const GeoTrf::Transform3D& xf)
{
  m_deltas.setTransform(axf, xf);
}

const GeoTrf::Transform3D* GeoAlignmentStore::getDelta(const GeoAlignableTransform* axf) const
{
  return m_deltas.getTransform(axf);
}

void GeoAlignmentStore::setAbsPosition(const GeoVFullPhysVol* fpv, const GeoTrf::Transform3D& xf) 
{
  m_absPositions.setTransform(fpv,xf);
}
 
const GeoTrf::Transform3D* GeoAlignmentStore::getAbsPosition(const GeoVFullPhysVol* fpv) const 
{
  return m_absPositions.getTransform(fpv);
}

void GeoAlignmentStore::setDefAbsPosition(const GeoVFullPhysVol* fpv, const GeoTrf::Transform3D& xf) 
{
  m_defAbsPositions.setTransform(fpv,xf);
}

const GeoTrf::Transform3D* GeoAlignmentStore::getDefAbsPosition(const GeoVFullPhysVol* fpv) const 
{
  return m_defAbsPositions.getTransform(fpv);
}
