/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPVLink.h"

GeoPVLink::GeoPVLink()
 : GeoPVConstLink (0)
{
}

GeoPVLink::GeoPVLink(const GeoPVLink &right)
  : GeoPVConstLink (right)
{
}

GeoPVLink::GeoPVLink (const GeoVPhysVol  *addr)
  : GeoPVConstLink (addr)
{
}

GeoPVLink::~GeoPVLink()
{
}

GeoVPhysVol & GeoPVLink::operator * ()
{
  return *m_ptr;
}

GeoVPhysVol * GeoPVLink::operator -> ()
{
  return m_ptr;
}

const GeoVPhysVol & GeoPVLink::operator * () const
{
  return *m_ptr;
}

const GeoVPhysVol * GeoPVLink::operator -> () const
{
  return m_ptr;
}

GeoPVLink& GeoPVLink::operator = (const GeoPVLink& right)
{
  if (&right != this)
    {
      m_ptr = right.m_ptr;
    }
  return *this;
}

