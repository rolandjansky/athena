/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD878BB0006.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD878BB0006.cm

//## begin module%3CD878BB0006.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD878BB0006.cp

//## Module: GeoMaterial%3CD878BB0006; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoMaterial.h

#ifndef GeoMaterial_h
#define GeoMaterial_h 1

//## begin module%3CD878BB0006.additionalIncludes preserve=no
//## end module%3CD878BB0006.additionalIncludes

//## begin module%3CD878BB0006.includes preserve=yes
//## end module%3CD878BB0006.includes

// RCBase
#include "GeoModelKernel/RCBase.h"
// GeoElement
#include "GeoModelKernel/GeoElement.h"
//## begin module%3CD878BB0006.additionalDeclarations preserve=yes
#include <vector>
//## end module%3CD878BB0006.additionalDeclarations


//## begin GeoMaterial%3CD878BB0006.preface preserve=yes
//## end GeoMaterial%3CD878BB0006.preface

//## Class: GeoMaterial%3CD878BB0006
//	This class represents materials.  A material is a
//	collection of elements.  In addition it has a density,
//	radiation length, and interaction length, and can
//	provide a description of energy lost which is computed
//	from its composition.
//
//	Also noteworthy:
//
//	* Materials are allocated on the heap and deleted
//	automatically when their reference count falls to zero.
//
//	* After the constructor is called the elements are added
//	one-by-one; when the last one is called the material
//	must be "locked" after the last element is added.
//
//	* The materials are assigned an identifier.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n

class GeoMaterial : public RCBase  //## Inherits: <unnamed>%3CD878BF02A1
{
  //## begin GeoMaterial%3CD878BB0006.initialDeclarations preserve=yes
  //## end GeoMaterial%3CD878BB0006.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoMaterial%3CDA10C001E4
      //	Constructor for the material.
      GeoMaterial (const std::string &Name, double Density);

    //## Other Operations (specified)
      //## Operation: add%3CDA0E1D02A2
      //	Add an element to the material.
      void add (GeoElement* element, double fraction = 1.0);

      //## Operation: add%3CDA0E2003E7
      //	Add another material to the material (this copies all of
      //	the element from the other material into this one).
      //	Fraction is by mass.
      void add (GeoMaterial* material, double fraction);

      //## Operation: lock%3CDA0E250362
      //	Lock the material against the addition of other
      //	materials or elements.
      void lock ();

      //## Operation: getDeDxConstant%3CDA0E3F026F
      //	Constant dEdx term.
      double getDeDxConstant () const;

      //## Operation: getDeDxI0%3CDA0E4A021B
      //	I0 term (ionization potential).  From tables. Units:  eV.
      double getDeDxI0 () const;

      //## Operation: getDeDxMin%3CDA0E5801D5
      //	Get dEdx_min.  dEdxConstant*11.528 Paul Avery, CBX-92-39.
      double getDeDxMin () const;

      //## Operation: getRadLength%3CDAF57E03B0
      //	Returns the radiation length, computed from the density
      //	and the list of constituents, and their properties.
      double getRadLength () const;

      //## Operation: getIntLength%3CDAF58501C6
      //	Return the nuclear interaction length, computed from the
      //	density, the list of constituents, and their properties.
      double getIntLength () const;

      //## Operation: getNumElements%3CDA103F00DA
      unsigned int getNumElements () const;

      //## Operation: getElement%3CDA104F0304
      //	Gets the ith element.
      const GeoElement* getElement (unsigned int i) const;

      //## Operation: getFraction%3CDB2237001D
      //	Gets the fraction by weight of the ith element
      double getFraction (int i) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: name%3CD87BD10157
      //	The name of the material.
      const std::string& getName () const;

      //## Attribute: density%3CD87BD70246
      //	The density of the material.
      const double& getDensity () const;

      //## Attribute: iD%3CD882E40010
      //	Gives an integral identifier for the material.  For
      //	convenience.
      const unsigned int& getID () const;

    // Additional Public Declarations
      //## begin GeoMaterial%3CD878BB0006.public preserve=yes
      //## end GeoMaterial%3CD878BB0006.public

  protected:
    //## Destructor (generated)
      virtual ~GeoMaterial();

    // Additional Protected Declarations
      //## begin GeoMaterial%3CD878BB0006.protected preserve=yes
      //## end GeoMaterial%3CD878BB0006.protected

  private:
    //## Constructors (generated)
      GeoMaterial(const GeoMaterial &right);

    //## Assignment Operation (generated)
      GeoMaterial & operator=(const GeoMaterial &right);

    // Data Members for Class Attributes

      //## begin GeoMaterial::m_name%3CD87BD10157.attr preserve=no  public: std::string {U} Name
      std::string m_name;
      //## end GeoMaterial::m_name%3CD87BD10157.attr

      //## begin GeoMaterial::m_density%3CD87BD70246.attr preserve=no  public: double {U} M_Density
      double m_density;
      //## end GeoMaterial::m_density%3CD87BD70246.attr

      //## begin GeoMaterial::m_iD%3CD882E40010.attr preserve=no  public: unsigned int {U} _lastID++
      unsigned int m_iD;
      //## end GeoMaterial::m_iD%3CD882E40010.attr

      //## Attribute: m_fraction%3CD882410016
      //	A list of the fractional composition of each material.
      //	Fraction is by mass.
      //## begin GeoMaterial::m_fraction%3CD882410016.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_fraction;
      //## end GeoMaterial::m_fraction%3CD882410016.attr

      //## Attribute: m_radLength%3CD87BD9007C
      //	The radiation length of the material.
      //## begin GeoMaterial::m_radLength%3CD87BD9007C.attr preserve=no  private: double {U} 0
      double m_radLength;
      //## end GeoMaterial::m_radLength%3CD87BD9007C.attr

      //## Attribute: m_intLength%3CD87BDB01BF
      //	The interaction length of the material.
      //## begin GeoMaterial::m_intLength%3CD87BDB01BF.attr preserve=no  private: double {U} 0
      double m_intLength;
      //## end GeoMaterial::m_intLength%3CD87BDB01BF.attr

      //## Attribute: s_lastID%3CD8833A01EB
      //	A static used to assign unique identifiers to new
      //	materials.
      //## begin GeoMaterial::s_lastID%3CD8833A01EB.attr preserve=no  private: static unsigned int {U} 0
      static unsigned int s_lastID;
      //## end GeoMaterial::s_lastID%3CD8833A01EB.attr

      //## Attribute: m_dedDxConst%3CDB1FDD0144
      //	The constant term in the formula governing dE/dx.
      //## begin GeoMaterial::m_dedDxConst%3CDB1FDD0144.attr preserve=no  private: double {U} 0
      double m_dedDxConst;
      //## end GeoMaterial::m_dedDxConst%3CDB1FDD0144.attr

      //## Attribute: m_deDxI0%3CDB2028016A
      //	The ionization potential in the formula governing dE/dx.
      //## begin GeoMaterial::m_deDxI0%3CDB2028016A.attr preserve=no  private: double {U} 0
      double m_deDxI0;
      //## end GeoMaterial::m_deDxI0%3CDB2028016A.attr

      //## Attribute: m_locked%3CDA09700200
      //	A flag used to lock the material from further addition
      //	of elements or other constituents.
      //## begin GeoMaterial::m_locked%3CDA09700200.attr preserve=no  private: bool {U} false
      bool m_locked;
      //## end GeoMaterial::m_locked%3CDA09700200.attr

    // Additional Private Declarations
      //## begin GeoMaterial%3CD878BB0006.private preserve=yes
  static const double s_ionizationPotential[93];
      //## end GeoMaterial%3CD878BB0006.private
  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::material composition%3CD87CC503CE
      //## Role: GeoMaterial::m_element%3CD87CC60312
      //	The list of GeoElements composing a GeoMaterial.
      //## begin GeoMaterial::m_element%3CD87CC60312.role preserve=no  public: GeoElement {1 -> nRHgN}
      std::vector<GeoElement *> m_element;
      //## end GeoMaterial::m_element%3CD87CC60312.role

    // Additional Implementation Declarations
      //## begin GeoMaterial%3CD878BB0006.implementation preserve=yes
      //## end GeoMaterial%3CD878BB0006.implementation

};

//## begin GeoMaterial%3CD878BB0006.postscript preserve=yes
//## end GeoMaterial%3CD878BB0006.postscript

// Class GeoMaterial 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoMaterial::getName () const
{
  //## begin GeoMaterial::getName%3CD87BD10157.get preserve=no
  return m_name;
  //## end GeoMaterial::getName%3CD87BD10157.get
}

inline const double& GeoMaterial::getDensity () const
{
  //## begin GeoMaterial::getDensity%3CD87BD70246.get preserve=no
  return m_density;
  //## end GeoMaterial::getDensity%3CD87BD70246.get
}

inline const unsigned int& GeoMaterial::getID () const
{
  //## begin GeoMaterial::getID%3CD882E40010.get preserve=no
  return m_iD;
  //## end GeoMaterial::getID%3CD882E40010.get
}

//## begin module%3CD878BB0006.epilog preserve=yes
//## end module%3CD878BB0006.epilog


#endif
