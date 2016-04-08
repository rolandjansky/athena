/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD86EAF030B.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD86EAF030B.cm

//## begin module%3CD86EAF030B.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD86EAF030B.cp

//## Module: GeoElement%3CD86EAF030B; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoElement.h

#ifndef GeoElement_h
#define GeoElement_h 1

//## begin module%3CD86EAF030B.additionalIncludes preserve=no
//## end module%3CD86EAF030B.additionalIncludes

//## begin module%3CD86EAF030B.includes preserve=yes
#include <string>
//## end module%3CD86EAF030B.includes

// RCBase
#include "GeoModelKernel/RCBase.h"
//## begin module%3CD86EAF030B.additionalDeclarations preserve=yes
#include "CLHEP/Units/PhysicalConstants.h"
//## end module%3CD86EAF030B.additionalDeclarations


//## begin GeoElement%3CD86EAF030B.preface preserve=yes
//## end GeoElement%3CD86EAF030B.preface

//## Class: GeoElement%3CD86EAF030B
//	This class represents chemical elements.  Elements are
//	allocated on the heap and deleted automatically when
//	their reference count falls to zero.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoElement : public RCBase  //## Inherits: <unnamed>%3CD86EC600C9
{
  //## begin GeoElement%3CD86EAF030B.initialDeclarations preserve=yes
  //## end GeoElement%3CD86EAF030B.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoElement%3CD87237027F
      //	Constructor for the element.
      GeoElement (const std::string &Name, const std::string &Symbol, double Z, double A);

    //## Equality Operations (generated)
      int operator==(const GeoElement &right) const;

      int operator!=(const GeoElement &right) const;


    //## Other Operations (specified)
      //## Operation: getN%3CDBB41F0065
      //	Returns the effective number of nucleons in the element.
      double getN () const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: name%3CD8701C0252
      //	The name of the element, e.g. "Carbon".
      const std::string& getName () const;

      //## Attribute: symbol%3CD870230125
      //	The chemical symbol for the element, e.g. C, O, S, Na....
      const std::string& getSymbol () const;

      //## Attribute: z%3CD870290051
      //	The atomic number Z for the material.
      const double& getZ () const;

      //## Attribute: a%3CD8703000CA
      //	The average atomic mass for the element.
      const double& getA () const;

      //        Tsai formula for the radiation length
      double getRadTsai () const;

    // Additional Public Declarations
      //## begin GeoElement%3CD86EAF030B.public preserve=yes
      //## end GeoElement%3CD86EAF030B.public

  protected:
    //## Destructor (generated)
      virtual ~GeoElement();

    // Additional Protected Declarations
      //## begin GeoElement%3CD86EAF030B.protected preserve=yes
      //## end GeoElement%3CD86EAF030B.protected

  private:
    //## Constructors (generated)
      GeoElement(const GeoElement &right);

    //## Assignment Operation (generated)
      GeoElement & operator=(const GeoElement &right);

    // Data Members for Class Attributes

      //## begin GeoElement::m_name%3CD8701C0252.attr preserve=no  public: std::string {U} 
      std::string m_name;
      //## end GeoElement::m_name%3CD8701C0252.attr

      //## begin GeoElement::m_symbol%3CD870230125.attr preserve=no  public: std::string {U} 
      std::string m_symbol;
      //## end GeoElement::m_symbol%3CD870230125.attr

      //## begin GeoElement::m_z%3CD870290051.attr preserve=no  public: double {U} 
      double m_z;
      //## end GeoElement::m_z%3CD870290051.attr

      //## begin GeoElement::m_a%3CD8703000CA.attr preserve=no  public: double {U} 
      double m_a;
      //## end GeoElement::m_a%3CD8703000CA.attr

    // Additional Private Declarations
      //## begin GeoElement%3CD86EAF030B.private preserve=yes
      //## end GeoElement%3CD86EAF030B.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoElement%3CD86EAF030B.implementation preserve=yes
      //## end GeoElement%3CD86EAF030B.implementation

};

//## begin GeoElement%3CD86EAF030B.postscript preserve=yes
//## end GeoElement%3CD86EAF030B.postscript

// Class GeoElement 

inline int GeoElement::operator==(const GeoElement &right) const
{
  //## begin GeoElement::operator==%3CD86EAF030B_eq.body preserve=yes
  return
    m_name ==
    right.m_name && m_symbol == right.m_symbol && m_z == right.m_z && m_a == right.m_a;
  //## end GeoElement::operator==%3CD86EAF030B_eq.body
}

inline int GeoElement::operator!=(const GeoElement &right) const
{
  //## begin GeoElement::operator!=%3CD86EAF030B_neq.body preserve=yes
  return m_name != right.m_name ||
    m_symbol != right.m_symbol || m_z != right.m_z || m_a != right.m_a;
  //## end GeoElement::operator!=%3CD86EAF030B_neq.body
}



//## Other Operations (inline)
inline double GeoElement::getN () const
{
  //## begin GeoElement::getN%3CDBB41F0065.body preserve=yes
  return m_a * (CLHEP::mole / CLHEP::gram);
  //## end GeoElement::getN%3CDBB41F0065.body
}

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoElement::getName () const
{
  //## begin GeoElement::getName%3CD8701C0252.get preserve=no
  return m_name;
  //## end GeoElement::getName%3CD8701C0252.get
}

inline const std::string& GeoElement::getSymbol () const
{
  //## begin GeoElement::getSymbol%3CD870230125.get preserve=no
  return m_symbol;
  //## end GeoElement::getSymbol%3CD870230125.get
}

inline const double& GeoElement::getZ () const
{
  //## begin GeoElement::getZ%3CD870290051.get preserve=no
  return m_z;
  //## end GeoElement::getZ%3CD870290051.get
}

inline const double& GeoElement::getA () const
{
  //## begin GeoElement::getA%3CD8703000CA.get preserve=no
  return m_a;
  //## end GeoElement::getA%3CD8703000CA.get
}

//## begin module%3CD86EAF030B.epilog preserve=yes
//## end module%3CD86EAF030B.epilog


#endif
