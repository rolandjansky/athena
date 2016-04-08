/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2595103E1.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2595103E1.cm

//## begin module%3CD2595103E1.cp preserve=no
//## end module%3CD2595103E1.cp

//## Module: GeoShapeSubtraction%3CD2595103E1; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeSubtraction.cxx

//## begin module%3CD2595103E1.additionalIncludes preserve=no
//## end module%3CD2595103E1.additionalIncludes

//## begin module%3CD2595103E1.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
#include "GeoModelKernel/GeoPolyhedrizeAction.h"
#include "GeoModelKernel/GeoPolyhedron.h"
#include <stdexcept>
//## end module%3CD2595103E1.includes

// GeoShapeSubtraction
#include "GeoModelKernel/GeoShapeSubtraction.h"
//## begin module%3CD2595103E1.additionalDeclarations preserve=yes
//## end module%3CD2595103E1.additionalDeclarations


// Class GeoShapeSubtraction 

//## begin GeoShapeSubtraction::s_classType%3CD2595103EC.attr preserve=no  public: static const std::string {U} "Subtraction"
const std::string GeoShapeSubtraction::s_classType = "Subtraction";
//## end GeoShapeSubtraction::s_classType%3CD2595103EC.attr

//## begin GeoShapeSubtraction::s_classTypeID%3CD2595103ED.attr preserve=no  public: static const ShapeType {U} 0x02
const ShapeType GeoShapeSubtraction::s_classTypeID = 0x02;
//## end GeoShapeSubtraction::s_classTypeID%3CD2595103ED.attr

GeoShapeSubtraction::GeoShapeSubtraction (const GeoShape* A, const GeoShape* B)
  //## begin GeoShapeSubtraction::GeoShapeSubtraction%3CD2595103E5.hasinit preserve=no
  //## end GeoShapeSubtraction::GeoShapeSubtraction%3CD2595103E5.hasinit
  //## begin GeoShapeSubtraction::GeoShapeSubtraction%3CD2595103E5.initialization preserve=yes
  :
m_opA (A),
m_opB (B)
  //## end GeoShapeSubtraction::GeoShapeSubtraction%3CD2595103E5.initialization
{
  //## begin GeoShapeSubtraction::GeoShapeSubtraction%3CD2595103E5.body preserve=yes
  m_opA->ref ();
  m_opB->ref ();
  //## end GeoShapeSubtraction::GeoShapeSubtraction%3CD2595103E5.body
}


GeoShapeSubtraction::~GeoShapeSubtraction()
{
  //## begin GeoShapeSubtraction::~GeoShapeSubtraction%3CD2595103E1_dest.body preserve=yes
  m_opA->unref ();
  m_opB->unref ();
  //## end GeoShapeSubtraction::~GeoShapeSubtraction%3CD2595103E1_dest.body
}



//## Other Operations (implementation)
double GeoShapeSubtraction::volume () const
{
  //## begin GeoShapeSubtraction::volume%3CD2595103E4.body preserve=yes
  GeoPolyhedrizeAction a;
  exec(&a);
  const GeoPolyhedron *poly = a.getPolyhedron();
  double vol = poly->GetVolume ();
  return vol;
  //## end GeoShapeSubtraction::volume%3CD2595103E4.body
}

const std::string & GeoShapeSubtraction::type () const
{
  //## begin GeoShapeSubtraction::type%3CD2595103E7.body preserve=yes
  return s_classType;
  //## end GeoShapeSubtraction::type%3CD2595103E7.body
}

ShapeType GeoShapeSubtraction::typeID () const
{
  //## begin GeoShapeSubtraction::typeID%3CD2595103E8.body preserve=yes
  return s_classTypeID;
  //## end GeoShapeSubtraction::typeID%3CD2595103E8.body
}

const GeoShape* GeoShapeSubtraction::getOpA () const
{
  //## begin GeoShapeSubtraction::getOpA%3CE7F07603D1.body preserve=yes
  return m_opA;
  //## end GeoShapeSubtraction::getOpA%3CE7F07603D1.body
}

const GeoShape* GeoShapeSubtraction::getOpB () const
{
  //## begin GeoShapeSubtraction::getOpB%3D1DD1E7025E.body preserve=yes
  return m_opB;
  //## end GeoShapeSubtraction::getOpB%3D1DD1E7025E.body
}

void GeoShapeSubtraction::exec (GeoShapeAction *action) const
{
  //## begin GeoShapeSubtraction::exec%3DB96AAE0171.body preserve=yes
  action->getPath ()->push (this);
  action->handleSubtraction (this);
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
  //## end GeoShapeSubtraction::exec%3DB96AAE0171.body
}

// Additional Declarations
  //## begin GeoShapeSubtraction%3CD2595103E1.declarations preserve=yes
  //## end GeoShapeSubtraction%3CD2595103E1.declarations

//## begin module%3CD2595103E1.epilog preserve=yes
//## end module%3CD2595103E1.epilog
