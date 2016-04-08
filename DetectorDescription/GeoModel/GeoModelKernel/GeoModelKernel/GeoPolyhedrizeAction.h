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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3DCD28730322.cp

//## Module: GeoPolyhedrizeAction%3DCD28730322; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPolyhedrizeAction.h

#ifndef GeoPolyhedrizeAction_h
#define GeoPolyhedrizeAction_h 1

//## begin module%3DCD28730322.additionalIncludes preserve=no
//## end module%3DCD28730322.additionalIncludes

//## begin module%3DCD28730322.includes preserve=yes
//## end module%3DCD28730322.includes

// GeoShapeAction
#include "GeoModelKernel/GeoShapeAction.h"
//## begin module%3DCD28730322.additionalDeclarations preserve=yes
class GeoPolyhedron;
//## end module%3DCD28730322.additionalDeclarations


//## begin GeoPolyhedrizeAction%3DCD28730322.preface preserve=yes
//## end GeoPolyhedrizeAction%3DCD28730322.preface

//## Class: GeoPolyhedrizeAction%3DCD28730322
//	This action polyhedrizes a shape.  This includes, for
//	example, boolean shapes.
//
//	A polhyedral representation is useful when you want to:
//
//	-visualize the shape.
//	-compute the shape's volume.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPolyhedrizeAction : public GeoShapeAction  //## Inherits: <unnamed>%3DCD28C803D8
{
  //## begin GeoPolyhedrizeAction%3DCD28730322.initialDeclarations preserve=yes
  //## end GeoPolyhedrizeAction%3DCD28730322.initialDeclarations

  public:
    //## Constructors (generated)
      GeoPolyhedrizeAction();

    //## Destructor (generated)
      virtual ~GeoPolyhedrizeAction();


    //## Other Operations (specified)
      //## Operation: handleShift%3DCD2D500015
      //	Handles a shift shape.
      virtual void handleShift (const GeoShapeShift *shift);

      //## Operation: handleUnion%3DCD2D500033
      //	Handles a union shape.
      virtual void handleUnion (const GeoShapeUnion *unio);

      //## Operation: handleIntersection%3DCD2D50005B
      //	Handles an intersection shape.
      virtual void handleIntersection (const GeoShapeIntersection *isect);

      //## Operation: handleSubtraction%3DCD2D500079
      //	Handles a subtraction shape.
      virtual void handleSubtraction (const GeoShapeSubtraction *subtract);

      //## Operation: handleBox%3DCD2D500097
      //	Handles a box shape.
      virtual void handleBox (const GeoBox *box);

      //## Operation: handleCons%3DCD2D5000B5
      //	Handles a cons shape.
      virtual void handleCons (const GeoCons *cons);

      //## Operation: handlePara%3DCD2D5000DD
      //	Handles a para shape.
      virtual void handlePara (const GeoPara *para);

      //## Operation: handlePcon%3DCD2D5000FB
      //	Handles a pcon shape.
      virtual void handlePcon (const GeoPcon *pcon);

      //## Operation: handlePgon%3DCD2D500119
      //	Handles a pgon shape.
      virtual void handlePgon (const GeoPgon *pgon);

      //## Operation: handleTrap%3DCD2D500137
      //	Handles a trap shape.
      virtual void handleTrap (const GeoTrap *trap);

      //## Operation: handleTrd%3DCD2D50015F
      //	Handles a  trd shape.
      virtual void handleTrd (const GeoTrd *trd);

      //## Operation: handleTube%3DCD2D50017D
      //	Handles a tube shape.
      virtual void handleTube (const GeoTube *tube);

      //## Operation: handleTubs%3DCD2D50019B
      //	Handles a tubs shape.
      virtual void handleTubs (const GeoTubs *tubs);

      //## Operation: getPolyhedron%3DCD2E3C00E6
      //	Returns the polyhedral representation of a shape.
      const GeoPolyhedron * getPolyhedron () const;

    // Additional Public Declarations
      //## begin GeoPolyhedrizeAction%3DCD28730322.public preserve=yes
      //## end GeoPolyhedrizeAction%3DCD28730322.public

  protected:
    // Additional Protected Declarations
      //## begin GeoPolyhedrizeAction%3DCD28730322.protected preserve=yes
      //## end GeoPolyhedrizeAction%3DCD28730322.protected

  private:
    //## Constructors (generated)
      GeoPolyhedrizeAction(const GeoPolyhedrizeAction &right);

    //## Assignment Operation (generated)
      GeoPolyhedrizeAction & operator=(const GeoPolyhedrizeAction &right);

    // Data Members for Class Attributes

      //## Attribute: m_polyhedron%3DCD2DA00060
      //	This polyhedral representation of the shape.
      //## begin GeoPolyhedrizeAction::m_polyhedron%3DCD2DA00060.attr preserve=no  private: GeoPolyhedron * {U} NULL
      GeoPolyhedron *m_polyhedron;
      //## end GeoPolyhedrizeAction::m_polyhedron%3DCD2DA00060.attr

    // Additional Private Declarations
      //## begin GeoPolyhedrizeAction%3DCD28730322.private preserve=yes
      //## end GeoPolyhedrizeAction%3DCD28730322.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPolyhedrizeAction%3DCD28730322.implementation preserve=yes
      //## end GeoPolyhedrizeAction%3DCD28730322.implementation

};

//## begin GeoPolyhedrizeAction%3DCD28730322.postscript preserve=yes
//## end GeoPolyhedrizeAction%3DCD28730322.postscript

// Class GeoPolyhedrizeAction 

//## begin module%3DCD28730322.epilog preserve=yes
//## end module%3DCD28730322.epilog


#endif
