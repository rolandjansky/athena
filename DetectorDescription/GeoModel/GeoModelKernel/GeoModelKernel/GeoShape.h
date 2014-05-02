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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD131E602E1.cp

//## Module: GeoShape%3CD131E602E1; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShape.h

#ifndef GeoShape_h
#define GeoShape_h 1

//## begin module%3CD131E602E1.additionalIncludes preserve=no
//## end module%3CD131E602E1.additionalIncludes

//## begin module%3CD131E602E1.includes preserve=yes
#include <CLHEP/Geometry/Transform3D.h>
#include <string>
//## end module%3CD131E602E1.includes

// RCBase
#include "GeoModelKernel/RCBase.h"
//## begin module%3CD131E602E1.additionalDeclarations preserve=yes
typedef unsigned int ShapeType;
class GeoShapeIntersection;
class GeoShapeUnion;
class GeoShapeSubtraction;
class GeoShapeShift;
////class HepGeom::Transform3D;
class GeoShapeAction;

//## end module%3CD131E602E1.additionalDeclarations


//## begin GeoShape%3CD131E602E1.preface preserve=yes
//## end GeoShape%3CD131E602E1.preface

//## Class: GeoShape%3CD131E602E1
//	This class describes a Shape. Shapes will be able to do
//	the following things:
//
//	    * Identify themselves.
//	    * Compute their volume.
//	    * Combine themselves using Boolean operations with
//	      other shapes.
//
//	The type identification works as follows:
//
//	     if (myShape->typeId()==GeoBox::classTypeId()) {
//
//	       .....
//
//	     }
//
//	This avoids the need for dynamic casting with these
//	classes.
//
//	Also noteworthy: Shapes are allocated on the heap and
//	deleted automatically when their reference count falls
//	to zero.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoShape : public RCBase  //## Inherits: <unnamed>%3CD131E602E2
{
  //## begin GeoShape%3CD131E602E1.initialDeclarations preserve=yes
  //## end GeoShape%3CD131E602E1.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoShape%3CD3020A03BC
      //	Constructor for shape.  Must provide the name, a string
      //	to identify this shape.
      GeoShape ();


    //## Other Operations (specified)
      //## Operation: volume%3CD14BB501EE
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const = 0;

      //## Operation: add%3CD14BDC035C
      //	Boolean OR operation for shapes
      const GeoShapeUnion & add (const GeoShape& shape) const;

      //## Operation: subtract%3CD14BBF0397
      //	Subtraction operation for shapes.
      const GeoShapeSubtraction & subtract (const GeoShape& shape) const;

      //## Operation: intersect%3CD14BD603E0
      //	Intersection of shapes.
      const GeoShapeIntersection & intersect (const GeoShape& shape) const;

      //## Operation: operator <<%3D1511DC0297
      //	Shift shapes, for boolean operations.
      const GeoShapeShift & operator << (const HepGeom::Transform3D &shift) const;

      //## Operation: type%3CD14CAE02B4
      //	Returns the shape type, as a string.
      virtual const std::string & type () const = 0;

      //## Operation: typeID%3CD14CB3037A
      //	Returns the shape type, as an coded integer.
      virtual ShapeType typeID () const = 0;

      //## Operation: exec%3DB969850304
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const = 0;

    // Additional Public Declarations
      //## begin GeoShape%3CD131E602E1.public preserve=yes
      //## end GeoShape%3CD131E602E1.public

  protected:
    //## Destructor (generated)
      virtual ~GeoShape();

    // Additional Protected Declarations
      //## begin GeoShape%3CD131E602E1.protected preserve=yes
      //## end GeoShape%3CD131E602E1.protected

  private:
    //## Constructors (generated)
      GeoShape(const GeoShape &right);

    //## Assignment Operation (generated)
      GeoShape & operator=(const GeoShape &right);

    // Additional Private Declarations
      //## begin GeoShape%3CD131E602E1.private preserve=yes
      //## end GeoShape%3CD131E602E1.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoShape%3CD131E602E1.implementation preserve=yes
      //## end GeoShape%3CD131E602E1.implementation

};

//## begin GeoShape%3CD131E602E1.postscript preserve=yes
//## end GeoShape%3CD131E602E1.postscript

// Class GeoShape 

//## begin module%3CD131E602E1.epilog preserve=yes
//## end module%3CD131E602E1.epilog


#endif
