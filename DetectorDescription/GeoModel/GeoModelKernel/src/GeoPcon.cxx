/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD26DBC00BB.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD26DBC00BB.cm

//## begin module%3CD26DBC00BB.cp preserve=no
//## end module%3CD26DBC00BB.cp

//## Module: GeoPcon%3CD26DBC00BB; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPcon.cxx

//## begin module%3CD26DBC00BB.additionalIncludes preserve=no
//## end module%3CD26DBC00BB.additionalIncludes

//## begin module%3CD26DBC00BB.includes preserve=yes
#include <cmath>
#include <stdexcept>
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD26DBC00BB.includes

// GeoPcon
#include "GeoModelKernel/GeoPcon.h"
//## begin module%3CD26DBC00BB.additionalDeclarations preserve=yes
//## end module%3CD26DBC00BB.additionalDeclarations


// Class GeoPcon 

//## begin GeoPcon::s_classType%3CD26DBC00BC.attr preserve=no  public: static const std::string {U} "Pcon"
const std::string GeoPcon::s_classType = "Pcon";
//## end GeoPcon::s_classType%3CD26DBC00BC.attr

//## begin GeoPcon::s_classTypeID%3CD26DBC00BD.attr preserve=no  public: static const ShapeType {U} 0x13
const ShapeType GeoPcon::s_classTypeID = 0x13;
//## end GeoPcon::s_classTypeID%3CD26DBC00BD.attr

GeoPcon::GeoPcon (double SPhi, double DPhi)
  //## begin GeoPcon::GeoPcon%3CD5A48D037F.hasinit preserve=no
  //## end GeoPcon::GeoPcon%3CD5A48D037F.hasinit
  //## begin GeoPcon::GeoPcon%3CD5A48D037F.initialization preserve=yes
  :
m_sPhi (SPhi),
m_dPhi (DPhi)
  //## end GeoPcon::GeoPcon%3CD5A48D037F.initialization
{
  //## begin GeoPcon::GeoPcon%3CD5A48D037F.body preserve=yes
  //## end GeoPcon::GeoPcon%3CD5A48D037F.body
}


GeoPcon::~GeoPcon()
{
  //## begin GeoPcon::~GeoPcon%3CD26DBC00BB_dest.body preserve=yes
  //## end GeoPcon::~GeoPcon%3CD26DBC00BB_dest.body
}



//## Other Operations (implementation)
double GeoPcon::volume () const
{
  //## begin GeoPcon::volume%3CD2A6E0030C.body preserve=yes
  if (!isValid ())
    throw std::runtime_error ("Volume requested for incomplete polycone");
  double v = 0;
  for (size_t s = 0; s < getNPlanes () - 1; s++)
    {
      double fDz = fabs (getZPlane (s + 1) - getZPlane (s)) / 2.0;
      double fRmin1 = getRMinPlane (s + 1);
      double fRmin2 = getRMinPlane (s);
      double fRmax1 = getRMaxPlane (s + 1);
      double fRmax2 = getRMaxPlane (s);
      v +=
	(m_dPhi * (1./3)) * fDz * (fRmax1 * fRmax1 + fRmax2 * fRmax2 +
                                   fRmax1 * fRmax2 - fRmin1 * fRmin1 -
                                   fRmin2 * fRmin2 - fRmin1 * fRmin2);
    }
  return v;

  //## end GeoPcon::volume%3CD2A6E0030C.body
}

const std::string & GeoPcon::type () const
{
  //## begin GeoPcon::type%3CD2A842029D.body preserve=yes
  return s_classType;
  //## end GeoPcon::type%3CD2A842029D.body
}

ShapeType GeoPcon::typeID () const
{
  //## begin GeoPcon::typeID%3CD2A84202BB.body preserve=yes
  return s_classTypeID;
  //## end GeoPcon::typeID%3CD2A84202BB.body
}

void GeoPcon::addPlane (double ZPlane, double RMinPlane, double RMaxPlane)
{
  //## begin GeoPcon::addPlane%3CD59FF902C6.body preserve=yes
  m_zPlane.push_back (ZPlane);
  m_rMinPlane.push_back (RMinPlane);
  m_rMaxPlane.push_back (RMaxPlane);
  //## end GeoPcon::addPlane%3CD59FF902C6.body
}

void GeoPcon::exec (GeoShapeAction *action) const
{
  //## begin GeoPcon::exec%3DB96A4901F8.body preserve=yes
	action->handlePcon(this);
  //## end GeoPcon::exec%3DB96A4901F8.body
}

// Additional Declarations
  //## begin GeoPcon%3CD26DBC00BB.declarations preserve=yes
  //## end GeoPcon%3CD26DBC00BB.declarations

//## begin module%3CD26DBC00BB.epilog preserve=yes
//## end module%3CD26DBC00BB.epilog
