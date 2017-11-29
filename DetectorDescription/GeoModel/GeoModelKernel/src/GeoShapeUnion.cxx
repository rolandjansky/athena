/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include <stdexcept>

const std::string GeoShapeUnion::s_classType = "Union";
const ShapeType GeoShapeUnion::s_classTypeID = 0x01;

GeoShapeUnion::GeoShapeUnion (const GeoShape* A, const GeoShape* B)
  : m_opA (A)
  , m_opB (B)
{
  m_opA->ref ();
  m_opB->ref ();
}

GeoShapeUnion::~GeoShapeUnion()
{
  m_opA->unref ();
  m_opB->unref ();
}

double GeoShapeUnion::volume () const
{
  GeoPolyhedrizeAction a;
  exec (&a);
  const GeoPolyhedron *poly = a.getPolyhedron ();
  double vol = poly->GetVolume ();
  return vol;
}

const std::string & GeoShapeUnion::type () const
{
  return s_classType;
}

ShapeType GeoShapeUnion::typeID () const
{
  return s_classTypeID;
}

const GeoShape* GeoShapeUnion::getOpA () const
{
  return m_opA;
}

const GeoShape* GeoShapeUnion::getOpB () const
{
  return m_opB;
}

void GeoShapeUnion::exec (GeoShapeAction *action) const
{
  action->getPath ()->push (this);
  action->handleUnion (this);
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
    m_opA->exec(action);
    if (action->shouldTerminate ())
	{
      action->getPath ()->pop ();
      return;
	}
    m_opB->exec(action);
    if (action->shouldTerminate ())
	{
      action->getPath ()->pop ();
      return;
    }
  }
  action->getPath()->pop();
}

