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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2596A0198.cp

//## Module: GeoShapeUnion%3CD2596A0198; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeUnion.h

#ifndef GeoShapeUnion_h
#define GeoShapeUnion_h 1

//## begin module%3CD2596A0198.additionalIncludes preserve=no
//## end module%3CD2596A0198.additionalIncludes

//## begin module%3CD2596A0198.includes preserve=yes
//## end module%3CD2596A0198.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2596A0198.additionalDeclarations preserve=yes
//## end module%3CD2596A0198.additionalDeclarations


//## begin GeoShapeUnion%3CD2596A0198.preface preserve=yes
//## end GeoShapeUnion%3CD2596A0198.preface

//## Class: GeoShapeUnion%3CD2596A0198
//	A GeoShape representing the Union of two shapes.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoShapeUnion : public GeoShape  //## Inherits: <unnamed>%3CD2596A0199
{
  //## begin GeoShapeUnion%3CD2596A0198.initialDeclarations preserve=yes
  //## end GeoShapeUnion%3CD2596A0198.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoShapeUnion%3CD2596A019D
      //	Constructor taking two shape operands.
      GeoShapeUnion (const GeoShape* A, const GeoShape* B);


    //## Other Operations (specified)
      //## Operation: volume%3CD2596A019C
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2596A01A0
      //	Returns the OR shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2596A01A1
      //	Returns the OR shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: getOpA%3CE7F18900AB
      //	Returns the first operand being ORed
      const GeoShape* getOpA () const;

      //## Operation: getOpB%3CE7F18900BF
      //	Returns the second operand being ORed.
      const GeoShape* getOpB () const;

      //## Operation: exec%3DB96AB90203
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2596A01A6
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2596A01A7
      //	For type identification.
      static ShapeType getClassTypeID ();

    // Additional Public Declarations
      //## begin GeoShapeUnion%3CD2596A0198.public preserve=yes
      //## end GeoShapeUnion%3CD2596A0198.public

  protected:
    //## Destructor (generated)
      virtual ~GeoShapeUnion();

    // Additional Protected Declarations
      //## begin GeoShapeUnion%3CD2596A0198.protected preserve=yes
      //## end GeoShapeUnion%3CD2596A0198.protected

  private:
    //## Constructors (generated)
      GeoShapeUnion(const GeoShapeUnion &right);

    //## Assignment Operation (generated)
      GeoShapeUnion & operator=(const GeoShapeUnion &right);

    // Data Members for Class Attributes

      //## Attribute: m_opA%3CD2596A01A4
      //	The first shape operand in the OR operation.
      //## begin GeoShapeUnion::m_opA%3CD2596A01A4.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_opA;
      //## end GeoShapeUnion::m_opA%3CD2596A01A4.attr

      //## Attribute: m_opB%3CD2596A01A5
      //	The second shape operand in the OR operation.
      //## begin GeoShapeUnion::m_opB%3CD2596A01A5.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_opB;
      //## end GeoShapeUnion::m_opB%3CD2596A01A5.attr

      //## begin GeoShapeUnion::s_classType%3CD2596A01A6.attr preserve=no  public: static const std::string {U} "Union"
      static const std::string s_classType;
      //## end GeoShapeUnion::s_classType%3CD2596A01A6.attr

      //## begin GeoShapeUnion::s_classTypeID%3CD2596A01A7.attr preserve=no  public: static const ShapeType {U} 0x01
      static const ShapeType s_classTypeID;
      //## end GeoShapeUnion::s_classTypeID%3CD2596A01A7.attr

    // Additional Private Declarations
      //## begin GeoShapeUnion%3CD2596A0198.private preserve=yes
      //## end GeoShapeUnion%3CD2596A0198.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoShapeUnion%3CD2596A0198.implementation preserve=yes
      //## end GeoShapeUnion%3CD2596A0198.implementation

};

//## begin GeoShapeUnion%3CD2596A0198.postscript preserve=yes
//## end GeoShapeUnion%3CD2596A0198.postscript

// Class GeoShapeUnion 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoShapeUnion::getClassType ()
{
  //## begin GeoShapeUnion::getClassType%3CD2596A01A6.get preserve=no
  return s_classType;
  //## end GeoShapeUnion::getClassType%3CD2596A01A6.get
}

inline ShapeType GeoShapeUnion::getClassTypeID ()
{
  //## begin GeoShapeUnion::getClassTypeID%3CD2596A01A7.get preserve=no
  return s_classTypeID;
  //## end GeoShapeUnion::getClassTypeID%3CD2596A01A7.get
}

//## begin module%3CD2596A0198.epilog preserve=yes
//## end module%3CD2596A0198.epilog


#endif
