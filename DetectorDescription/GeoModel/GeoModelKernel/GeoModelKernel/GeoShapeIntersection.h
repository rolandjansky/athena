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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD259190322.cp

//## Module: GeoShapeIntersection%3CD259190322; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeIntersection.h

#ifndef GeoShapeIntersection_h
#define GeoShapeIntersection_h 1

//## begin module%3CD259190322.additionalIncludes preserve=no
//## end module%3CD259190322.additionalIncludes

//## begin module%3CD259190322.includes preserve=yes
//## end module%3CD259190322.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD259190322.additionalDeclarations preserve=yes
//## end module%3CD259190322.additionalDeclarations


//## begin GeoShapeIntersection%3CD259190322.preface preserve=yes
//## end GeoShapeIntersection%3CD259190322.preface

//## Class: GeoShapeIntersection%3CD259190322
//	A GeoShape representing the Intersection of two shapes.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoShapeIntersection : public GeoShape  //## Inherits: <unnamed>%3CD259190323
{
  //## begin GeoShapeIntersection%3CD259190322.initialDeclarations preserve=yes
  //## end GeoShapeIntersection%3CD259190322.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoShapeIntersection%3CD259190327
      //	Constructor taking two shape operands.
      GeoShapeIntersection (const GeoShape* A, const GeoShape* B);


    //## Other Operations (specified)
      //## Operation: volume%3CD259190326
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD25919032A
      //	Returns the AND shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD25919032B
      //	Returns the AND shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: getOpA%3CE7F14F02C4
      //	Returns the first operand being ANDed
      const GeoShape* getOpA () const;

      //## Operation: getOpB%3CE7F1510317
      //	Returns the second operand being ANDed.
      const GeoShape* getOpB () const;

      //## Operation: exec%3DB96AA102BC
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD259190330
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD259190331
      //	For type identification.
      static ShapeType getClassTypeID ();

    // Additional Public Declarations
      //## begin GeoShapeIntersection%3CD259190322.public preserve=yes
      //## end GeoShapeIntersection%3CD259190322.public

  protected:
    //## Destructor (generated)
      virtual ~GeoShapeIntersection();

    // Additional Protected Declarations
      //## begin GeoShapeIntersection%3CD259190322.protected preserve=yes
      //## end GeoShapeIntersection%3CD259190322.protected

  private:
    //## Constructors (generated)
      GeoShapeIntersection(const GeoShapeIntersection &right);

    //## Assignment Operation (generated)
      GeoShapeIntersection & operator=(const GeoShapeIntersection &right);

    // Data Members for Class Attributes

      //## Attribute: m_opA%3CD25919032E
      //	The first shape operand in the AND operation.
      //## begin GeoShapeIntersection::m_opA%3CD25919032E.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_opA;
      //## end GeoShapeIntersection::m_opA%3CD25919032E.attr

      //## Attribute: m_opB%3CD25919032F
      //	The second shape operand in the AND operation.
      //## begin GeoShapeIntersection::m_opB%3CD25919032F.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_opB;
      //## end GeoShapeIntersection::m_opB%3CD25919032F.attr

      //## begin GeoShapeIntersection::s_classType%3CD259190330.attr preserve=no  public: static const std::string {U} "Intersection"
      static const std::string s_classType;
      //## end GeoShapeIntersection::s_classType%3CD259190330.attr

      //## begin GeoShapeIntersection::s_classTypeID%3CD259190331.attr preserve=no  public: static const ShapeType {U} 0x00
      static const ShapeType s_classTypeID;
      //## end GeoShapeIntersection::s_classTypeID%3CD259190331.attr

    // Additional Private Declarations
      //## begin GeoShapeIntersection%3CD259190322.private preserve=yes
      //## end GeoShapeIntersection%3CD259190322.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoShapeIntersection%3CD259190322.implementation preserve=yes
      //## end GeoShapeIntersection%3CD259190322.implementation

};

//## begin GeoShapeIntersection%3CD259190322.postscript preserve=yes
//## end GeoShapeIntersection%3CD259190322.postscript

// Class GeoShapeIntersection 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoShapeIntersection::getClassType ()
{
  //## begin GeoShapeIntersection::getClassType%3CD259190330.get preserve=no
  return s_classType;
  //## end GeoShapeIntersection::getClassType%3CD259190330.get
}

inline ShapeType GeoShapeIntersection::getClassTypeID ()
{
  //## begin GeoShapeIntersection::getClassTypeID%3CD259190331.get preserve=no
  return s_classTypeID;
  //## end GeoShapeIntersection::getClassTypeID%3CD259190331.get
}

//## begin module%3CD259190322.epilog preserve=yes
//## end module%3CD259190322.epilog


#endif
