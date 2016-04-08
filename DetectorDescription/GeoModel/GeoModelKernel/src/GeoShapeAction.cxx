/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3DCD269E0092.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DCD269E0092.cm

//## begin module%3DCD269E0092.cp preserve=no
//## end module%3DCD269E0092.cp

//## Module: GeoShapeAction%3DCD269E0092; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeAction.cxx

//## begin module%3DCD269E0092.additionalIncludes preserve=no
//## end module%3DCD269E0092.additionalIncludes

//## begin module%3DCD269E0092.includes preserve=yes
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
//## end module%3DCD269E0092.includes

#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTessellatedSolid.h"
#include "GeoModelKernel/GeoEllipticalTube.h"
#include "GeoModelKernel/GeoTorus.h"
#include "GeoModelKernel/GeoGenericTrap.h"

// GeoShapeAction
#include "GeoModelKernel/GeoShapeAction.h"
//## begin module%3DCD269E0092.additionalDeclarations preserve=yes
//## end module%3DCD269E0092.additionalDeclarations


// Class GeoShapeAction 

GeoShapeAction::GeoShapeAction()
  //## begin GeoShapeAction::GeoShapeAction%3DCD269E0092_const.hasinit preserve=no
  //## end GeoShapeAction::GeoShapeAction%3DCD269E0092_const.hasinit
  //## begin GeoShapeAction::GeoShapeAction%3DCD269E0092_const.initialization preserve=yes
  :m_terminate(false)
  //## end GeoShapeAction::GeoShapeAction%3DCD269E0092_const.initialization
{
  //## begin GeoShapeAction::GeoShapeAction%3DCD269E0092_const.body preserve=yes
  //## end GeoShapeAction::GeoShapeAction%3DCD269E0092_const.body
}


GeoShapeAction::~GeoShapeAction()
{
  //## begin GeoShapeAction::~GeoShapeAction%3DCD269E0092_dest.body preserve=yes
  //## end GeoShapeAction::~GeoShapeAction%3DCD269E0092_dest.body
}



//## Other Operations (implementation)
Query<unsigned int>  GeoShapeAction::getDepthLimit ()
{
  //## begin GeoShapeAction::getDepthLimit%3DCD27C70049.body preserve=yes
    return m_depth;
  //## end GeoShapeAction::getDepthLimit%3DCD27C70049.body
}

void GeoShapeAction::setDepthLimit (unsigned int limit)
{
  //## begin GeoShapeAction::setDepthLimit%3DCD27F9033A.body preserve=yes
    m_depth=limit;
  //## end GeoShapeAction::setDepthLimit%3DCD27F9033A.body
}

void GeoShapeAction::clearDepthLimit ()
{
  //## begin GeoShapeAction::clearDepthLimit%3DCD27FE0084.body preserve=yes
	m_depth=Query<unsigned int> ();
  //## end GeoShapeAction::clearDepthLimit%3DCD27FE0084.body
}

void GeoShapeAction::terminate ()
{
  //## begin GeoShapeAction::terminate%3DCD27ED015C.body preserve=yes
    m_terminate=true;
  //## end GeoShapeAction::terminate%3DCD27ED015C.body
}

bool GeoShapeAction::shouldTerminate () const
{
  //## begin GeoShapeAction::shouldTerminate%3DCD27F400BC.body preserve=yes
    return m_terminate;
  //## end GeoShapeAction::shouldTerminate%3DCD27F400BC.body
}

void GeoShapeAction::handleShape (const GeoShape *)
{
  //## begin GeoShapeAction::handleShape%3DCD296E019C.body preserve=yes
  //## end GeoShapeAction::handleShape%3DCD296E019C.body
}

void GeoShapeAction::handleShift (const GeoShapeShift *shift)
{
  //## begin GeoShapeAction::handleShift%3DCD29F30283.body preserve=yes
  handleShape(shift);
  //## end GeoShapeAction::handleShift%3DCD29F30283.body
}

void GeoShapeAction::handleUnion (const GeoShapeUnion *unio)
{
  //## begin GeoShapeAction::handleUnion%3DCD29F60101.body preserve=yes
  handleShape(unio);
  //## end GeoShapeAction::handleUnion%3DCD29F60101.body
}

void GeoShapeAction::handleIntersection (const GeoShapeIntersection *isect)
{
  //## begin GeoShapeAction::handleIntersection%3DCD29F90232.body preserve=yes
  handleShape(isect);
  //## end GeoShapeAction::handleIntersection%3DCD29F90232.body
}

void GeoShapeAction::handleSubtraction (const GeoShapeSubtraction *subtract)
{
  //## begin GeoShapeAction::handleSubtraction%3DCD2A570179.body preserve=yes
  handleShape(subtract);
  //## end GeoShapeAction::handleSubtraction%3DCD2A570179.body
}

void GeoShapeAction::handleBox (const GeoBox *box)
{
  //## begin GeoShapeAction::handleBox%3DCD2A4200EC.body preserve=yes
  handleShape(box);
  //## end GeoShapeAction::handleBox%3DCD2A4200EC.body
}

void GeoShapeAction::handleCons (const GeoCons *cons)
{
  //## begin GeoShapeAction::handleCons%3DCD2A47018A.body preserve=yes
  handleShape(cons);
  //## end GeoShapeAction::handleCons%3DCD2A47018A.body
}

void GeoShapeAction::handlePara (const GeoPara *para)
{
  //## begin GeoShapeAction::handlePara%3DCD2A4803E4.body preserve=yes
  handleShape(para);
  //## end GeoShapeAction::handlePara%3DCD2A4803E4.body
}

void GeoShapeAction::handlePcon (const GeoPcon *pcon)
{
  //## begin GeoShapeAction::handlePcon%3DCD2A4A0365.body preserve=yes
  handleShape(pcon);
  //## end GeoShapeAction::handlePcon%3DCD2A4A0365.body
}

void GeoShapeAction::handlePgon (const GeoPgon *pgon)
{
  //## begin GeoShapeAction::handlePgon%3DCD2A4C02B4.body preserve=yes
  handleShape(pgon);
  //## end GeoShapeAction::handlePgon%3DCD2A4C02B4.body
}

void GeoShapeAction::handleTrap (const GeoTrap *trap)
{
  //## begin GeoShapeAction::handleTrap%3DCD2A4F0041.body preserve=yes
  handleShape(trap);
  //## end GeoShapeAction::handleTrap%3DCD2A4F0041.body
}

void GeoShapeAction::handleTrd (const GeoTrd *trd)
{
  //## begin GeoShapeAction::handleTrd%3DCD2A5101A2.body preserve=yes
  handleShape(trd);
  //## end GeoShapeAction::handleTrd%3DCD2A5101A2.body
}

void GeoShapeAction::handleTube (const GeoTube *tube)
{
  //## begin GeoShapeAction::handleTube%3DCD2A5301A5.body preserve=yes
  handleShape(tube);
  //## end GeoShapeAction::handleTube%3DCD2A5301A5.body
}

void GeoShapeAction::handleTubs (const GeoTubs *tubs)
{
  //## begin GeoShapeAction::handleTubs%3DCD2A550162.body preserve=yes
  handleShape(tubs);
  //## end GeoShapeAction::handleTubs%3DCD2A550162.body
}

GeoShapePath * GeoShapeAction::getPath ()
{
  //## begin GeoShapeAction::getPath%3DCE8FD30175.body preserve=yes
	return &m_path;
  //## end GeoShapeAction::getPath%3DCE8FD30175.body
}

// Additional Declarations
  //## begin GeoShapeAction%3DCD269E0092.declarations preserve=yes
void GeoShapeAction::handleLArCustom (const LArCustomShape *lar)
{
  handleShape( (GeoShape *) lar);
}

  //## end GeoShapeAction%3DCD269E0092.declarations

void GeoShapeAction::handleSimplePolygonBrep (const GeoSimplePolygonBrep* brep)
{
  handleShape(brep);
}

void GeoShapeAction::handleTessellatedSolid (const GeoTessellatedSolid* tessellated)
{
  handleShape(tessellated);
}

void GeoShapeAction::handleEllipticalTube (const GeoEllipticalTube * elltube)
{
  handleShape(elltube);
}

void GeoShapeAction::handleTorus (const GeoTorus * torus)
{
  handleShape(torus);
}

void GeoShapeAction::handleGenericTrap (const GeoGenericTrap * gentrap)
{
  handleShape(gentrap);
}

//## begin module%3DCD269E0092.epilog preserve=yes
//## end module%3DCD269E0092.epilog
