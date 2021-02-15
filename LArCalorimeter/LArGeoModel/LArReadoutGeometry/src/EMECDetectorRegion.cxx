/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVFullPhysVol.h"

#include "LArReadoutGeometry/EMECDetectorRegion.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

EMECDetectorRegion::EMECDetectorRegion (const GeoVFullPhysVol *physVol
					, const EMECDetDescr *emecDescriptor
					, DetectorSide endcap
					, double projectivityDisplacement)
  : GeoVDetectorElement(physVol)
  , m_descriptor(emecDescriptor)
  , m_endcapIndex(endcap)
  , m_projectivityDisplacement(projectivityDisplacement)
{
  emecDescriptor->ref();
}

EMECDetectorRegion::~EMECDetectorRegion()
{
  m_descriptor->unref();
}

EMECCellConstLink EMECDetectorRegion::getEMECCell (unsigned int ieta, unsigned int iphi) const
{
  return EMECCellConstLink(new EMECCell(m_endcapIndex,m_descriptor,ieta,iphi));
}

const GeoTrf::Transform3D &  EMECDetectorRegion::getAbsoluteTransform () const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getAbsoluteTransform();
}

const GeoTrf::Transform3D &  EMECDetectorRegion::getDefAbsoluteTransform () const
{
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getDefAbsoluteTransform();
}

HepGeom::Point3D<double> EMECDetectorRegion::getRefPlanePos () const
{
  const GeoVFullPhysVol *physVol = getMaterialGeom();
  const GeoLogVol       *logVol  = physVol->getLogVol();
  const GeoShape        *shape   = logVol->getShape();
  if (shape->typeID()!=GeoPcon::getClassTypeID()) {
    throw std::runtime_error ("EMECDetectorRegion cannot compute absolute position of reference plane");
  }
  const GeoPcon *pcon = static_cast<const GeoPcon *> (shape);
  HepGeom::Point3D<double> center(0,0,pcon->getZPlane(0)-m_descriptor->getManager()->getRefToActive());
  return (Amg::EigenTransformToCLHEP(physVol->getAbsoluteTransform())*center);
}

HepGeom::Point3D<double> EMECDetectorRegion::getFocalPointPos () const
{
  HepGeom::Point3D<double> zRef = getRefPlanePos();
  int    sgn  = m_endcapIndex==0 ? -1 : +1;
  HepGeom::Point3D<double> focalPos = HepGeom::Point3D<double>(0,0,sgn*m_descriptor->getManager()->getFocalToRef());
  return zRef - focalPos;
}

const Amg::Transform3D  EMECDetectorRegion::getAbsoluteTransformAmg () const
{
  return getAbsoluteTransform();
}

const Amg::Transform3D  EMECDetectorRegion::getDefAbsoluteTransformAmg () const
{
  return getDefAbsoluteTransform();
}

Amg::Vector3D EMECDetectorRegion::getRefPlanePosAmg () const
{
    const HepGeom::Point3D<double> point = getRefPlanePos();
    return Amg::Vector3D(point.x(), point.y(), point.z());
}

Amg::Vector3D EMECDetectorRegion::getFocalPointPosAmg () const
{
    const HepGeom::Point3D<double> point = getFocalPointPos();
    return Amg::Vector3D(point.x(),point.y(),point.z());
}
