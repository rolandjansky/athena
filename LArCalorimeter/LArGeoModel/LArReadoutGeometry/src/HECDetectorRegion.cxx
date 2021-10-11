/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/HECDetectorRegion.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

HECDetectorRegion::HECDetectorRegion (const GeoVFullPhysVol *physVol
				      , const HECDetDescr *hecDescriptor
				      , DetectorSide endcap
				      , double projectivityDisplacement)
  : GeoVDetectorElement(physVol)
  , m_descriptor(hecDescriptor) 
  , m_endcapIndex(endcap)
  , m_projectivityDisplacement(projectivityDisplacement)
{
  hecDescriptor->ref();
}


HECDetectorRegion::~HECDetectorRegion()
{
  m_descriptor->unref();
}

HECCellConstLink HECDetectorRegion::getHECCell (unsigned int ieta, unsigned int iphi) const
{
  HECCell *cell = new HECCell(m_endcapIndex,m_descriptor,ieta,iphi);
  return HECCellConstLink(cell);
}

Amg::Vector3D HECDetectorRegion::getRefPlanePosAmg () const
{
    HepGeom::Point3D<double> clhepPoint = getRefPlanePos();
    return Amg::Vector3D(clhepPoint.x(), clhepPoint.y(), clhepPoint.z());
}
const Amg::Transform3D&  HECDetectorRegion::getAbsoluteTransform (const GeoAlignmentStore* alignStore) const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return alignStore
    ? fullPhysVol->getCachedAbsoluteTransform(alignStore)
    : fullPhysVol->getAbsoluteTransform();
}

const Amg::Transform3D&  HECDetectorRegion::getDefAbsoluteTransform (const GeoAlignmentStore* alignStore) const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return alignStore
    ? fullPhysVol->getCachedDefAbsoluteTransform(alignStore)
    : fullPhysVol->getDefAbsoluteTransform();
}

Amg::Vector3D HECDetectorRegion::getFocalPointPosAmg () const
{
    HepGeom::Point3D<double> clhepPoint = getFocalPointPos();
    return Amg::Vector3D(clhepPoint.x(), clhepPoint.y(), clhepPoint.z());
}


HepGeom::Point3D<double> HECDetectorRegion::getRefPlanePos () const
{

  // Get the material geometry:

  const GeoVFullPhysVol *physVol = getMaterialGeom();
  const GeoLogVol       *logVol  = physVol->getLogVol();
  const GeoShape        *shape   = logVol->getShape();
  if (shape->typeID()!=GeoPcon::getClassTypeID()) {
    throw std::runtime_error ("HECDetectorRegion cannot compute absolute position of reference plane");
  }
  const GeoPcon *pcon = static_cast<const GeoPcon *> (shape);
  HepGeom::Point3D<double> center(0,0,pcon->getZPlane(0));
  return (Amg::EigenTransformToCLHEP(physVol->getAbsoluteTransform())*center);
}

HepGeom::Point3D<double> HECDetectorRegion::getFocalPointPos () const
{
  HepGeom::Point3D<double> zRef = getRefPlanePos();
  int    sgn  = m_endcapIndex==0 ? -1 : +1;
  bool   hec2 = m_descriptor->getSamplingIndex()>1;
  HepGeom::Point3D<double> focalPos = hec2 ? 
    HepGeom::Point3D<double>(0,0,sgn*m_descriptor->getManager()->getFocalToRef2()):
    HepGeom::Point3D<double>(0,0,sgn*m_descriptor->getManager()->getFocalToRef1());
  return zRef - focalPos;
}

