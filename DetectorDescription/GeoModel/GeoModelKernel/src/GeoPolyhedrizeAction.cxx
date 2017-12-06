/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPara.h"

GeoPolyhedrizeAction::GeoPolyhedrizeAction()
  : m_polyhedron(nullptr)
{
  setDepthLimit(0);
}

GeoPolyhedrizeAction::~GeoPolyhedrizeAction()
{
  delete m_polyhedron;
}

void GeoPolyhedrizeAction::handleShift (const GeoShapeShift *shift)
{
  shift->getOp()->exec(this); 
  m_polyhedron->Transform (shift->getX().getRotation (), shift->getX().getTranslation());
}

void GeoPolyhedrizeAction::handleUnion (const GeoShapeUnion *unio)
{
  GeoPolyhedrizeAction auxA,auxB;
  unio->getOpA()->exec(&auxA);
  unio->getOpB()->exec(&auxB);
  m_polyhedron = new GeoPolyhedron(auxA.getPolyhedron()->add(*auxB.getPolyhedron()));
}

void GeoPolyhedrizeAction::handleIntersection (const GeoShapeIntersection *isect)
{
  GeoPolyhedrizeAction auxA,auxB;
  isect->getOpA()->exec(&auxA);
  isect->getOpB()->exec(&auxB);
  m_polyhedron=new GeoPolyhedron(auxA.getPolyhedron()->intersect(*auxB.getPolyhedron()));
}

void GeoPolyhedrizeAction::handleSubtraction (const GeoShapeSubtraction *subtract)
{
  GeoPolyhedrizeAction auxA,auxB;
  subtract->getOpA()->exec(&auxA);
  subtract->getOpB()->exec(&auxB);
  m_polyhedron=new GeoPolyhedron(auxA.getPolyhedron()->subtract(*auxB.getPolyhedron()));
}

void GeoPolyhedrizeAction::handleBox (const GeoBox *box)
{
  m_polyhedron=new GeoPolyhedronBox (box->getXHalfLength(),
				     box->getYHalfLength(),
				     box->getZHalfLength());
}

void GeoPolyhedrizeAction::handleCons (const GeoCons *cons)
{
  m_polyhedron = new GeoPolyhedronCons (cons->getRMin1(), 
					cons->getRMax1(), 
					cons->getRMin2(), 
					cons->getRMax2(), 
					cons->getDZ(),
					cons->getSPhi(), 
					cons->getDPhi());
}

void GeoPolyhedrizeAction::handlePara (const GeoPara *para)
{
  m_polyhedron=new GeoPolyhedronPara(para->getXHalfLength(),
				     para->getYHalfLength(),
				     para->getZHalfLength(),
				     para->getAlpha(),
				     para->getTheta(),
				     para->getPhi());
}

void GeoPolyhedrizeAction::handlePcon (const GeoPcon *pcon)
{
  double *z = new double[pcon->getNPlanes ()];
  double *rmn = new double[pcon->getNPlanes ()];
  double *rmx = new double[pcon->getNPlanes ()];

  for (unsigned int s = 0; s < pcon->getNPlanes (); s++)
    {
      z[s] = pcon->getZPlane (s);
      rmn[s] = pcon->getRMinPlane (s);
      rmx[s] = pcon->getRMaxPlane (s);
    }
  m_polyhedron = new GeoPolyhedronPcon (pcon->getSPhi(), pcon->getDPhi(), pcon->getNPlanes (), z, rmn, rmx);

  delete[]z;
  delete[]rmn;
  delete[]rmx;
}

void GeoPolyhedrizeAction::handlePgon (const GeoPgon *pgon)
{
  double *z   = new double[pgon->getNPlanes ()];
  double *rmn = new double[pgon->getNPlanes ()];
  double *rmx = new double[pgon->getNPlanes ()];

  for (unsigned int s = 0; s < pgon->getNPlanes (); s++)
    {
      z[s]   = pgon->getZPlane (s);
      rmn[s] = pgon->getRMinPlane (s);
      rmx[s] = pgon->getRMaxPlane (s);
    }
  m_polyhedron = new GeoPolyhedronPgon (pgon->getSPhi(), pgon->getDPhi(), pgon->getNSides(), pgon->getNPlanes (), z, rmn, rmx);

  delete[]z;
  delete[]rmn;
  delete[]rmx;
}

void GeoPolyhedrizeAction::handleTrap (const GeoTrap *trap)
{
  m_polyhedron = new GeoPolyhedronTrap (trap->getZHalfLength(),
					trap->getTheta(),
					trap->getPhi(),
					trap->getDydzn(),
					trap->getDxdyndzn(),
					trap->getDxdypdzn(), 0,
					trap->getDydzp(), 
					trap->getDxdyndzp(), 
					trap->getDxdypdzp(),0);
}

void GeoPolyhedrizeAction::handleTrd (const GeoTrd *trd)
{
  m_polyhedron = new GeoPolyhedronTrd2 (trd->getXHalfLength1(),
					trd->getXHalfLength2(),
					trd->getYHalfLength1(),
					trd->getYHalfLength2(),
					trd->getZHalfLength());
}

void GeoPolyhedrizeAction::handleTube (const GeoTube *tube)
{
  m_polyhedron = new GeoPolyhedronTube (tube->getRMin(),
					tube->getRMax(),
					tube->getZHalfLength());
}

void GeoPolyhedrizeAction::handleTubs (const GeoTubs *tubs)
{
  m_polyhedron= new GeoPolyhedronTubs (tubs->getRMin(), 
				       tubs->getRMax(), 
				       tubs->getZHalfLength(),
				       tubs->getSPhi(),
				       tubs->getDPhi());
}

const GeoPolyhedron * GeoPolyhedrizeAction::getPolyhedron () const
{
  return m_polyhedron;
}
