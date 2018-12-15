/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVFullPhysVol.h"

#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "GeoPrimitives/GeoPrimitives.h"

EMBDetectorRegion::EMBDetectorRegion (const GeoVFullPhysVol *physVol
				      , const EMBDetDescr *embDescriptor
				      , DetectorSide endcap)
  : GeoVDetectorElement(physVol)
  , m_descriptor(embDescriptor)
  , m_endcapIndex(endcap)
{
  m_descriptor->ref();
}


EMBDetectorRegion::~EMBDetectorRegion()
{
  m_descriptor->unref();
}

EMBCellConstLink EMBDetectorRegion::getEMBCell (unsigned int ieta, unsigned int iphi) const
{
  return EMBCellConstLink(new EMBCell(m_endcapIndex,m_descriptor,ieta,iphi));
}

const Amg::Transform3D  EMBDetectorRegion::getAbsoluteTransformAmg () const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getAbsoluteTransform();
}

const Amg::Transform3D  EMBDetectorRegion::getDefAbsoluteTransformAmg () const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getDefAbsoluteTransform();
}
const GeoTrf::Transform3D &  EMBDetectorRegion::getAbsoluteTransform () const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getAbsoluteTransform();
}

const GeoTrf::Transform3D &  EMBDetectorRegion::getDefAbsoluteTransform () const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getDefAbsoluteTransform();
}
