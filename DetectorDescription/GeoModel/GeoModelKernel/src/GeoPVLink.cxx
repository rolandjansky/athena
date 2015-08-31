/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D13DDFD02C8.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D13DDFD02C8.cm

//## begin module%3D13DDFD02C8.cp preserve=no
//## end module%3D13DDFD02C8.cp

//## Module: GeoPVLink%3D13DDFD02C8; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPVLink.cxx

//## begin module%3D13DDFD02C8.additionalIncludes preserve=no
//## end module%3D13DDFD02C8.additionalIncludes

//## begin module%3D13DDFD02C8.includes preserve=yes
//## end module%3D13DDFD02C8.includes

// GeoPVLink
#include "GeoModelKernel/GeoPVLink.h"
//## begin module%3D13DDFD02C8.additionalDeclarations preserve=yes
//## end module%3D13DDFD02C8.additionalDeclarations


// Class GeoPVLink 

GeoPVLink::GeoPVLink()
  //## begin GeoPVLink::GeoPVLink%3D13DDFD02C8_const.hasinit preserve=no
  //## end GeoPVLink::GeoPVLink%3D13DDFD02C8_const.hasinit
  //## begin GeoPVLink::GeoPVLink%3D13DDFD02C8_const.initialization preserve=yes
:GeoPVConstLink (0)
  //## end GeoPVLink::GeoPVLink%3D13DDFD02C8_const.initialization
{
  //## begin GeoPVLink::GeoPVLink%3D13DDFD02C8_const.body preserve=yes
  //## end GeoPVLink::GeoPVLink%3D13DDFD02C8_const.body
}

GeoPVLink::GeoPVLink(const GeoPVLink &right)
  //## begin GeoPVLink::GeoPVLink%3D13DDFD02C8_copy.hasinit preserve=no
  //## end GeoPVLink::GeoPVLink%3D13DDFD02C8_copy.hasinit
  //## begin GeoPVLink::GeoPVLink%3D13DDFD02C8_copy.initialization preserve=yes
:
GeoPVConstLink (right)
  //## end GeoPVLink::GeoPVLink%3D13DDFD02C8_copy.initialization
{
  //## begin GeoPVLink::GeoPVLink%3D13DDFD02C8_copy.body preserve=yes
  //## end GeoPVLink::GeoPVLink%3D13DDFD02C8_copy.body
}

GeoPVLink::GeoPVLink (const GeoVPhysVol  *addr)
  //## begin GeoPVLink::GeoPVLink%3D14067D024B.hasinit preserve=no
  //## end GeoPVLink::GeoPVLink%3D14067D024B.hasinit
  //## begin GeoPVLink::GeoPVLink%3D14067D024B.initialization preserve=yes
  :
GeoPVConstLink (addr)
  //## end GeoPVLink::GeoPVLink%3D14067D024B.initialization
{
  //## begin GeoPVLink::GeoPVLink%3D14067D024B.body preserve=yes
  //## end GeoPVLink::GeoPVLink%3D14067D024B.body
}


GeoPVLink::~GeoPVLink()
{
  //## begin GeoPVLink::~GeoPVLink%3D13DDFD02C8_dest.body preserve=yes
  //## end GeoPVLink::~GeoPVLink%3D13DDFD02C8_dest.body
}



//## Other Operations (implementation)
GeoVPhysVol & GeoPVLink::operator * ()
{
  //## begin GeoPVLink::operator *%3D13DE1B0311.body preserve=yes
  return *m_ptr;
  //## end GeoPVLink::operator *%3D13DE1B0311.body
}

GeoVPhysVol * GeoPVLink::operator -> ()
{
  //## begin GeoPVLink::operator->%3D13DE1B032F.body preserve=yes
  return m_ptr;
  //## end GeoPVLink::operator->%3D13DE1B032F.body
}

const GeoVPhysVol & GeoPVLink::operator * () const
{
  //## begin GeoPVLink::operator *%3D140E0001EC.body preserve=yes
  return *m_ptr;
  //## end GeoPVLink::operator *%3D140E0001EC.body
}

const GeoVPhysVol * GeoPVLink::operator -> () const
{
  //## begin GeoPVLink::operator->%3D140E000214.body preserve=yes
  return m_ptr;
  //## end GeoPVLink::operator->%3D140E000214.body
}

GeoPVLink& GeoPVLink::operator = (const GeoPVLink& right)
{
  //## begin GeoPVLink::operator=%3D1DC1D202EA.body preserve=yes
  if (&right != this)
    {
      m_ptr = right.m_ptr;
    }
  return *this;
  //## end GeoPVLink::operator=%3D1DC1D202EA.body
}

// Additional Declarations
  //## begin GeoPVLink%3D13DDFD02C8.declarations preserve=yes
  //## end GeoPVLink%3D13DDFD02C8.declarations

//## begin module%3D13DDFD02C8.epilog preserve=yes
//## end module%3D13DDFD02C8.epilog
