/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeAction.h"

const std::string GeoShapeShift::s_classType = "Shift";
const ShapeType GeoShapeShift::s_classTypeID = 0x03;

GeoShapeShift::GeoShapeShift (const GeoShape* A, const HepGeom::Transform3D &X)
  : m_op (A)
  , m_shift (X)
{
  m_op->ref ();
}

GeoShapeShift::~GeoShapeShift()
{
  m_op->unref ();
}

double GeoShapeShift::volume () const
{
  return m_op->volume ();
}

const std::string & GeoShapeShift::type () const
{
  return s_classType;
}

ShapeType GeoShapeShift::typeID () const
{
  return s_classTypeID;
}

const GeoShape* GeoShapeShift::getOp () const
{
  return m_op;
}

const HepGeom::Transform3D & GeoShapeShift::getX () const
{
  return m_shift;
}

void GeoShapeShift::exec (GeoShapeAction *action) const
{
  action->getPath ()->push (this);
  action->handleShift (this);
  if (action->shouldTerminate ())
    {
      action->getPath ()->pop ();
      return;
    }
  
  if (action->getDepthLimit ().isValid ()
      && action->getPath ()->getLength () > action->getDepthLimit ())
    {
    }

  else 
	{
    m_op->exec(action);
    if (action->shouldTerminate ())
	{
      action->getPath ()->pop ();
      return;
	}
  }
  action->getPath()->pop();
}
