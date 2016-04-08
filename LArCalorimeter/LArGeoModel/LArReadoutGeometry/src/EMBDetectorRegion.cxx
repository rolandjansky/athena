/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.7%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.7%.codegen_version

//## begin module%44957D640109.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%44957D640109.CLHEP::cm

//## begin module%44957D640109.cp preserve=no
//## end module%44957D640109.cp

//## Module: EMBDetectorRegion%44957D640109; Pseudo Package body
//## Source file: /home/atlas/TEST-12.0.2-00/LArCalorimeter/LArGeoModel/LArReadoutGeometry/src/EMBDetectorRegion.cxx

//## begin module%44957D640109.additionalIncludes preserve=no
//## end module%44957D640109.additionalIncludes

//## begin module%44957D640109.includes preserve=yes
#include "GeoModelKernel/GeoVFullPhysVol.h"
//## end module%44957D640109.includes

// EMBDetectorRegion
#include "LArReadoutGeometry/EMBDetectorRegion.h"
//## begin module%44957D640109.additionalDeclarations preserve=yes
//## end module%44957D640109.additionalDeclarations

#include "GeoPrimitives/GeoPrimitives.h"

// Class EMBDetectorRegion 

EMBDetectorRegion::EMBDetectorRegion (const GeoVFullPhysVol *physVol, const EMBDetDescr *embDescriptor, DetectorSide endcap)
  //## begin EMBDetectorRegion::EMBDetectorRegion%4495802702A2.hasinit preserve=no
  //## end EMBDetectorRegion::EMBDetectorRegion%4495802702A2.hasinit
  //## begin EMBDetectorRegion::EMBDetectorRegion%4495802702A2.initialization preserve=yes
  :GeoVDetectorElement(physVol),m_descriptor(embDescriptor),m_endcapIndex(endcap)
  //## end EMBDetectorRegion::EMBDetectorRegion%4495802702A2.initialization
{
  //## begin EMBDetectorRegion::EMBDetectorRegion%4495802702A2.body preserve=yes
  m_descriptor->ref();
  //## end EMBDetectorRegion::EMBDetectorRegion%4495802702A2.body
}


EMBDetectorRegion::~EMBDetectorRegion()
{
  //## begin EMBDetectorRegion::~EMBDetectorRegion%44957D640109_dest.body preserve=yes
  m_descriptor->unref();
  //## end EMBDetectorRegion::~EMBDetectorRegion%44957D640109_dest.body
}



//## Other Operations (implementation)
EMBCellConstLink EMBDetectorRegion::getEMBCell (unsigned int ieta, unsigned int iphi) const
{
  //## begin EMBDetectorRegion::getEMBCell%449580270257.body preserve=yes
 return EMBCellConstLink(new EMBCell(m_endcapIndex,m_descriptor,ieta,iphi));
  //## end EMBDetectorRegion::getEMBCell%449580270257.body
}

const Amg::Transform3D  EMBDetectorRegion::getAbsoluteTransformAmg () const
{
  //## begin EMBDetectorRegion::getAbsoluteTransform%449580270293.body preserve=yes
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return Amg::CLHEPTransformToEigen(fullPhysVol->getAbsoluteTransform());
  //## end EMBDetectorRegion::getAbsoluteTransform%449580270293.body
}

const Amg::Transform3D  EMBDetectorRegion::getDefAbsoluteTransformAmg () const
{
  //## begin EMBDetectorRegion::getDefAbsoluteTransform%44958027029A.body preserve=yes
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return Amg::CLHEPTransformToEigen(fullPhysVol->getDefAbsoluteTransform());
  //## end EMBDetectorRegion::getDefAbsoluteTransform%44958027029A.body
}
const HepGeom::Transform3D &  EMBDetectorRegion::getAbsoluteTransform () const
{
  //## begin EMBDetectorRegion::getAbsoluteTransform%449580270293.body preserve=yes
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getAbsoluteTransform();
  //## end EMBDetectorRegion::getAbsoluteTransform%449580270293.body
}

const HepGeom::Transform3D &  EMBDetectorRegion::getDefAbsoluteTransform () const
{
  //## begin EMBDetectorRegion::getDefAbsoluteTransform%44958027029A.body preserve=yes
  const GeoVFullPhysVol *fullPhysVol = getMaterialGeom();
  return fullPhysVol->getDefAbsoluteTransform();
  //## end EMBDetectorRegion::getDefAbsoluteTransform%44958027029A.body
}


// Additional Declarations
  //## begin EMBDetectorRegion%44957D640109.declarations preserve=yes
  //## end EMBDetectorRegion%44957D640109.declarations

//## begin module%44957D640109.epilog preserve=yes
//## end module%44957D640109.epilog
