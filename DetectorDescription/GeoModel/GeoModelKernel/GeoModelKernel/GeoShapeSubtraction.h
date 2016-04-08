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
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2595103E1.cp

//## Module: GeoShapeSubtraction%3CD2595103E1; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoShapeSubtraction.h

#ifndef GeoShapeSubtraction_h
#define GeoShapeSubtraction_h 1

//## begin module%3CD2595103E1.additionalIncludes preserve=no
//## end module%3CD2595103E1.additionalIncludes

//## begin module%3CD2595103E1.includes preserve=yes
//## end module%3CD2595103E1.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2595103E1.additionalDeclarations preserve=yes
//## end module%3CD2595103E1.additionalDeclarations


//## begin GeoShapeSubtraction%3CD2595103E1.preface preserve=yes
//## end GeoShapeSubtraction%3CD2595103E1.preface

//## Class: GeoShapeSubtraction%3CD2595103E1
//	A GeoShape representing the NOT of a shape.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoShapeSubtraction : public GeoShape  //## Inherits: <unnamed>%3CD2595103E2
{
  //## begin GeoShapeSubtraction%3CD2595103E1.initialDeclarations preserve=yes
  //## end GeoShapeSubtraction%3CD2595103E1.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoShapeSubtraction%3CD2595103E5
      //	Constructor taking a shape operand.
      GeoShapeSubtraction (const GeoShape* A, const GeoShape* B);


    //## Other Operations (specified)
      //## Operation: volume%3CD2595103E4
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2595103E7
      //	Returns the NOT shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2595103E8
      //	Returns the NOT shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: getOpA%3CE7F07603D1
      //	Returns the first operand in the subtraction
      const GeoShape* getOpA () const;

      //## Operation: getOpB%3D1DD1E7025E
      //	Returns the second operand in the subtraction
      const GeoShape* getOpB () const;

      //## Operation: exec%3DB96AAE0171
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2595103EC
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2595103ED
      //	For type identification.
      static ShapeType getClassTypeID ();

    // Additional Public Declarations
      //## begin GeoShapeSubtraction%3CD2595103E1.public preserve=yes
      //## end GeoShapeSubtraction%3CD2595103E1.public

  protected:
    //## Destructor (generated)
      virtual ~GeoShapeSubtraction();

    // Additional Protected Declarations
      //## begin GeoShapeSubtraction%3CD2595103E1.protected preserve=yes
      //## end GeoShapeSubtraction%3CD2595103E1.protected

  private:
    //## Constructors (generated)
      GeoShapeSubtraction(const GeoShapeSubtraction &right);

    //## Assignment Operation (generated)
      GeoShapeSubtraction & operator=(const GeoShapeSubtraction &right);

    // Data Members for Class Attributes

      //## Attribute: m_opA%3CD2595103EB
      //	The shape operand in the Subtraction operation
      //## begin GeoShapeSubtraction::m_opA%3CD2595103EB.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_opA;
      //## end GeoShapeSubtraction::m_opA%3CD2595103EB.attr

      //## Attribute: m_opB%3D1DD1980319
      //	The shape operand in the Subtraction operation
      //## begin GeoShapeSubtraction::m_opB%3D1DD1980319.attr preserve=no  private: const GeoShape* {U} 
      const GeoShape* m_opB;
      //## end GeoShapeSubtraction::m_opB%3D1DD1980319.attr

      //## begin GeoShapeSubtraction::s_classType%3CD2595103EC.attr preserve=no  public: static const std::string {U} "Subtraction"
      static const std::string s_classType;
      //## end GeoShapeSubtraction::s_classType%3CD2595103EC.attr

      //## begin GeoShapeSubtraction::s_classTypeID%3CD2595103ED.attr preserve=no  public: static const ShapeType {U} 0x02
      static const ShapeType s_classTypeID;
      //## end GeoShapeSubtraction::s_classTypeID%3CD2595103ED.attr

    // Additional Private Declarations
      //## begin GeoShapeSubtraction%3CD2595103E1.private preserve=yes
      //## end GeoShapeSubtraction%3CD2595103E1.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoShapeSubtraction%3CD2595103E1.implementation preserve=yes
      //## end GeoShapeSubtraction%3CD2595103E1.implementation

};

//## begin GeoShapeSubtraction%3CD2595103E1.postscript preserve=yes
//## end GeoShapeSubtraction%3CD2595103E1.postscript

// Class GeoShapeSubtraction 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoShapeSubtraction::getClassType ()
{
  //## begin GeoShapeSubtraction::getClassType%3CD2595103EC.get preserve=no
  return s_classType;
  //## end GeoShapeSubtraction::getClassType%3CD2595103EC.get
}

inline ShapeType GeoShapeSubtraction::getClassTypeID ()
{
  //## begin GeoShapeSubtraction::getClassTypeID%3CD2595103ED.get preserve=no
  return s_classTypeID;
  //## end GeoShapeSubtraction::getClassTypeID%3CD2595103ED.get
}

//## begin module%3CD2595103E1.epilog preserve=yes
//## end module%3CD2595103E1.epilog


#endif
