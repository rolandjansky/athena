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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3DCD269E0092.cp

//## Module: GeoShapeAction%3DCD269E0092; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeAction.h

#ifndef GeoShapeAction_h
#define GeoShapeAction_h 1

//## begin module%3DCD269E0092.additionalIncludes preserve=no
//## end module%3DCD269E0092.additionalIncludes

//## begin module%3DCD269E0092.includes preserve=yes
#include "GeoModelKernel/Query.h"
//## end module%3DCD269E0092.includes

// GeoShapePath
#include "GeoModelKernel/GeoShapePath.h"
//## begin module%3DCD269E0092.additionalDeclarations preserve=yes
class GeoShape;
class GeoShapeIntersection;
class GeoShapeUnion;
class GeoShapeSubtraction;
class GeoShapeShift;
class GeoBox;
class GeoCons;
class GeoPcon;
class GeoPgon;
class GeoTrap;
class GeoTrd;
class GeoPara;
class GeoTubs;
class GeoTube;
class GeoEllipticalTube;
class GeoTorus;

class LArCustomShape;

class GeoSimplePolygonBrep;
class GeoTessellatedSolid;

class GeoGenericTrap;

//## end module%3DCD269E0092.additionalDeclarations


//## begin GeoShapeAction%3DCD269E0092.preface preserve=yes
//## end GeoShapeAction%3DCD269E0092.preface

//## Class: GeoShapeAction%3DCD269E0092
//	This class is a visitor for shapes.  It allows a
//	navigation of the boolean expression tree for shapes, in
//	order to accomplish actions such as volume computation.
//	This is essentially an internal class.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoShapeAction 
{
  //## begin GeoShapeAction%3DCD269E0092.initialDeclarations preserve=yes
  //## end GeoShapeAction%3DCD269E0092.initialDeclarations

  public:
    //## Constructors (generated)
      GeoShapeAction();

    //## Destructor (generated)
      virtual ~GeoShapeAction();


    //## Other Operations (specified)
      //## Operation: getDepthLimit%3DCD27C70049
      Query<unsigned int>  getDepthLimit ();

      //## Operation: setDepthLimit%3DCD27F9033A
      //	Sets a depth limit.
      void setDepthLimit (unsigned int limit);

      //## Operation: clearDepthLimit%3DCD27FE0084
      //	Clears a depth limit, if any.
      void clearDepthLimit ();

      //## Operation: terminate%3DCD27ED015C
      //	Cause the action to terminate.
      void terminate ();

      //## Operation: shouldTerminate%3DCD27F400BC
      //	Determine whether the action should terminate.
      bool shouldTerminate () const;

      //## Operation: handleShape%3DCD296E019C
      //	Handles a shape.
      virtual void handleShape (const GeoShape *);

      //## Operation: handleShift%3DCD29F30283
      //	Handles a shift shape.
      virtual void handleShift (const GeoShapeShift *shift);

      //## Operation: handleUnion%3DCD29F60101
      //	Handles a union shape.
      virtual void handleUnion (const GeoShapeUnion *unio);

      //## Operation: handleIntersection%3DCD29F90232
      //	Handles an intersection shape.
      virtual void handleIntersection (const GeoShapeIntersection *isect);

      //## Operation: handleSubtraction%3DCD2A570179
      //	Handles a subtraction shape.
      virtual void handleSubtraction (const GeoShapeSubtraction *subtract);

      //## Operation: handleBox%3DCD2A4200EC
      //	Handles a box shape.
      virtual void handleBox (const GeoBox *box);

      //## Operation: handleCons%3DCD2A47018A
      //	Handles a cons shape.
      virtual void handleCons (const GeoCons *cons);

      //## Operation: handlePara%3DCD2A4803E4
      //	Handles a para shape.
      virtual void handlePara (const GeoPara *para);

      //## Operation: handlePcon%3DCD2A4A0365
      //	Handles a pcon shape.
      virtual void handlePcon (const GeoPcon *pcon);

      //## Operation: handlePgon%3DCD2A4C02B4
      //	Handles a pgon shape.
      virtual void handlePgon (const GeoPgon *pgon);

      //## Operation: handleTrap%3DCD2A4F0041
      //	Handles a trap shape.
      virtual void handleTrap (const GeoTrap *trap);

      //## Operation: handleTrd%3DCD2A5101A2
      //	Handles a  trd shape.
      virtual void handleTrd (const GeoTrd *trd);

      //## Operation: handleTube%3DCD2A5301A5
      //	Handles a tube shape.
      virtual void handleTube (const GeoTube *tube);

      //## Operation: handleTubs%3DCD2A550162
      //	Handles a tubs shape.
      virtual void handleTubs (const GeoTubs *tubs);

      //## Operation: getPath%3DCE8FD30175
      //	Returns a pointer to the path object.
      GeoShapePath * getPath ();

    // Additional Public Declarations
      //## begin GeoShapeAction%3DCD269E0092.public preserve=yes
      virtual void handleLArCustom (const LArCustomShape *);
      //## end GeoShapeAction%3DCD269E0092.public

      virtual void handleSimplePolygonBrep (const GeoSimplePolygonBrep *);

      virtual void handleTessellatedSolid (const GeoTessellatedSolid *);

      virtual void handleEllipticalTube (const GeoEllipticalTube *);

      virtual void handleTorus (const GeoTorus *);

      virtual void handleGenericTrap (const GeoGenericTrap *);

  protected:
    // Additional Protected Declarations
      //## begin GeoShapeAction%3DCD269E0092.protected preserve=yes
      //## end GeoShapeAction%3DCD269E0092.protected

  private:
    //## Constructors (generated)
      GeoShapeAction(const GeoShapeAction &right);

    //## Assignment Operation (generated)
      GeoShapeAction & operator=(const GeoShapeAction &right);

    // Data Members for Class Attributes

      //## Attribute: m_depth%3DCD278F0310
      //	A limit may be placed upon the depth to which the action
      //	descends.  0 = self.  1 = self and children.
      //## begin GeoShapeAction::m_depth%3DCD278F0310.attr preserve=no  private: Query<unsigned int> {U} 
      Query<unsigned int> m_depth;
      //## end GeoShapeAction::m_depth%3DCD278F0310.attr

      //## Attribute: m_terminate%3DCD278F031A
      //	Termination flag; causes an abortion of action execution.
      //## begin GeoShapeAction::m_terminate%3DCD278F031A.attr preserve=no  private: bool {U} 
      bool m_terminate;
      //## end GeoShapeAction::m_terminate%3DCD278F031A.attr

    // Additional Private Declarations
      //## begin GeoShapeAction%3DCD269E0092.private preserve=yes
      //## end GeoShapeAction%3DCD269E0092.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::Actions::Path management%3DCE8D810009
      //## Role: GeoShapeAction::m_path%3DCE8D8203AE
      //## begin GeoShapeAction::m_path%3DCE8D8203AE.role preserve=no  public: GeoShapePath {1 -> 1VHgN}
      GeoShapePath m_path;
      //## end GeoShapeAction::m_path%3DCE8D8203AE.role

    // Additional Implementation Declarations
      //## begin GeoShapeAction%3DCD269E0092.implementation preserve=yes
      //## end GeoShapeAction%3DCD269E0092.implementation

};

//## begin GeoShapeAction%3DCD269E0092.postscript preserve=yes
//## end GeoShapeAction%3DCD269E0092.postscript

// Class GeoShapeAction 

//## begin module%3DCD269E0092.epilog preserve=yes
//## end module%3DCD269E0092.epilog


#endif
