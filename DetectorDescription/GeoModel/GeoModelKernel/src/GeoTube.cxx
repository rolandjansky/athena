/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A5E301F0.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A5E301F0.cm

//## begin module%3CD2A5E301F0.cp preserve=no
//## end module%3CD2A5E301F0.cp

//## Module: GeoTube%3CD2A5E301F0; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTube.cxx

//## begin module%3CD2A5E301F0.additionalIncludes preserve=no
//## end module%3CD2A5E301F0.additionalIncludes

//## begin module%3CD2A5E301F0.includes preserve=yes
#include <cmath>
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD2A5E301F0.includes

// GeoTube
#include "GeoModelKernel/GeoTube.h"
//## begin module%3CD2A5E301F0.additionalDeclarations preserve=yes
//## end module%3CD2A5E301F0.additionalDeclarations


// Class GeoTube 

//## begin GeoTube::s_classType%3CD2A5E301FA.attr preserve=no  public: static const std::string {U} "Tube"
const std::string GeoTube::s_classType = "Tube";
//## end GeoTube::s_classType%3CD2A5E301FA.attr

//## begin GeoTube::s_classTypeID%3CD2A5E301FB.attr preserve=no  public: static const ShapeType {U} 0x17
const ShapeType GeoTube::s_classTypeID = 0x17;
//## end GeoTube::s_classTypeID%3CD2A5E301FB.attr

GeoTube::GeoTube (double RMin, double RMax, double ZHalfLength)
  //## begin GeoTube::GeoTube%3CD5AF79016A.hasinit preserve=no
  //## end GeoTube::GeoTube%3CD5AF79016A.hasinit
  //## begin GeoTube::GeoTube%3CD5AF79016A.initialization preserve=yes
  :
m_rMin (RMin),
m_rMax (RMax),
m_zHalfLength (ZHalfLength)
  //## end GeoTube::GeoTube%3CD5AF79016A.initialization
{
  //## begin GeoTube::GeoTube%3CD5AF79016A.body preserve=yes
  //## end GeoTube::GeoTube%3CD5AF79016A.body
}


GeoTube::~GeoTube()
{
  //## begin GeoTube::~GeoTube%3CD2A5E301F0_dest.body preserve=yes
  //## end GeoTube::~GeoTube%3CD2A5E301F0_dest.body
}



//## Other Operations (implementation)
double GeoTube::volume () const
{
  //## begin GeoTube::volume%3CD2A7130369.body preserve=yes
#ifndef M_PI
  double M_PI = acos (-1.0);
#endif
  return 2 * M_PI * (m_rMax * m_rMax - m_rMin * m_rMin) * m_zHalfLength;
  //## end GeoTube::volume%3CD2A7130369.body
}

const std::string & GeoTube::type () const
{
  //## begin GeoTube::type%3CD2A868007B.body preserve=yes
  return s_classType;
  //## end GeoTube::type%3CD2A868007B.body
}

ShapeType GeoTube::typeID () const
{
  //## begin GeoTube::typeID%3CD2A8680099.body preserve=yes
  return s_classTypeID;
  //## end GeoTube::typeID%3CD2A8680099.body
}

void GeoTube::exec (GeoShapeAction *action) const
{
  //## begin GeoTube::exec%3DB96A8A03D2.body preserve=yes
	action->handleTube(this);
  //## end GeoTube::exec%3DB96A8A03D2.body
}

// Additional Declarations
  //## begin GeoTube%3CD2A5E301F0.declarations preserve=yes
  //## end GeoTube%3CD2A5E301F0.declarations

//## begin module%3CD2A5E301F0.epilog preserve=yes
//## end module%3CD2A5E301F0.epilog
