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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3D150E4703DA.cp

//## Module: GeoShapeShift%3D150E4703DA; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeShift.h

#ifndef GeoShapeShift_h
#define GeoShapeShift_h 1

//## begin module%3D150E4703DA.additionalIncludes preserve=no
//## end module%3D150E4703DA.additionalIncludes

//## begin module%3D150E4703DA.includes preserve=yes
//## end module%3D150E4703DA.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3D150E4703DA.additionalDeclarations preserve=yes
#include "CLHEP/Geometry/Transform3D.h"
//## end module%3D150E4703DA.additionalDeclarations


//## begin GeoShapeShift%3D150E4703DA.preface preserve=yes
//## end GeoShapeShift%3D150E4703DA.preface

//## Class: GeoShapeShift%3D150E4703DA
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoShapeShift : public GeoShape  //## Inherits: <unnamed>%3D150E4B02C8
{
  //## begin GeoShapeShift%3D150E4703DA.initialDeclarations preserve=yes
  //## end GeoShapeShift%3D150E4703DA.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoShapeShift%3D150F480131
      //	Constructor taking two shape operands.
      GeoShapeShift (const GeoShape* A, const HepGeom::Transform3D &X);


    //## Other Operations (specified)
      //## Operation: volume%3D150F48011D
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3D150F48014F
      //	Returns the OR shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3D150F480163
      //	Returns the OR shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: getOp%3D150F480181
      //	Returns the first operand being ORed
      const GeoShape* getOp () const;

      //## Operation: getX%3D150F480195
      //	Returns the shift of this shape.
      const HepGeom::Transform3D & getX () const;

      //## Operation: exec%3DB96AC303B6
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3D150E9F008E
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3D150E9F0098
      //	For type identification.
      static ShapeType getClassTypeID ();

    // Additional Public Declarations
      //## begin GeoShapeShift%3D150E4703DA.public preserve=yes
      //## end GeoShapeShift%3D150E4703DA.public

  protected:
    //## Destructor (generated)
      virtual ~GeoShapeShift();

    // Additional Protected Declarations
      //## begin GeoShapeShift%3D150E4703DA.protected preserve=yes
      //## end GeoShapeShift%3D150E4703DA.protected

  private:
    //## Constructors (generated)
      GeoShapeShift(const GeoShapeShift &right);

    //## Assignment Operation (generated)
      GeoShapeShift & operator=(const GeoShapeShift &right);

    // Data Members for Class Attributes

      //## Attribute: op%3D150E9F0084
      //	The shape operand in the NOT operation.
      //## begin GeoShapeShift::op%3D150E9F0084.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_op;
      //## end GeoShapeShift::op%3D150E9F0084.attr

      //## Attribute: shift%3D150EEF00B1
      //	Gives the amount by which the volume is shifted.
      //## begin GeoShapeShift::shift%3D150EEF00B1.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_shift;
      //## end GeoShapeShift::shift%3D150EEF00B1.attr

      //## begin GeoShapeShift::s_classType%3D150E9F008E.attr preserve=no  public: static const std::string {U} "Shift"
      static const std::string s_classType;
      //## end GeoShapeShift::s_classType%3D150E9F008E.attr

      //## begin GeoShapeShift::s_classTypeID%3D150E9F0098.attr preserve=no  public: static const ShapeType {U} 0x03
      static const ShapeType s_classTypeID;
      //## end GeoShapeShift::s_classTypeID%3D150E9F0098.attr

    // Additional Private Declarations
      //## begin GeoShapeShift%3D150E4703DA.private preserve=yes
      //## end GeoShapeShift%3D150E4703DA.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoShapeShift%3D150E4703DA.implementation preserve=yes
      //## end GeoShapeShift%3D150E4703DA.implementation

};

//## begin GeoShapeShift%3D150E4703DA.postscript preserve=yes
//## end GeoShapeShift%3D150E4703DA.postscript

// Class GeoShapeShift 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoShapeShift::getClassType ()
{
  //## begin GeoShapeShift::getClassType%3D150E9F008E.get preserve=no
  return s_classType;
  //## end GeoShapeShift::getClassType%3D150E9F008E.get
}

inline ShapeType GeoShapeShift::getClassTypeID ()
{
  //## begin GeoShapeShift::getClassTypeID%3D150E9F0098.get preserve=no
  return s_classTypeID;
  //## end GeoShapeShift::getClassTypeID%3D150E9F0098.get
}

//## begin module%3D150E4703DA.epilog preserve=yes
//## end module%3D150E4703DA.epilog


#endif
