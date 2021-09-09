/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVFullPhysVol.h"

#include "LArReadoutGeometry/EMBDetectorRegion.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

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

const Amg::Transform3D&  EMBDetectorRegion::getAbsoluteTransform (const GeoAlignmentStore* alignStore) const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return alignStore
    ? fullPhysVol->getCachedAbsoluteTransform(alignStore)
    : fullPhysVol->getAbsoluteTransform();
}

const Amg::Transform3D&  EMBDetectorRegion::getDefAbsoluteTransform (const GeoAlignmentStore* alignStore) const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return alignStore
    ? fullPhysVol->getCachedDefAbsoluteTransform(alignStore)
    : fullPhysVol->getDefAbsoluteTransform();
}
