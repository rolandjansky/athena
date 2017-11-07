/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include <stdexcept>

const std::string GeoShapeIntersection::s_classType = "Intersection";
const ShapeType GeoShapeIntersection::s_classTypeID = 0x00;

GeoShapeIntersection::GeoShapeIntersection (const GeoShape* A, const GeoShape* B)
  : m_opA (A)
  , m_opB (B)
{
  m_opA->ref ();
  m_opB->ref ();
}

GeoShapeIntersection::~GeoShapeIntersection()
{
  m_opA->unref ();
  m_opB->unref ();
}

double GeoShapeIntersection::volume () const
{
  GeoPolyhedrizeAction a;
  exec(&a);
  const GeoPolyhedron *poly = a.getPolyhedron();
  double vol = poly->GetVolume ();
  return vol;
}

const std::string & GeoShapeIntersection::type () const
{
  return s_classType;
}

ShapeType GeoShapeIntersection::typeID () const
{
  return s_classTypeID;
}

const GeoShape* GeoShapeIntersection::getOpA () const
{
  return m_opA;
}

const GeoShape* GeoShapeIntersection::getOpB () const
{
  return m_opB;
}

void GeoShapeIntersection::exec (GeoShapeAction *action) const
{
  action->getPath ()->push (this);
  action->handleIntersection (this);
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
