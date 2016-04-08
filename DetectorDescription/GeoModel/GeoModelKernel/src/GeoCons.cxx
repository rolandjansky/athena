/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD26BFA00BE.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD26BFA00BE.cm

//## begin module%3CD26BFA00BE.cp preserve=no
//## end module%3CD26BFA00BE.cp

//## Module: GeoCons%3CD26BFA00BE; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoCons.cxx

//## begin module%3CD26BFA00BE.additionalIncludes preserve=no
//## end module%3CD26BFA00BE.additionalIncludes

//## begin module%3CD26BFA00BE.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3CD26BFA00BE.includes

// GeoCons
#include "GeoModelKernel/GeoCons.h"
//## begin module%3CD26BFA00BE.additionalDeclarations preserve=yes
//## end module%3CD26BFA00BE.additionalDeclarations


// Class GeoCons 

//## begin GeoCons::s_classType%3CD26BFA00BF.attr preserve=no  public: static const std::string {U} "Cons"
const std::string GeoCons::s_classType = "Cons";
//## end GeoCons::s_classType%3CD26BFA00BF.attr

//## begin GeoCons::s_classTypeID%3CD26BFA00C0.attr preserve=no  public: static const ShapeType {U} 0x11
const ShapeType GeoCons::s_classTypeID = 0x11;
//## end GeoCons::s_classTypeID%3CD26BFA00C0.attr

GeoCons::GeoCons (double RMin1, double RMin2, double RMax1, double RMax2, double DZ, double SPhi, double DPhi)
  //## begin GeoCons::GeoCons%3CD58C08006B.hasinit preserve=no
  //## end GeoCons::GeoCons%3CD58C08006B.hasinit
  //## begin GeoCons::GeoCons%3CD58C08006B.initialization preserve=yes
  :
m_rMin1 (RMin1),
m_rMin2 (RMin2),
m_rMax1 (RMax1),
m_rMax2 (RMax2),
m_dZ (DZ),
m_sPhi (SPhi),
m_dPhi (DPhi)
  //## end GeoCons::GeoCons%3CD58C08006B.initialization
{
  //## begin GeoCons::GeoCons%3CD58C08006B.body preserve=yes
  //## end GeoCons::GeoCons%3CD58C08006B.body
}


GeoCons::~GeoCons()
{
  //## begin GeoCons::~GeoCons%3CD26BFA00BE_dest.body preserve=yes
  //## end GeoCons::~GeoCons%3CD26BFA00BE_dest.body
}



//## Other Operations (implementation)
double GeoCons::volume () const
{
  //## begin GeoCons::volume%3CD2A6BC0134.body preserve=yes
  return (m_dPhi * (1./3)) * m_dZ * (m_rMax1 * m_rMax1 + m_rMax2 * m_rMax2 + m_rMax1 * m_rMax2
                                 - m_rMin1 * m_rMin1 - m_rMin2 * m_rMin2 -
                                 m_rMin1 * m_rMin2);

  //## end GeoCons::volume%3CD2A6BC0134.body
}

const std::string & GeoCons::type () const
{
  //## begin GeoCons::type%3CD2A83400A8.body preserve=yes
  return s_classType;
  //## end GeoCons::type%3CD2A83400A8.body
}

ShapeType GeoCons::typeID () const
{
  //## begin GeoCons::typeID%3CD2A83400BC.body preserve=yes
  return s_classTypeID;
  //## end GeoCons::typeID%3CD2A83400BC.body
}

void GeoCons::exec (GeoShapeAction *action) const
{
  //## begin GeoCons::exec%3DB96A2B0118.body preserve=yes
	action->handleCons(this);
  //## end GeoCons::exec%3DB96A2B0118.body
}

// Additional Declarations
  //## begin GeoCons%3CD26BFA00BE.declarations preserve=yes
  //## end GeoCons%3CD26BFA00BE.declarations

//## begin module%3CD26BFA00BE.epilog preserve=yes
//## end module%3CD26BFA00BE.epilog
