/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD131E602E1.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD131E602E1.cm

//## begin module%3CD131E602E1.cp preserve=no
//## end module%3CD131E602E1.cp

//## Module: GeoShape%3CD131E602E1; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShape.cxx

//## begin module%3CD131E602E1.additionalIncludes preserve=no
//## end module%3CD131E602E1.additionalIncludes

//## begin module%3CD131E602E1.includes preserve=yes
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"
//## end module%3CD131E602E1.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD131E602E1.additionalDeclarations preserve=yes
//## end module%3CD131E602E1.additionalDeclarations


// Class GeoShape 

GeoShape::GeoShape ()
  //## begin GeoShape::GeoShape%3CD3020A03BC.hasinit preserve=no
  //## end GeoShape::GeoShape%3CD3020A03BC.hasinit
  //## begin GeoShape::GeoShape%3CD3020A03BC.initialization preserve=yes
  //## end GeoShape::GeoShape%3CD3020A03BC.initialization
{
  //## begin GeoShape::GeoShape%3CD3020A03BC.body preserve=yes
  //## end GeoShape::GeoShape%3CD3020A03BC.body
}


GeoShape::~GeoShape()
{
  //## begin GeoShape::~GeoShape%3CD131E602E1_dest.body preserve=yes
  //## end GeoShape::~GeoShape%3CD131E602E1_dest.body
}



//## Other Operations (implementation)
const GeoShapeUnion & GeoShape::add (const GeoShape& shape) const
{
  //## begin GeoShape::add%3CD14BDC035C.body preserve=yes
  GeoShapeUnion *unionNode = new GeoShapeUnion (this, &shape);
  return *unionNode;
  //## end GeoShape::add%3CD14BDC035C.body
}

const GeoShapeSubtraction & GeoShape::subtract (const GeoShape& shape) const
{
  //## begin GeoShape::subtract%3CD14BBF0397.body preserve=yes
  GeoShapeSubtraction *subNode = new GeoShapeSubtraction (this, &shape);
  return *subNode;
  //## end GeoShape::subtract%3CD14BBF0397.body
}

const GeoShapeIntersection & GeoShape::intersect (const GeoShape& shape) const
{
  //## begin GeoShape::intersect%3CD14BD603E0.body preserve=yes
  GeoShapeIntersection *intNode = new GeoShapeIntersection (this, &shape);
  return *intNode;
  //## end GeoShape::intersect%3CD14BD603E0.body
}

const GeoShapeShift & GeoShape::operator << (const HepGeom::Transform3D &shift) const
{
  //## begin GeoShape::operator <<%3D1511DC0297.body preserve=yes
  GeoShapeShift *shiftNode = new GeoShapeShift (this, shift);
  return *shiftNode;
  //## end GeoShape::operator <<%3D1511DC0297.body
}

// Additional Declarations
  //## begin GeoShape%3CD131E602E1.declarations preserve=yes
  //## end GeoShape%3CD131E602E1.declarations

//## begin module%3CD131E602E1.epilog preserve=yes
//## end module%3CD131E602E1.epilog
