/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A59100F8.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A59100F8.cm

//## begin module%3CD2A59100F8.cp preserve=no
//## end module%3CD2A59100F8.cp

//## Module: GeoTrd%3CD2A59100F8; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTrd.cxx

//## begin module%3CD2A59100F8.additionalIncludes preserve=no
//## end module%3CD2A59100F8.additionalIncludes

//## begin module%3CD2A59100F8.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
#include "GeoModelKernel/GeoPolyhedron.h"
//## end module%3CD2A59100F8.includes

// GeoTrd
#include "GeoModelKernel/GeoTrd.h"
//## begin module%3CD2A59100F8.additionalDeclarations preserve=yes
//## end module%3CD2A59100F8.additionalDeclarations


// Class GeoTrd 

//## begin GeoTrd::s_classType%3CD2A59100F9.attr preserve=no  public: static const std::string {U} "Trd"
const std::string GeoTrd::s_classType = "Trd";
//## end GeoTrd::s_classType%3CD2A59100F9.attr

//## begin GeoTrd::s_classTypeID%3CD2A59100FA.attr preserve=no  public: static const ShapeType {U} 0x16
const ShapeType GeoTrd::s_classTypeID = 0x16;
//## end GeoTrd::s_classTypeID%3CD2A59100FA.attr

GeoTrd::GeoTrd (double XHalfLength1, double XHalfLength2, double YHalfLength1, double YHalfLength2, double ZHalfLength)
  //## begin GeoTrd::GeoTrd%3CD5B5110183.hasinit preserve=no
  //## end GeoTrd::GeoTrd%3CD5B5110183.hasinit
  //## begin GeoTrd::GeoTrd%3CD5B5110183.initialization preserve=yes
  :
m_xHalfLength1 (XHalfLength1),
m_xHalfLength2 (XHalfLength2),
m_yHalfLength1 (YHalfLength1),
m_yHalfLength2 (YHalfLength2),
m_zHalfLength (ZHalfLength)
  //## end GeoTrd::GeoTrd%3CD5B5110183.initialization
{
  //## begin GeoTrd::GeoTrd%3CD5B5110183.body preserve=yes
  //## end GeoTrd::GeoTrd%3CD5B5110183.body
}


GeoTrd::~GeoTrd()
{
  //## begin GeoTrd::~GeoTrd%3CD2A59100F8_dest.body preserve=yes
  //## end GeoTrd::~GeoTrd%3CD2A59100F8_dest.body
}



//## Other Operations (implementation)
double GeoTrd::volume () const
{
  //## begin GeoTrd::volume%3CD2A6F6026D.body preserve=yes
  double fDz = m_zHalfLength;
  double fDy1 = m_yHalfLength1;
  double fDx1 = m_xHalfLength1;
  double fDx2 = m_xHalfLength2;
  double fDy2 = m_yHalfLength2;
  return 4.0 * ((fDx1 + fDx2) * (fDy1 + fDy2) * (fDz * 0.5) +
		(fDx2 - fDx1) * (fDy2 - fDy1) * (fDz * (1./6)));

  //## end GeoTrd::volume%3CD2A6F6026D.body
}

const std::string & GeoTrd::type () const
{
  //## begin GeoTrd::type%3CD2A8610279.body preserve=yes
  return s_classType;
  //## end GeoTrd::type%3CD2A8610279.body
}

ShapeType GeoTrd::typeID () const
{
  //## begin GeoTrd::typeID%3CD2A861028D.body preserve=yes
  return s_classTypeID;
  //## end GeoTrd::typeID%3CD2A861028D.body
}

void GeoTrd::exec (GeoShapeAction *action) const
{
  //## begin GeoTrd::exec%3DB96A8300BB.body preserve=yes
	action->handleTrd(this);
  //## end GeoTrd::exec%3DB96A8300BB.body
}

// Additional Declarations
  //## begin GeoTrd%3CD2A59100F8.declarations preserve=yes
  //## end GeoTrd%3CD2A59100F8.declarations

//## begin module%3CD2A59100F8.epilog preserve=yes
//## end module%3CD2A59100F8.epilog
