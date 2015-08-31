/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D13D8DE0092.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D13D8DE0092.cm

//## begin module%3D13D8DE0092.cp preserve=no
//## end module%3D13D8DE0092.cp

//## Module: GeoPVConstLink%3D13D8DE0092; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPVConstLink.cxx

//## begin module%3D13D8DE0092.additionalIncludes preserve=no
//## end module%3D13D8DE0092.additionalIncludes

//## begin module%3D13D8DE0092.includes preserve=yes
//## end module%3D13D8DE0092.includes

// GeoPVConstLink
#include "GeoModelKernel/GeoPVConstLink.h"
//## begin module%3D13D8DE0092.additionalDeclarations preserve=yes
//## end module%3D13D8DE0092.additionalDeclarations


// Class GeoPVConstLink 

GeoPVConstLink::GeoPVConstLink()
  //## begin GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_const.hasinit preserve=no
  //## end GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_const.hasinit
  //## begin GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_const.initialization preserve=yes
:m_ptr (0)
  //## end GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_const.initialization
{
  //## begin GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_const.body preserve=yes
  //## end GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_const.body
}

GeoPVConstLink::GeoPVConstLink(const GeoPVConstLink &right)
  //## begin GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_copy.hasinit preserve=no
  //## end GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_copy.hasinit
  //## begin GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_copy.initialization preserve=yes
  :
m_ptr (right.m_ptr)
  //## end GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_copy.initialization
{
  //## begin GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_copy.body preserve=yes
  //## end GeoPVConstLink::GeoPVConstLink%3D13D8DE0092_copy.body
}

GeoPVConstLink::GeoPVConstLink (const GeoVPhysVol  *addr)
  //## begin GeoPVConstLink::GeoPVConstLink%3D14059C00B8.hasinit preserve=no
  //## end GeoPVConstLink::GeoPVConstLink%3D14059C00B8.hasinit
  //## begin GeoPVConstLink::GeoPVConstLink%3D14059C00B8.initialization preserve=yes
  :
m_ptr (const_cast < GeoVPhysVol * >(addr))
  //## end GeoPVConstLink::GeoPVConstLink%3D14059C00B8.initialization
{
  //## begin GeoPVConstLink::GeoPVConstLink%3D14059C00B8.body preserve=yes
  //## end GeoPVConstLink::GeoPVConstLink%3D14059C00B8.body
}


GeoPVConstLink::~GeoPVConstLink()
{
  //## begin GeoPVConstLink::~GeoPVConstLink%3D13D8DE0092_dest.body preserve=yes
  //## end GeoPVConstLink::~GeoPVConstLink%3D13D8DE0092_dest.body
}


int GeoPVConstLink::operator==(const GeoPVConstLink &right) const
{
  //## begin GeoPVConstLink::operator==%3D13D8DE0092_eq.body preserve=yes
  return m_ptr == right.m_ptr;
  //## end GeoPVConstLink::operator==%3D13D8DE0092_eq.body
}

int GeoPVConstLink::operator!=(const GeoPVConstLink &right) const
{
  //## begin GeoPVConstLink::operator!=%3D13D8DE0092_neq.body preserve=yes
  return m_ptr != right.m_ptr;
  //## end GeoPVConstLink::operator!=%3D13D8DE0092_neq.body
}


int GeoPVConstLink::operator<(const GeoPVConstLink &right) const
{
  //## begin GeoPVConstLink::operator<%3D13D8DE0092_ls.body preserve=yes
  return m_ptr < right.m_ptr;
  //## end GeoPVConstLink::operator<%3D13D8DE0092_ls.body
}

int GeoPVConstLink::operator>(const GeoPVConstLink &right) const
{
  //## begin GeoPVConstLink::operator>%3D13D8DE0092_gt.body preserve=yes
  return m_ptr > right.m_ptr;
  //## end GeoPVConstLink::operator>%3D13D8DE0092_gt.body
}

int GeoPVConstLink::operator<=(const GeoPVConstLink &right) const
{
  //## begin GeoPVConstLink::operator<=%3D13D8DE0092_lseq.body preserve=yes
  return m_ptr <= right.m_ptr;
  //## end GeoPVConstLink::operator<=%3D13D8DE0092_lseq.body
}

int GeoPVConstLink::operator>=(const GeoPVConstLink &right) const
{
  //## begin GeoPVConstLink::operator>=%3D13D8DE0092_gteq.body preserve=yes
  return m_ptr >= right.m_ptr;
  //## end GeoPVConstLink::operator>=%3D13D8DE0092_gteq.body
}



//## Other Operations (implementation)
const GeoVPhysVol & GeoPVConstLink::operator * () const
{
  //## begin GeoPVConstLink::operator *%3D13DCE90234.body preserve=yes
  return *m_ptr;
  //## end GeoPVConstLink::operator *%3D13DCE90234.body
}

const GeoVPhysVol * GeoPVConstLink::operator -> () const
{
  //## begin GeoPVConstLink::operator->%3D13DD9601ED.body preserve=yes
  return m_ptr;
  //## end GeoPVConstLink::operator->%3D13DD9601ED.body
}

GeoPVConstLink::operator bool () const
{
  //## begin GeoPVConstLink::operator bool%3D140A1E02A1.body preserve=yes
  return m_ptr;
  //## end GeoPVConstLink::operator bool%3D140A1E02A1.body
}

GeoPVConstLink& GeoPVConstLink::operator = (const GeoPVConstLink& right)
{
  //## begin GeoPVConstLink::operator=%3D1DC05000AA.body preserve=yes
  if (this != &right)
    {
      m_ptr = right.m_ptr;
    }
  return *this;
  //## end GeoPVConstLink::operator=%3D1DC05000AA.body
}

// Additional Declarations
  //## begin GeoPVConstLink%3D13D8DE0092.declarations preserve=yes
  //## end GeoPVConstLink%3D13D8DE0092.declarations

//## begin module%3D13D8DE0092.epilog preserve=yes
//## end module%3D13D8DE0092.epilog
