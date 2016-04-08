/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%446626240261.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%446626240261.CLHEP::cm

//## begin module%446626240261.cp preserve=no
//## end module%446626240261.cp

//## Module: EMECDetectorRegion%446626240261; Pseudo Package body
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/src/EMECDetectorRegion.cxx

//## begin module%446626240261.additionalIncludes preserve=no
//## end module%446626240261.additionalIncludes

//## begin module%446626240261.includes preserve=yes
#include "GeoModelKernel/GeoVFullPhysVol.h"
//## end module%446626240261.includes

// EMECDetectorRegion
#include "LArReadoutGeometry/EMECDetectorRegion.h"
//## begin module%446626240261.additionalDeclarations preserve=yes
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoPcon.h"
//## end module%446626240261.additionalDeclarations


// Class EMECDetectorRegion 

EMECDetectorRegion::EMECDetectorRegion (const GeoVFullPhysVol *physVol, const EMECDetDescr *emecDescriptor, DetectorSide endcap, double projectivityDisplacement)
  //## begin EMECDetectorRegion::EMECDetectorRegion%4466280603B3.hasinit preserve=no
  //## end EMECDetectorRegion::EMECDetectorRegion%4466280603B3.hasinit
  //## begin EMECDetectorRegion::EMECDetectorRegion%4466280603B3.initialization preserve=yes
  :GeoVDetectorElement(physVol),m_descriptor(emecDescriptor),m_endcapIndex(endcap),m_projectivityDisplacement(projectivityDisplacement)
  //## end EMECDetectorRegion::EMECDetectorRegion%4466280603B3.initialization
{
  //## begin EMECDetectorRegion::EMECDetectorRegion%4466280603B3.body preserve=yes
  emecDescriptor->ref();
  //## end EMECDetectorRegion::EMECDetectorRegion%4466280603B3.body
}


EMECDetectorRegion::~EMECDetectorRegion()
{
  //## begin EMECDetectorRegion::~EMECDetectorRegion%446626240261_dest.body preserve=yes
  m_descriptor->unref();
  //## end EMECDetectorRegion::~EMECDetectorRegion%446626240261_dest.body
}



//## Other Operations (implementation)
EMECCellConstLink EMECDetectorRegion::getEMECCell (unsigned int ieta, unsigned int iphi) const
{
  //## begin EMECDetectorRegion::getEMECCell%446629070190.body preserve=yes
  return EMECCellConstLink(new EMECCell(m_endcapIndex,m_descriptor,ieta,iphi));
  //## end EMECDetectorRegion::getEMECCell%446629070190.body
}

const HepGeom::Transform3D &  EMECDetectorRegion::getAbsoluteTransform () const
{
  //## begin EMECDetectorRegion::getAbsoluteTransform%446E079E0373.body preserve=yes
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getAbsoluteTransform();
  //## end EMECDetectorRegion::getAbsoluteTransform%446E079E0373.body
}

const HepGeom::Transform3D &  EMECDetectorRegion::getDefAbsoluteTransform () const
{
  //## begin EMECDetectorRegion::getDefAbsoluteTransform%446E079E037D.body preserve=yes
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getDefAbsoluteTransform();
  //## end EMECDetectorRegion::getDefAbsoluteTransform%446E079E037D.body
}

HepGeom::Point3D<double> EMECDetectorRegion::getRefPlanePos () const
{
  //## begin EMECDetectorRegion::getRefPlanePos%45432A27024F.body preserve=yes
  const GeoVFullPhysVol *physVol = getMaterialGeom();
  const GeoLogVol       *logVol  = physVol->getLogVol();
  const GeoShape        *shape   = logVol->getShape();
  if (shape->typeID()!=GeoPcon::getClassTypeID()) {
    throw std::runtime_error ("EMECDetectorRegion cannot compute absolute position of reference plane");
  }
  GeoPcon *pcon = (GeoPcon *) shape;
  HepGeom::Point3D<double> center(0,0,pcon->getZPlane(0)-m_descriptor->getManager()->getRefToActive());
  return (physVol->getAbsoluteTransform()*center);
  //## end EMECDetectorRegion::getRefPlanePos%45432A27024F.body
}

HepGeom::Point3D<double> EMECDetectorRegion::getFocalPointPos () const
{
  //## begin EMECDetectorRegion::getFocalPointPos%45432A270254.body preserve=yes
  HepGeom::Point3D<double> zRef = getRefPlanePos();
  int    sgn  = m_endcapIndex==0 ? -1 : +1;
  HepGeom::Point3D<double> focalPos = HepGeom::Point3D<double>(0,0,sgn*m_descriptor->getManager()->getFocalToRef());
  return zRef - focalPos;
  //## end EMECDetectorRegion::getFocalPointPos%45432A270254.body
}

const Amg::Transform3D  EMECDetectorRegion::getAbsoluteTransformAmg () const
{
  return Amg::CLHEPTransformToEigen(getAbsoluteTransform());
}

const Amg::Transform3D  EMECDetectorRegion::getDefAbsoluteTransformAmg () const
{
  return Amg::CLHEPTransformToEigen(getDefAbsoluteTransform());
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
