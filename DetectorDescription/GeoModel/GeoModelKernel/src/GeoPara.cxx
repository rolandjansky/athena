/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD26D7503D0.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD26D7503D0.cm

//## begin module%3CD26D7503D0.cp preserve=no
//## end module%3CD26D7503D0.cp

//## Module: GeoPara%3CD26D7503D0; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPara.cxx

//## begin module%3CD26D7503D0.additionalIncludes preserve=no
//## end module%3CD26D7503D0.additionalIncludes

//## begin module%3CD26D7503D0.includes preserve=yes
#include <cmath>
#include "CLHEP/Geometry/Vector3D.h"
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD26D7503D0.includes

// GeoPara
#include "GeoModelKernel/GeoPara.h"
//## begin module%3CD26D7503D0.additionalDeclarations preserve=yes
//## end module%3CD26D7503D0.additionalDeclarations


// Class GeoPara 

//## begin GeoPara::s_classType%3CD26D7503D1.attr preserve=no  public: static const std::string {U} "Para"
const std::string GeoPara::s_classType = "Para";
//## end GeoPara::s_classType%3CD26D7503D1.attr

//## begin GeoPara::s_classTypeID%3CD26D7503D2.attr preserve=no  public: static const ShapeType {U} 0x12
const ShapeType GeoPara::s_classTypeID = 0x12;
//## end GeoPara::s_classTypeID%3CD26D7503D2.attr

GeoPara::GeoPara (double XHalfLength, double YHalfLength, double ZHalfLength, double Alpha, double Theta, double Phi)
  //## begin GeoPara::GeoPara%3CD5A8D5005A.hasinit preserve=no
  //## end GeoPara::GeoPara%3CD5A8D5005A.hasinit
  //## begin GeoPara::GeoPara%3CD5A8D5005A.initialization preserve=yes
  :
m_xHalfLength (XHalfLength),
m_yHalfLength (YHalfLength),
m_zHalfLength (ZHalfLength),
m_theta (Theta),
m_alpha (Alpha),
m_phi (Phi)
  //## end GeoPara::GeoPara%3CD5A8D5005A.initialization
{
  //## begin GeoPara::GeoPara%3CD5A8D5005A.body preserve=yes
  //## end GeoPara::GeoPara%3CD5A8D5005A.body
}


GeoPara::~GeoPara()
{
  //## begin GeoPara::~GeoPara%3CD26D7503D0_dest.body preserve=yes
  //## end GeoPara::~GeoPara%3CD26D7503D0_dest.body
}



//## Other Operations (implementation)
double GeoPara::volume () const
{
  //## begin GeoPara::volume%3CD2A6DB00FC.body preserve=yes
  HepGeom::Vector3D<double>
    v0 (1, 0, 0),
    v1 (sin (m_alpha), cos (m_alpha), 0),
    v2 (sin (m_theta) * cos (m_phi), sin (m_theta) * sin (m_phi), cos (m_theta));
  double factor = (v0.cross (v1).dot (v2));

  return 8.0 * factor * m_xHalfLength * m_yHalfLength * m_zHalfLength;
  //## end GeoPara::volume%3CD2A6DB00FC.body
}

const std::string & GeoPara::type () const
{
  //## begin GeoPara::type%3CD2A83C0208.body preserve=yes
  return s_classType;
  //## end GeoPara::type%3CD2A83C0208.body
}

ShapeType GeoPara::typeID () const
{
  //## begin GeoPara::typeID%3CD2A83C0226.body preserve=yes
  return s_classTypeID;
  //## end GeoPara::typeID%3CD2A83C0226.body
}

void GeoPara::exec (GeoShapeAction *action) const
{
  //## begin GeoPara::exec%3DB96A3602F5.body preserve=yes
	action->handlePara(this);
  //## end GeoPara::exec%3DB96A3602F5.body
}

// Additional Declarations
  //## begin GeoPara%3CD26D7503D0.declarations preserve=yes
  //## end GeoPara%3CD26D7503D0.declarations

//## begin module%3CD26D7503D0.epilog preserve=yes
//## end module%3CD26D7503D0.epilog
