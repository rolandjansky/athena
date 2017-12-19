/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPVConstLink.h"

GeoPVConstLink::GeoPVConstLink()
 : m_ptr (0)
{
}

GeoPVConstLink::GeoPVConstLink(const GeoPVConstLink &right)
  : m_ptr (right.m_ptr)
{
}

GeoPVConstLink::GeoPVConstLink (const GeoVPhysVol  *addr)
  : m_ptr (const_cast < GeoVPhysVol * >(addr))
{
}

GeoPVConstLink::~GeoPVConstLink()
{
}


int GeoPVConstLink::operator==(const GeoPVConstLink &right) const
{
  return m_ptr == right.m_ptr;
}

int GeoPVConstLink::operator!=(const GeoPVConstLink &right) const
{
  return m_ptr != right.m_ptr;
}

int GeoPVConstLink::operator<(const GeoPVConstLink &right) const
{
  return m_ptr < right.m_ptr;
}

int GeoPVConstLink::operator>(const GeoPVConstLink &right) const
{
  return m_ptr > right.m_ptr;
}

int GeoPVConstLink::operator<=(const GeoPVConstLink &right) const
{
  return m_ptr <= right.m_ptr;
}

int GeoPVConstLink::operator>=(const GeoPVConstLink &right) const
{
  return m_ptr >= right.m_ptr;
}

const GeoVPhysVol & GeoPVConstLink::operator * () const
{
  return *m_ptr;
}

const GeoVPhysVol * GeoPVConstLink::operator -> () const
{
  return m_ptr;
}

GeoPVConstLink::operator bool () const
{
  return m_ptr;
}

GeoPVConstLink& GeoPVConstLink::operator = (const GeoPVConstLink& right)
{
  if (this != &right)
    {
      m_ptr = right.m_ptr;
    }
  return *this;
}
