/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A40B0223.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A40B0223.cm

//## begin module%3CD2A40B0223.cp preserve=no
//## end module%3CD2A40B0223.cp

//## Module: GeoPgon%3CD2A40B0223; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPgon.cxx

//## begin module%3CD2A40B0223.additionalIncludes preserve=no
//## end module%3CD2A40B0223.additionalIncludes

//## begin module%3CD2A40B0223.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD2A40B0223.includes

// GeoPgon
#include "GeoModelKernel/GeoPgon.h"
//## begin module%3CD2A40B0223.additionalDeclarations preserve=yes
#include <cmath>
#include <stdexcept>
//## end module%3CD2A40B0223.additionalDeclarations


// Class GeoPgon 

//## begin GeoPgon::s_classType%3CD2A40B022D.attr preserve=no  public: static const std::string {U} "Pgon"
const std::string GeoPgon::s_classType = "Pgon";
//## end GeoPgon::s_classType%3CD2A40B022D.attr

//## begin GeoPgon::s_classTypeID%3CD2A40B022E.attr preserve=no  public: static const ShapeType {U} 0x14
const ShapeType GeoPgon::s_classTypeID = 0x14;
//## end GeoPgon::s_classTypeID%3CD2A40B022E.attr

GeoPgon::GeoPgon (double SPhi, double DPhi, unsigned int NSides)
  //## begin GeoPgon::GeoPgon%3CD5AA3E029D.hasinit preserve=no
  //## end GeoPgon::GeoPgon%3CD5AA3E029D.hasinit
  //## begin GeoPgon::GeoPgon%3CD5AA3E029D.initialization preserve=yes
  :
m_sPhi (SPhi),
m_dPhi (DPhi),
m_nSides (NSides)
  //## end GeoPgon::GeoPgon%3CD5AA3E029D.initialization
{
  //## begin GeoPgon::GeoPgon%3CD5AA3E029D.body preserve=yes
  //## end GeoPgon::GeoPgon%3CD5AA3E029D.body
}


GeoPgon::~GeoPgon()
{
  //## begin GeoPgon::~GeoPgon%3CD2A40B0223_dest.body preserve=yes
  //## end GeoPgon::~GeoPgon%3CD2A40B0223_dest.body
}



//## Other Operations (implementation)
double GeoPgon::volume () const
{
  //## begin GeoPgon::volume%3CD2A6E600EE.body preserve=yes
  if (!isValid ())
    throw std::runtime_error ("Volume requested for incomplete polygon");
  double v = 0;
  int sides = getNSides ();
  double alpha = m_dPhi/sides;
  double sinAlpha = sin(alpha);
  
  for (size_t s = 0; s < getNPlanes () - 1; s++) {
    double z2 = getZPlane (s);;
    double z1 = getZPlane (s + 1);;
    double fRmin1 = getRMinPlane (s + 1);
    double fRmin2 = getRMinPlane (s);
    double fRmax1 = getRMaxPlane (s + 1);
    double fRmax2 = getRMaxPlane (s);

    double b1 = (fRmax1 - fRmax2)/(z1 - z2);
    double b2 = (fRmin1 - fRmin2)/(z1 - z2);

    double a1 = fRmax2  - b1 * z2;
    double a2 = fRmin2  - b2 * z2;

    //v+=fabs((a1*a1-a2*a2)*(z1-z2) + (a1*b1-a2*b2)*(z1*z1-z2*z2) + (b1*b1-b2*b2)*(z1*z1*z1-z2*z2*z2)/3.);
    // Equivalent which should be less sensitive to numerical precision errors:
    v += fabs(z1 - z2) * ((a1 - a2) * (a1 + a2)  + 
			  (a1*b1 - a2*b2) * (z1 + z2) +
			  (b1 - b2) * (b1 + b2) * (z1*z1+z2*z2+z1*z2)*(1./3));  
  } 
  v *=  0.5 * sides * sinAlpha;
  return v;
  //## end GeoPgon::volume%3CD2A6E600EE.body
}

const std::string & GeoPgon::type () const
{
  //## begin GeoPgon::type%3CD2A8480219.body preserve=yes
  return s_classType;
  //## end GeoPgon::type%3CD2A8480219.body
}

ShapeType GeoPgon::typeID () const
{
  //## begin GeoPgon::typeID%3CD2A8480237.body preserve=yes
  return s_classTypeID;
  //## end GeoPgon::typeID%3CD2A8480237.body
}

void GeoPgon::addPlane (double ZPlane, double RMinPlane, double RMaxPlane)
{
  //## begin GeoPgon::addPlane%3CD5AA3E0243.body preserve=yes
  m_zPlane.push_back (ZPlane);
  m_rMinPlane.push_back (RMinPlane);
  m_rMaxPlane.push_back (RMaxPlane);
  //## end GeoPgon::addPlane%3CD5AA3E0243.body
}

void GeoPgon::exec (GeoShapeAction *action) const
{
  //## begin GeoPgon::exec%3DB96A6E00B0.body preserve=yes
  action->handlePgon(this);
  //## end GeoPgon::exec%3DB96A6E00B0.body
}

// Additional Declarations
  //## begin GeoPgon%3CD2A40B0223.declarations preserve=yes
  //## end GeoPgon%3CD2A40B0223.declarations

//## begin module%3CD2A40B0223.epilog preserve=yes
//## end module%3CD2A40B0223.epilog
