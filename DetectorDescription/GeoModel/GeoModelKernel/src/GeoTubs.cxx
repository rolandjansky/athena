/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A64B01BD.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A64B01BD.cm

//## begin module%3CD2A64B01BD.cp preserve=no
//## end module%3CD2A64B01BD.cp

//## Module: GeoTubs%3CD2A64B01BD; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTubs.cxx

//## begin module%3CD2A64B01BD.additionalIncludes preserve=no
//## end module%3CD2A64B01BD.additionalIncludes

//## begin module%3CD2A64B01BD.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
#include "GeoModelKernel/GeoPolyhedron.h"
//## end module%3CD2A64B01BD.includes

// GeoTubs
#include "GeoModelKernel/GeoTubs.h"
//## begin module%3CD2A64B01BD.additionalDeclarations preserve=yes
//## end module%3CD2A64B01BD.additionalDeclarations


// Class GeoTubs 

//## begin GeoTubs::s_classType%3CD2A64B01BE.attr preserve=no  public: static const std::string {U} "Tubs"
const std::string GeoTubs::s_classType = "Tubs";
//## end GeoTubs::s_classType%3CD2A64B01BE.attr

//## begin GeoTubs::s_classTypeID%3CD2A64B01BF.attr preserve=no  public: static const ShapeType {U} 0x18
const ShapeType GeoTubs::s_classTypeID = 0x18;
//## end GeoTubs::s_classTypeID%3CD2A64B01BF.attr

GeoTubs::GeoTubs (double RMin, double RMax, double ZHalfLength, double SPhi, double DPhi)
  //## begin GeoTubs::GeoTubs%3CD5B1E402BD.hasinit preserve=no
  //## end GeoTubs::GeoTubs%3CD5B1E402BD.hasinit
  //## begin GeoTubs::GeoTubs%3CD5B1E402BD.initialization preserve=yes
  :
m_rMin (RMin),
m_rMax (RMax),
m_zHalfLength (ZHalfLength),
m_sPhi (SPhi),
m_dPhi (DPhi)
  //## end GeoTubs::GeoTubs%3CD5B1E402BD.initialization
{
  //## begin GeoTubs::GeoTubs%3CD5B1E402BD.body preserve=yes
  //## end GeoTubs::GeoTubs%3CD5B1E402BD.body
}


GeoTubs::~GeoTubs()
{
  //## begin GeoTubs::~GeoTubs%3CD2A64B01BD_dest.body preserve=yes
  //## end GeoTubs::~GeoTubs%3CD2A64B01BD_dest.body
}



//## Other Operations (implementation)
double GeoTubs::volume () const
{
  //## begin GeoTubs::volume%3CD2A71A00FD.body preserve=yes
  return m_dPhi * (m_rMax * m_rMax - m_rMin * m_rMin) * m_zHalfLength;
  //## end GeoTubs::volume%3CD2A71A00FD.body
}

const std::string & GeoTubs::type () const
{
  //## begin GeoTubs::type%3CD2A86F0099.body preserve=yes
  return s_classType;
  //## end GeoTubs::type%3CD2A86F0099.body
}

ShapeType GeoTubs::typeID () const
{
  //## begin GeoTubs::typeID%3CD2A86F00B7.body preserve=yes
  return s_classTypeID;
  //## end GeoTubs::typeID%3CD2A86F00B7.body
}

void GeoTubs::exec (GeoShapeAction *action) const
{
  //## begin GeoTubs::exec%3DB96A9200C6.body preserve=yes
	action->handleTubs(this);
  //## end GeoTubs::exec%3DB96A9200C6.body
}

// Additional Declarations
  //## begin GeoTubs%3CD2A64B01BD.declarations preserve=yes
  //## end GeoTubs%3CD2A64B01BD.declarations

//## begin module%3CD2A64B01BD.epilog preserve=yes
//## end module%3CD2A64B01BD.epilog
