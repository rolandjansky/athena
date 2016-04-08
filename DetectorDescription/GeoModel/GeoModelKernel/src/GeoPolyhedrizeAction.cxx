/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3DCD28730322.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DCD28730322.cm

//## begin module%3DCD28730322.cp preserve=no
//## end module%3DCD28730322.cp

//## Module: GeoPolyhedrizeAction%3DCD28730322; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPolyhedrizeAction.cxx

//## begin module%3DCD28730322.additionalIncludes preserve=no
//## end module%3DCD28730322.additionalIncludes

//## begin module%3DCD28730322.includes preserve=yes
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

//## end module%3DCD28730322.includes

// GeoPolyhedrizeAction
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
//## begin module%3DCD28730322.additionalDeclarations preserve=yes
#include "GeoModelKernel/GeoPolyhedron.h"
//## end module%3DCD28730322.additionalDeclarations


// Class GeoPolyhedrizeAction 

GeoPolyhedrizeAction::GeoPolyhedrizeAction()
  //## begin GeoPolyhedrizeAction::GeoPolyhedrizeAction%3DCD28730322_const.hasinit preserve=no
      : m_polyhedron(NULL)
  //## end GeoPolyhedrizeAction::GeoPolyhedrizeAction%3DCD28730322_const.hasinit
  //## begin GeoPolyhedrizeAction::GeoPolyhedrizeAction%3DCD28730322_const.initialization preserve=yes
  //## end GeoPolyhedrizeAction::GeoPolyhedrizeAction%3DCD28730322_const.initialization
{
  //## begin GeoPolyhedrizeAction::GeoPolyhedrizeAction%3DCD28730322_const.body preserve=yes
	setDepthLimit(0);
  //## end GeoPolyhedrizeAction::GeoPolyhedrizeAction%3DCD28730322_const.body
}


GeoPolyhedrizeAction::~GeoPolyhedrizeAction()
{
  //## begin GeoPolyhedrizeAction::~GeoPolyhedrizeAction%3DCD28730322_dest.body preserve=yes
    delete m_polyhedron;
  //## end GeoPolyhedrizeAction::~GeoPolyhedrizeAction%3DCD28730322_dest.body
}



//## Other Operations (implementation)
void GeoPolyhedrizeAction::handleShift (const GeoShapeShift *shift)
{
  //## begin GeoPolyhedrizeAction::handleShift%3DCD2D500015.body preserve=yes
  shift->getOp()->exec(this); 
  m_polyhedron->Transform (shift->getX().getRotation (), shift->getX().getTranslation());
  //## end GeoPolyhedrizeAction::handleShift%3DCD2D500015.body
}

void GeoPolyhedrizeAction::handleUnion (const GeoShapeUnion *unio)
{
  //## begin GeoPolyhedrizeAction::handleUnion%3DCD2D500033.body preserve=yes
  GeoPolyhedrizeAction auxA,auxB;
  unio->getOpA()->exec(&auxA);
  unio->getOpB()->exec(&auxB);
  m_polyhedron = new GeoPolyhedron(auxA.getPolyhedron()->add(*auxB.getPolyhedron()));
  //## end GeoPolyhedrizeAction::handleUnion%3DCD2D500033.body
}

void GeoPolyhedrizeAction::handleIntersection (const GeoShapeIntersection *isect)
{
  //## begin GeoPolyhedrizeAction::handleIntersection%3DCD2D50005B.body preserve=yes
	GeoPolyhedrizeAction auxA,auxB;
	isect->getOpA()->exec(&auxA);
	isect->getOpB()->exec(&auxB);
	m_polyhedron=new GeoPolyhedron(auxA.getPolyhedron()->intersect(*auxB.getPolyhedron()));
  //## end GeoPolyhedrizeAction::handleIntersection%3DCD2D50005B.body
}

void GeoPolyhedrizeAction::handleSubtraction (const GeoShapeSubtraction *subtract)
{
  //## begin GeoPolyhedrizeAction::handleSubtraction%3DCD2D500079.body preserve=yes
    GeoPolyhedrizeAction auxA,auxB;
	subtract->getOpA()->exec(&auxA);
	subtract->getOpB()->exec(&auxB);
	m_polyhedron=new GeoPolyhedron(auxA.getPolyhedron()->subtract(*auxB.getPolyhedron()));
  //## end GeoPolyhedrizeAction::handleSubtraction%3DCD2D500079.body
}

void GeoPolyhedrizeAction::handleBox (const GeoBox *box)
{
  //## begin GeoPolyhedrizeAction::handleBox%3DCD2D500097.body preserve=yes
	 m_polyhedron=new GeoPolyhedronBox (box->getXHalfLength(),
									   box->getYHalfLength(),
									   box->getZHalfLength());
  //## end GeoPolyhedrizeAction::handleBox%3DCD2D500097.body
}

void GeoPolyhedrizeAction::handleCons (const GeoCons *cons)
{
  //## begin GeoPolyhedrizeAction::handleCons%3DCD2D5000B5.body preserve=yes
	  m_polyhedron = new GeoPolyhedronCons (cons->getRMin1(), 
										   cons->getRMax1(), 
										   cons->getRMin2(), 
										   cons->getRMax2(), 
										   cons->getDZ(),
										   cons->getSPhi(), 
										   cons->getDPhi());
  //## end GeoPolyhedrizeAction::handleCons%3DCD2D5000B5.body
}

void GeoPolyhedrizeAction::handlePara (const GeoPara *para)
{
  //## begin GeoPolyhedrizeAction::handlePara%3DCD2D5000DD.body preserve=yes
    m_polyhedron=new GeoPolyhedronPara(para->getXHalfLength(),
									  para->getYHalfLength(),
									  para->getZHalfLength(),
									  para->getAlpha(),
									  para->getTheta(),
									  para->getPhi());
  //## end GeoPolyhedrizeAction::handlePara%3DCD2D5000DD.body
}

void GeoPolyhedrizeAction::handlePcon (const GeoPcon *pcon)
{
  //## begin GeoPolyhedrizeAction::handlePcon%3DCD2D5000FB.body preserve=yes
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
  //## end GeoPolyhedrizeAction::handlePcon%3DCD2D5000FB.body
}

void GeoPolyhedrizeAction::handlePgon (const GeoPgon *pgon)
{
  //## begin GeoPolyhedrizeAction::handlePgon%3DCD2D500119.body preserve=yes
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
  //## end GeoPolyhedrizeAction::handlePgon%3DCD2D500119.body
}

void GeoPolyhedrizeAction::handleTrap (const GeoTrap *trap)
{
  //## begin GeoPolyhedrizeAction::handleTrap%3DCD2D500137.body preserve=yes
	m_polyhedron = new GeoPolyhedronTrap (trap->getZHalfLength(),
					                     trap->getTheta(),
					                     trap->getPhi(),
					                     trap->getDydzn(),
					                     trap->getDxdyndzn(),
					                     trap->getDxdypdzn(), 0,
					                     trap->getDydzp(), 
						                 trap->getDxdyndzp(), 
						                 trap->getDxdypdzp(),0);
  //## end GeoPolyhedrizeAction::handleTrap%3DCD2D500137.body
}

void GeoPolyhedrizeAction::handleTrd (const GeoTrd *trd)
{
  //## begin GeoPolyhedrizeAction::handleTrd%3DCD2D50015F.body preserve=yes
	m_polyhedron = new GeoPolyhedronTrd2 (trd->getXHalfLength1(),
										 trd->getXHalfLength2(),
					                     trd->getYHalfLength1(),
										 trd->getYHalfLength2(),
										 trd->getZHalfLength());
  //## end GeoPolyhedrizeAction::handleTrd%3DCD2D50015F.body
}

void GeoPolyhedrizeAction::handleTube (const GeoTube *tube)
{
  //## begin GeoPolyhedrizeAction::handleTube%3DCD2D50017D.body preserve=yes
	m_polyhedron = new GeoPolyhedronTube (tube->getRMin(),
		                                 tube->getRMax(),
										 tube->getZHalfLength());
  //## end GeoPolyhedrizeAction::handleTube%3DCD2D50017D.body
}

void GeoPolyhedrizeAction::handleTubs (const GeoTubs *tubs)
{
  //## begin GeoPolyhedrizeAction::handleTubs%3DCD2D50019B.body preserve=yes
	m_polyhedron=
    new GeoPolyhedronTubs (tubs->getRMin(), 
	                       tubs->getRMax(), 
						   tubs->getZHalfLength(),
						   tubs->getSPhi(),
						   tubs->getDPhi());
  //## end GeoPolyhedrizeAction::handleTubs%3DCD2D50019B.body
}

const GeoPolyhedron * GeoPolyhedrizeAction::getPolyhedron () const
{
  //## begin GeoPolyhedrizeAction::getPolyhedron%3DCD2E3C00E6.body preserve=yes
	return m_polyhedron;
  //## end GeoPolyhedrizeAction::getPolyhedron%3DCD2E3C00E6.body
}

// Additional Declarations
  //## begin GeoPolyhedrizeAction%3DCD28730322.declarations preserve=yes
  //## end GeoPolyhedrizeAction%3DCD28730322.declarations

//## begin module%3DCD28730322.epilog preserve=yes
//## end module%3DCD28730322.epilog
