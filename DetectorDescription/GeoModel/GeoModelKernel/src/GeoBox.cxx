/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoShapeAction.h"

const std::string GeoBox::s_classType = "Box";
const ShapeType GeoBox::s_classTypeID = 0x10;

GeoBox::GeoBox (double XHalfLength, double YHalfLength, double ZHalfLength)
  : m_xHalfLength (XHalfLength)
  , m_yHalfLength (YHalfLength)
  , m_zHalfLength (ZHalfLength)
{
}

GeoBox::~GeoBox()
{
}

double GeoBox::volume () const
{
  return 8.0 * m_xHalfLength * m_yHalfLength * m_zHalfLength;
}

const std::string & GeoBox::type () const
{
  return s_classType;
}

ShapeType GeoBox::typeID () const
{
  return s_classTypeID;
}

void GeoBox::exec (GeoShapeAction *action) const
{
  action->handleBox(this);
}
