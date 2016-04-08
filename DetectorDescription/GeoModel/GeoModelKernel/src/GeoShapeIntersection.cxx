/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD259190322.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD259190322.cm

//## begin module%3CD259190322.cp preserve=no
//## end module%3CD259190322.cp

//## Module: GeoShapeIntersection%3CD259190322; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeIntersection.cxx

//## begin module%3CD259190322.additionalIncludes preserve=no
//## end module%3CD259190322.additionalIncludes

//## begin module%3CD259190322.includes preserve=yes
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoShapeAction.h"
#include <stdexcept>
//## end module%3CD259190322.includes

// GeoShapeIntersection
#include "GeoModelKernel/GeoShapeIntersection.h"
//## begin module%3CD259190322.additionalDeclarations preserve=yes
//## end module%3CD259190322.additionalDeclarations


// Class GeoShapeIntersection 

//## begin GeoShapeIntersection::s_classType%3CD259190330.attr preserve=no  public: static const std::string {U} "Intersection"
const std::string GeoShapeIntersection::s_classType = "Intersection";
//## end GeoShapeIntersection::s_classType%3CD259190330.attr

//## begin GeoShapeIntersection::s_classTypeID%3CD259190331.attr preserve=no  public: static const ShapeType {U} 0x00
const ShapeType GeoShapeIntersection::s_classTypeID = 0x00;
//## end GeoShapeIntersection::s_classTypeID%3CD259190331.attr

GeoShapeIntersection::GeoShapeIntersection (const GeoShape* A, const GeoShape* B)
  //## begin GeoShapeIntersection::GeoShapeIntersection%3CD259190327.hasinit preserve=no
  //## end GeoShapeIntersection::GeoShapeIntersection%3CD259190327.hasinit
  //## begin GeoShapeIntersection::GeoShapeIntersection%3CD259190327.initialization preserve=yes
  :
m_opA (A),
m_opB (B)
  //## end GeoShapeIntersection::GeoShapeIntersection%3CD259190327.initialization
{
  //## begin GeoShapeIntersection::GeoShapeIntersection%3CD259190327.body preserve=yes
  m_opA->ref ();
  m_opB->ref ();
  //## end GeoShapeIntersection::GeoShapeIntersection%3CD259190327.body
}


GeoShapeIntersection::~GeoShapeIntersection()
{
  //## begin GeoShapeIntersection::~GeoShapeIntersection%3CD259190322_dest.body preserve=yes
  m_opA->unref ();
  m_opB->unref ();
  //## end GeoShapeIntersection::~GeoShapeIntersection%3CD259190322_dest.body
}



//## Other Operations (implementation)
double GeoShapeIntersection::volume () const
{
  //## begin GeoShapeIntersection::volume%3CD259190326.body preserve=yes
  GeoPolyhedrizeAction a;
  exec(&a);
  const GeoPolyhedron *poly = a.getPolyhedron();
  double vol = poly->GetVolume ();
  return vol;
  //## end GeoShapeIntersection::volume%3CD259190326.body
}

const std::string & GeoShapeIntersection::type () const
{
  //## begin GeoShapeIntersection::type%3CD25919032A.body preserve=yes
  return s_classType;
  //## end GeoShapeIntersection::type%3CD25919032A.body
}

ShapeType GeoShapeIntersection::typeID () const
{
  //## begin GeoShapeIntersection::typeID%3CD25919032B.body preserve=yes
  return s_classTypeID;
  //## end GeoShapeIntersection::typeID%3CD25919032B.body
}

const GeoShape* GeoShapeIntersection::getOpA () const
{
  //## begin GeoShapeIntersection::getOpA%3CE7F14F02C4.body preserve=yes
  return m_opA;
  //## end GeoShapeIntersection::getOpA%3CE7F14F02C4.body
}

const GeoShape* GeoShapeIntersection::getOpB () const
{
  //## begin GeoShapeIntersection::getOpB%3CE7F1510317.body preserve=yes
  return m_opB;
  //## end GeoShapeIntersection::getOpB%3CE7F1510317.body
}

void GeoShapeIntersection::exec (GeoShapeAction *action) const
{
  //## begin GeoShapeIntersection::exec%3DB96AA102BC.body preserve=yes
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
  //## end GeoShapeIntersection::exec%3DB96AA102BC.body
}

// Additional Declarations
  //## begin GeoShapeIntersection%3CD259190322.declarations preserve=yes
  //## end GeoShapeIntersection%3CD259190322.declarations

//## begin module%3CD259190322.epilog preserve=yes
//## end module%3CD259190322.epilog
