/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2696E017F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2696E017F.cm

//## begin module%3CD2696E017F.cp preserve=no
//## end module%3CD2696E017F.cp

//## Module: GeoBox%3CD2696E017F; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoBox.cxx

//## begin module%3CD2696E017F.additionalIncludes preserve=no
//## end module%3CD2696E017F.additionalIncludes

//## begin module%3CD2696E017F.includes preserve=yes
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD2696E017F.includes

// GeoBox
#include "GeoModelKernel/GeoBox.h"
//## begin module%3CD2696E017F.additionalDeclarations preserve=yes
//## end module%3CD2696E017F.additionalDeclarations


// Class GeoBox 

//## begin GeoBox::s_classType%3CD26A64002E.attr preserve=no  public: static const std::string {U} "Box"
const std::string GeoBox::s_classType = "Box";
//## end GeoBox::s_classType%3CD26A64002E.attr

//## begin GeoBox::s_classTypeID%3CD26A640038.attr preserve=no  public: static const ShapeType {U} 0x10
const ShapeType GeoBox::s_classTypeID = 0x10;
//## end GeoBox::s_classTypeID%3CD26A640038.attr

GeoBox::GeoBox (double XHalfLength, double YHalfLength, double ZHalfLength)
  //## begin GeoBox::GeoBox%3CD58E8B01CE.hasinit preserve=no
  //## end GeoBox::GeoBox%3CD58E8B01CE.hasinit
  //## begin GeoBox::GeoBox%3CD58E8B01CE.initialization preserve=yes
  :
m_xHalfLength (XHalfLength),
m_yHalfLength (YHalfLength),
m_zHalfLength (ZHalfLength)
  //## end GeoBox::GeoBox%3CD58E8B01CE.initialization
{
  //## begin GeoBox::GeoBox%3CD58E8B01CE.body preserve=yes
  //## end GeoBox::GeoBox%3CD58E8B01CE.body
}


GeoBox::~GeoBox()
{
  //## begin GeoBox::~GeoBox%3CD2696E017F_dest.body preserve=yes
  //## end GeoBox::~GeoBox%3CD2696E017F_dest.body
}



//## Other Operations (implementation)
double GeoBox::volume () const
{
  //## begin GeoBox::volume%3CD2A69A0008.body preserve=yes
  return 8.0 * m_xHalfLength * m_yHalfLength * m_zHalfLength;

  //## end GeoBox::volume%3CD2A69A0008.body
}

const std::string & GeoBox::type () const
{
  //## begin GeoBox::type%3CD2A7FB011E.body preserve=yes
  return s_classType;
  //## end GeoBox::type%3CD2A7FB011E.body
}

ShapeType GeoBox::typeID () const
{
  //## begin GeoBox::typeID%3CD2A7FB0132.body preserve=yes
  return s_classTypeID;
  //## end GeoBox::typeID%3CD2A7FB0132.body
}

void GeoBox::exec (GeoShapeAction *action) const
{
  //## begin GeoBox::exec%3DB96A050150.body preserve=yes
	action->handleBox(this);
  //## end GeoBox::exec%3DB96A050150.body
}

// Additional Declarations
  //## begin GeoBox%3CD2696E017F.declarations preserve=yes
  //## end GeoBox%3CD2696E017F.declarations

//## begin module%3CD2696E017F.epilog preserve=yes
//## end module%3CD2696E017F.epilog
