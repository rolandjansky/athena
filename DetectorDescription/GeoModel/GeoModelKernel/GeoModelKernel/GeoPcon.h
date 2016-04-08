/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD26DBC00BB.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD26DBC00BB.cm

//## begin module%3CD26DBC00BB.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD26DBC00BB.cp

//## Module: GeoPcon%3CD26DBC00BB; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPcon.h

#ifndef GeoPcon_h
#define GeoPcon_h 1

//## begin module%3CD26DBC00BB.additionalIncludes preserve=no
//## end module%3CD26DBC00BB.additionalIncludes

//## begin module%3CD26DBC00BB.includes preserve=yes
#include <vector>
//## end module%3CD26DBC00BB.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD26DBC00BB.additionalDeclarations preserve=yes
//## end module%3CD26DBC00BB.additionalDeclarations


//## begin GeoPcon%3CD26DBC00BB.preface preserve=yes
//## end GeoPcon%3CD26DBC00BB.preface

//## Class: GeoPcon%3CD26DBC00BB
//	This shape represents a polycone.
//
//	Specify the starting angle and delta angle (subtended
//	angle)
//	of the polycone first, Then add at least two planes with
//	the
//	addPlane( double zPlane, double rInner,  double rOuter)
//	method.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPcon : public GeoShape  //## Inherits: <unnamed>%3CD26DBC00BE
{
  //## begin GeoPcon%3CD26DBC00BB.initialDeclarations preserve=yes
  //## end GeoPcon%3CD26DBC00BB.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoPcon%3CD5A48D037F
      //	Constructor for the PCON.  Note that the constructor
      //	does not fully build this object.  The PCON is not valid
      //	until at least two polygon planes have been added.
      GeoPcon (double SPhi, double DPhi);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A6E0030C
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A842029D
      //	Returns the PCON shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A84202BB
      //	Returns the PCON shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: addPlane%3CD59FF902C6
      //	Add another plane to the polycone  A minimum of two
      //	planes are required to create a valid polycone.
      void addPlane (double ZPlane, double RMinPlane, double RMaxPlane);

      //## Operation: getNPlanes%3CD59FFD03E4
      //	Returns the number of planes that have been created for
      //	the polycone.
      unsigned int getNPlanes () const;

      //## Operation: isValid%3CD5A13402C9
      //	True if the polycone has at least two planes.  False
      //	otherwise.
      bool isValid () const;

      //## Operation: getZPlane%3CD5A16B0282
      //	Get the Z Position of the specified plane.
      const double & getZPlane (unsigned int i) const;

      //## Operation: getRMinPlane%3CD5A1C30207
      //	Get the RMin of the specified plane.
      const double & getRMinPlane (unsigned int i) const;

      //## Operation: getRMaxPlane%3CD5A1C600F2
      //	Get the Z Position of the specified plane.
      const double & getRMaxPlane (unsigned int i) const;

      //## Operation: exec%3DB96A4901F8
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD26DBC00BC
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD26DBC00BD
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: m_sPhi%3CD59E1403D7
      //	Starting angle of the segment in radians.
      const double& getSPhi () const;

      //## Attribute: dPhi%3CD59E4002B8
      //	Delta angle of the segment in radians.
      const double& getDPhi () const;

    // Additional Public Declarations
      //## begin GeoPcon%3CD26DBC00BB.public preserve=yes
      //## end GeoPcon%3CD26DBC00BB.public

  protected:
    //## Destructor (generated)
      virtual ~GeoPcon();

    // Additional Protected Declarations
      //## begin GeoPcon%3CD26DBC00BB.protected preserve=yes
      //## end GeoPcon%3CD26DBC00BB.protected

  private:
    //## Constructors (generated)
      GeoPcon(const GeoPcon &right);

    //## Assignment Operation (generated)
      GeoPcon & operator=(const GeoPcon &right);

    // Data Members for Class Attributes

      //## begin GeoPcon::s_classType%3CD26DBC00BC.attr preserve=no  public: static const std::string {U} "Pcon"
      static const std::string s_classType;
      //## end GeoPcon::s_classType%3CD26DBC00BC.attr

      //## begin GeoPcon::s_classTypeID%3CD26DBC00BD.attr preserve=no  public: static const ShapeType {U} 0x13
      static const ShapeType s_classTypeID;
      //## end GeoPcon::s_classTypeID%3CD26DBC00BD.attr

      //## begin GeoPcon::m_sPhi%3CD59E1403D7.attr preserve=no  public: double {U} 
      double m_sPhi;
      //## end GeoPcon::m_sPhi%3CD59E1403D7.attr

      //## begin GeoPcon::m_dPhi%3CD59E4002B8.attr preserve=no  public: double {U} 
      double m_dPhi;
      //## end GeoPcon::m_dPhi%3CD59E4002B8.attr

      //## Attribute: m_zPlane%3CD59F2103CA
      //	Z Position of poly-cone planes.
      //## begin GeoPcon::m_zPlane%3CD59F2103CA.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_zPlane;
      //## end GeoPcon::m_zPlane%3CD59F2103CA.attr

      //## Attribute: m_rMinPlane%3CD59F690098
      //	Minimum radius of poly-cone planes.
      //## begin GeoPcon::m_rMinPlane%3CD59F690098.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_rMinPlane;
      //## end GeoPcon::m_rMinPlane%3CD59F690098.attr

      //## Attribute: m_rMaxPlane%3CD59F6B00A5
      //	Maximum radius of poly-cone planes.
      //## begin GeoPcon::m_rMaxPlane%3CD59F6B00A5.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_rMaxPlane;
      //## end GeoPcon::m_rMaxPlane%3CD59F6B00A5.attr

    // Additional Private Declarations
      //## begin GeoPcon%3CD26DBC00BB.private preserve=yes
      //## end GeoPcon%3CD26DBC00BB.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPcon%3CD26DBC00BB.implementation preserve=yes
      //## end GeoPcon%3CD26DBC00BB.implementation

};

//## begin GeoPcon%3CD26DBC00BB.postscript preserve=yes
//## end GeoPcon%3CD26DBC00BB.postscript

// Class GeoPcon 


//## Other Operations (inline)
inline unsigned int GeoPcon::getNPlanes () const
{
  //## begin GeoPcon::getNPlanes%3CD59FFD03E4.body preserve=yes
  return m_zPlane.size ();
  //## end GeoPcon::getNPlanes%3CD59FFD03E4.body
}

inline bool GeoPcon::isValid () const
{
  //## begin GeoPcon::isValid%3CD5A13402C9.body preserve=yes
  return getNPlanes () >= 2;
  //## end GeoPcon::isValid%3CD5A13402C9.body
}

inline const double & GeoPcon::getZPlane (unsigned int i) const
{
  //## begin GeoPcon::getZPlane%3CD5A16B0282.body preserve=yes
  return m_zPlane[i];
  //## end GeoPcon::getZPlane%3CD5A16B0282.body
}

inline const double & GeoPcon::getRMinPlane (unsigned int i) const
{
  //## begin GeoPcon::getRMinPlane%3CD5A1C30207.body preserve=yes
  return m_rMinPlane[i];
  //## end GeoPcon::getRMinPlane%3CD5A1C30207.body
}

inline const double & GeoPcon::getRMaxPlane (unsigned int i) const
{
  //## begin GeoPcon::getRMaxPlane%3CD5A1C600F2.body preserve=yes
  return m_rMaxPlane[i];
  //## end GeoPcon::getRMaxPlane%3CD5A1C600F2.body
}

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoPcon::getClassType ()
{
  //## begin GeoPcon::getClassType%3CD26DBC00BC.get preserve=no
  return s_classType;
  //## end GeoPcon::getClassType%3CD26DBC00BC.get
}

inline ShapeType GeoPcon::getClassTypeID ()
{
  //## begin GeoPcon::getClassTypeID%3CD26DBC00BD.get preserve=no
  return s_classTypeID;
  //## end GeoPcon::getClassTypeID%3CD26DBC00BD.get
}

inline const double& GeoPcon::getSPhi () const
{
  //## begin GeoPcon::getSPhi%3CD59E1403D7.get preserve=no
  return m_sPhi;
  //## end GeoPcon::getSPhi%3CD59E1403D7.get
}

inline const double& GeoPcon::getDPhi () const
{
  //## begin GeoPcon::getDPhi%3CD59E4002B8.get preserve=no
  return m_dPhi;
  //## end GeoPcon::getDPhi%3CD59E4002B8.get
}

//## begin module%3CD26DBC00BB.epilog preserve=yes
//## end module%3CD26DBC00BB.epilog


#endif
