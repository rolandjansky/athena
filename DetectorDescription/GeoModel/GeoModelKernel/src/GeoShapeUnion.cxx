/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2596A0198.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2596A0198.cm

//## begin module%3CD2596A0198.cp preserve=no
//## end module%3CD2596A0198.cp

//## Module: GeoShapeUnion%3CD2596A0198; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeUnion.cxx

//## begin module%3CD2596A0198.additionalIncludes preserve=no
//## end module%3CD2596A0198.additionalIncludes

//## begin module%3CD2596A0198.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include <stdexcept>
//## end module%3CD2596A0198.includes

// GeoShapeUnion
#include "GeoModelKernel/GeoShapeUnion.h"
//## begin module%3CD2596A0198.additionalDeclarations preserve=yes
//## end module%3CD2596A0198.additionalDeclarations


// Class GeoShapeUnion 

//## begin GeoShapeUnion::s_classType%3CD2596A01A6.attr preserve=no  public: static const std::string {U} "Union"
const std::string GeoShapeUnion::s_classType = "Union";
//## end GeoShapeUnion::s_classType%3CD2596A01A6.attr

//## begin GeoShapeUnion::s_classTypeID%3CD2596A01A7.attr preserve=no  public: static const ShapeType {U} 0x01
const ShapeType GeoShapeUnion::s_classTypeID = 0x01;
//## end GeoShapeUnion::s_classTypeID%3CD2596A01A7.attr

GeoShapeUnion::GeoShapeUnion (const GeoShape* A, const GeoShape* B)
  //## begin GeoShapeUnion::GeoShapeUnion%3CD2596A019D.hasinit preserve=no
  //## end GeoShapeUnion::GeoShapeUnion%3CD2596A019D.hasinit
  //## begin GeoShapeUnion::GeoShapeUnion%3CD2596A019D.initialization preserve=yes
  :
m_opA (A),
m_opB (B)
  //## end GeoShapeUnion::GeoShapeUnion%3CD2596A019D.initialization
{
  //## begin GeoShapeUnion::GeoShapeUnion%3CD2596A019D.body preserve=yes
  m_opA->ref ();
  m_opB->ref ();
  //## end GeoShapeUnion::GeoShapeUnion%3CD2596A019D.body
}


GeoShapeUnion::~GeoShapeUnion()
{
  //## begin GeoShapeUnion::~GeoShapeUnion%3CD2596A0198_dest.body preserve=yes
  m_opA->unref ();
  m_opB->unref ();
  //## end GeoShapeUnion::~GeoShapeUnion%3CD2596A0198_dest.body
}



//## Other Operations (implementation)
double GeoShapeUnion::volume () const
{
  //## begin GeoShapeUnion::volume%3CD2596A019C.body preserve=yes
  GeoPolyhedrizeAction a;
  exec (&a);
  const GeoPolyhedron *poly = a.getPolyhedron ();
  double vol = poly->GetVolume ();
  return vol;
  //## end GeoShapeUnion::volume%3CD2596A019C.body
}

const std::string & GeoShapeUnion::type () const
{
  //## begin GeoShapeUnion::type%3CD2596A01A0.body preserve=yes
  return s_classType;
  //## end GeoShapeUnion::type%3CD2596A01A0.body
}

ShapeType GeoShapeUnion::typeID () const
{
  //## begin GeoShapeUnion::typeID%3CD2596A01A1.body preserve=yes
  return s_classTypeID;
  //## end GeoShapeUnion::typeID%3CD2596A01A1.body
}

const GeoShape* GeoShapeUnion::getOpA () const
{
  //## begin GeoShapeUnion::getOpA%3CE7F18900AB.body preserve=yes
  return m_opA;
  //## end GeoShapeUnion::getOpA%3CE7F18900AB.body
}

const GeoShape* GeoShapeUnion::getOpB () const
{
  //## begin GeoShapeUnion::getOpB%3CE7F18900BF.body preserve=yes
  return m_opB;
  //## end GeoShapeUnion::getOpB%3CE7F18900BF.body
}

void GeoShapeUnion::exec (GeoShapeAction *action) const
{
  //## begin GeoShapeUnion::exec%3DB96AB90203.body preserve=yes

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

  //## end GeoShapeUnion::exec%3DB96AB90203.body
}

// Additional Declarations
  //## begin GeoShapeUnion%3CD2596A0198.declarations preserve=yes
  //## end GeoShapeUnion%3CD2596A0198.declarations

//## begin module%3CD2596A0198.epilog preserve=yes
//## end module%3CD2596A0198.epilog
