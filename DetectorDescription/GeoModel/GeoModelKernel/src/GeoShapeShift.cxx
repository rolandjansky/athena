/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D150E4703DA.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D150E4703DA.cm

//## begin module%3D150E4703DA.cp preserve=no
//## end module%3D150E4703DA.cp

//## Module: GeoShapeShift%3D150E4703DA; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeShift.cxx

//## begin module%3D150E4703DA.additionalIncludes preserve=no
//## end module%3D150E4703DA.additionalIncludes

//## begin module%3D150E4703DA.includes preserve=yes
#include "GeoModelKernel/GeoShapeAction.h"
//## end module%3D150E4703DA.includes

// GeoShapeShift
#include "GeoModelKernel/GeoShapeShift.h"
//## begin module%3D150E4703DA.additionalDeclarations preserve=yes
//## end module%3D150E4703DA.additionalDeclarations


// Class GeoShapeShift 

//## begin GeoShapeShift::s_classType%3D150E9F008E.attr preserve=no  public: static const std::string {U} "Shift"
const std::string GeoShapeShift::s_classType = "Shift";
//## end GeoShapeShift::s_classType%3D150E9F008E.attr

//## begin GeoShapeShift::s_classTypeID%3D150E9F0098.attr preserve=no  public: static const ShapeType {U} 0x03
const ShapeType GeoShapeShift::s_classTypeID = 0x03;
//## end GeoShapeShift::s_classTypeID%3D150E9F0098.attr

GeoShapeShift::GeoShapeShift (const GeoShape* A, const HepGeom::Transform3D &X)
  //## begin GeoShapeShift::GeoShapeShift%3D150F480131.hasinit preserve=no
  //## end GeoShapeShift::GeoShapeShift%3D150F480131.hasinit
  //## begin GeoShapeShift::GeoShapeShift%3D150F480131.initialization preserve=yes
  :
m_op (A),
m_shift (X)
  //## end GeoShapeShift::GeoShapeShift%3D150F480131.initialization
{
  //## begin GeoShapeShift::GeoShapeShift%3D150F480131.body preserve=yes
  m_op->ref ();
  //## end GeoShapeShift::GeoShapeShift%3D150F480131.body
}


GeoShapeShift::~GeoShapeShift()
{
  //## begin GeoShapeShift::~GeoShapeShift%3D150E4703DA_dest.body preserve=yes
  m_op->unref ();
  //## end GeoShapeShift::~GeoShapeShift%3D150E4703DA_dest.body
}



//## Other Operations (implementation)
double GeoShapeShift::volume () const
{
  //## begin GeoShapeShift::volume%3D150F48011D.body preserve=yes
  return m_op->volume ();
  //## end GeoShapeShift::volume%3D150F48011D.body
}

const std::string & GeoShapeShift::type () const
{
  //## begin GeoShapeShift::type%3D150F48014F.body preserve=yes
  return s_classType;
  //## end GeoShapeShift::type%3D150F48014F.body
}

ShapeType GeoShapeShift::typeID () const
{
  //## begin GeoShapeShift::typeID%3D150F480163.body preserve=yes
  return s_classTypeID;
  //## end GeoShapeShift::typeID%3D150F480163.body
}

const GeoShape* GeoShapeShift::getOp () const
{
  //## begin GeoShapeShift::getOp%3D150F480181.body preserve=yes
  return m_op;
  //## end GeoShapeShift::getOp%3D150F480181.body
}

const HepGeom::Transform3D & GeoShapeShift::getX () const
{
  //## begin GeoShapeShift::getX%3D150F480195.body preserve=yes
  return m_shift;
  //## end GeoShapeShift::getX%3D150F480195.body
}

void GeoShapeShift::exec (GeoShapeAction *action) const
{
  //## begin GeoShapeShift::exec%3DB96AC303B6.body preserve=yes
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
  //## end GeoShapeShift::exec%3DB96AC303B6.body
}

// Additional Declarations
  //## begin GeoShapeShift%3D150E4703DA.declarations preserve=yes
  //## end GeoShapeShift%3D150E4703DA.declarations

//## begin module%3D150E4703DA.epilog preserve=yes
//## end module%3D150E4703DA.epilog
