/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A40B0223.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A40B0223.cm

//## begin module%3CD2A40B0223.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2A40B0223.cp

//## Module: GeoPgon%3CD2A40B0223; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPgon.h

#ifndef GeoPgon_h
#define GeoPgon_h 1

//## begin module%3CD2A40B0223.additionalIncludes preserve=no
//## end module%3CD2A40B0223.additionalIncludes

//## begin module%3CD2A40B0223.includes preserve=yes
#include <vector>
//## end module%3CD2A40B0223.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2A40B0223.additionalDeclarations preserve=yes
//## end module%3CD2A40B0223.additionalDeclarations


//## begin GeoPgon%3CD2A40B0223.preface preserve=yes
//## end GeoPgon%3CD2A40B0223.preface

//## Class: GeoPgon%3CD2A40B0223
//	This shape represents a polygon
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPgon : public GeoShape  //## Inherits: <unnamed>%3CD2A40B022F
{
  //## begin GeoPgon%3CD2A40B0223.initialDeclarations preserve=yes
  //## end GeoPgon%3CD2A40B0223.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoPgon%3CD5AA3E029D
      //	Constructor for the PGON.  Note that the constructor
      //	does not fully build this object.  The PGON is not valid
      //	until at least two polygon planes have been added.
      GeoPgon (double SPhi, double DPhi, unsigned int NSides);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A6E600EE
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A8480219
      //	Returns the PGON shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A8480237
      //	Returns the PGON shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: addPlane%3CD5AA3E0243
      //	Add another plane to the polygon.  A minimum of two
      //	planes are required to create a valid polygon.
      void addPlane (double ZPlane, double RMinPlane, double RMaxPlane);

      //## Operation: getNPlanes%3CD5AA3E024D
      //	Returns the number of planes that have been created for
      //	the polygon.
      unsigned int getNPlanes () const;

      //## Operation: isValid%3CD5AA3E0261
      //	True if the polygon has at least two planes.  False
      //	otherwise.
      bool isValid () const;

      //## Operation: getZPlane%3CD5AA3E026B
      //	Get the Z Position of the specified plane.
      const double & getZPlane (unsigned int i) const;

      //## Operation: getRMinPlane%3CD5AA3E027F
      //	Get the RMin of the specified plane.
      const double & getRMinPlane (unsigned int i) const;

      //## Operation: getRMaxPlane%3CD5AA3E0293
      //	Get the Z Position of the specified plane.
      const double & getRMaxPlane (unsigned int i) const;

      //## Operation: exec%3DB96A6E00B0
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2A40B022D
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2A40B022E
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: sPhi%3CD5AC050360
      //	Starting angle of the segment in radians.
      const double& getSPhi () const;

      //## Attribute: dPhi%3CD5AC05036A
      //	Delta angle of the segment in radians.
      const double& getDPhi () const;

      //## Attribute: nSides%3CD5AC9100AE
      //	Number of sides in each polygonal segment.
      const unsigned int& getNSides () const;

    // Additional Public Declarations
      //## begin GeoPgon%3CD2A40B0223.public preserve=yes
      //## end GeoPgon%3CD2A40B0223.public

  protected:
    //## Destructor (generated)
      virtual ~GeoPgon();

    // Additional Protected Declarations
      //## begin GeoPgon%3CD2A40B0223.protected preserve=yes
      //## end GeoPgon%3CD2A40B0223.protected

  private:
    //## Constructors (generated)
      GeoPgon(const GeoPgon &right);

    //## Assignment Operation (generated)
      GeoPgon & operator=(const GeoPgon &right);

    // Data Members for Class Attributes

      //## begin GeoPgon::s_classType%3CD2A40B022D.attr preserve=no  public: static const std::string {U} "Pgon"
      static const std::string s_classType;
      //## end GeoPgon::s_classType%3CD2A40B022D.attr

      //## begin GeoPgon::s_classTypeID%3CD2A40B022E.attr preserve=no  public: static const ShapeType {U} 0x14
      static const ShapeType s_classTypeID;
      //## end GeoPgon::s_classTypeID%3CD2A40B022E.attr

      //## begin GeoPgon::m_sPhi%3CD5AC050360.attr preserve=no  public: double {U} 
      double m_sPhi;
      //## end GeoPgon::m_sPhi%3CD5AC050360.attr

      //## begin GeoPgon::m_dPhi%3CD5AC05036A.attr preserve=no  public: double {U} 
      double m_dPhi;
      //## end GeoPgon::m_dPhi%3CD5AC05036A.attr

      //## begin GeoPgon::m_nSides%3CD5AC9100AE.attr preserve=no  public: unsigned int {U} 
      unsigned int m_nSides;
      //## end GeoPgon::m_nSides%3CD5AC9100AE.attr

      //## Attribute: m_zPlane%3CD5AC050374
      //	Z Position of polygon planes.
      //## begin GeoPgon::m_zPlane%3CD5AC050374.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_zPlane;
      //## end GeoPgon::m_zPlane%3CD5AC050374.attr

      //## Attribute: m_rMinPlane%3CD5AC05037E
      //	Minimum radius of polygon planes.
      //## begin GeoPgon::m_rMinPlane%3CD5AC05037E.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_rMinPlane;
      //## end GeoPgon::m_rMinPlane%3CD5AC05037E.attr

      //## Attribute: m_rMaxPlane%3CD5AC05037F
      //	Maximum radius of polygon planes.
      //## begin GeoPgon::m_rMaxPlane%3CD5AC05037F.attr preserve=no  private: std::vector<double> {U} 
      std::vector<double> m_rMaxPlane;
      //## end GeoPgon::m_rMaxPlane%3CD5AC05037F.attr

    // Additional Private Declarations
      //## begin GeoPgon%3CD2A40B0223.private preserve=yes
      //## end GeoPgon%3CD2A40B0223.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPgon%3CD2A40B0223.implementation preserve=yes
      //## end GeoPgon%3CD2A40B0223.implementation

};

//## begin GeoPgon%3CD2A40B0223.postscript preserve=yes
//## end GeoPgon%3CD2A40B0223.postscript

// Class GeoPgon 


//## Other Operations (inline)
inline unsigned int GeoPgon::getNPlanes () const
{
  //## begin GeoPgon::getNPlanes%3CD5AA3E024D.body preserve=yes
  return m_zPlane.size ();
  //## end GeoPgon::getNPlanes%3CD5AA3E024D.body
}

inline bool GeoPgon::isValid () const
{
  //## begin GeoPgon::isValid%3CD5AA3E0261.body preserve=yes
  return m_zPlane.size () >= 2;
  //## end GeoPgon::isValid%3CD5AA3E0261.body
}

inline const double & GeoPgon::getZPlane (unsigned int i) const
{
  //## begin GeoPgon::getZPlane%3CD5AA3E026B.body preserve=yes
  return m_zPlane[i];
  //## end GeoPgon::getZPlane%3CD5AA3E026B.body
}

inline const double & GeoPgon::getRMinPlane (unsigned int i) const
{
  //## begin GeoPgon::getRMinPlane%3CD5AA3E027F.body preserve=yes
  return m_rMinPlane[i];
  //## end GeoPgon::getRMinPlane%3CD5AA3E027F.body
}

inline const double & GeoPgon::getRMaxPlane (unsigned int i) const
{
  //## begin GeoPgon::getRMaxPlane%3CD5AA3E0293.body preserve=yes
  return m_rMaxPlane[i];
  //## end GeoPgon::getRMaxPlane%3CD5AA3E0293.body
}

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoPgon::getClassType ()
{
  //## begin GeoPgon::getClassType%3CD2A40B022D.get preserve=no
  return s_classType;
  //## end GeoPgon::getClassType%3CD2A40B022D.get
}

inline ShapeType GeoPgon::getClassTypeID ()
{
  //## begin GeoPgon::getClassTypeID%3CD2A40B022E.get preserve=no
  return s_classTypeID;
  //## end GeoPgon::getClassTypeID%3CD2A40B022E.get
}

inline const double& GeoPgon::getSPhi () const
{
  //## begin GeoPgon::getSPhi%3CD5AC050360.get preserve=no
  return m_sPhi;
  //## end GeoPgon::getSPhi%3CD5AC050360.get
}

inline const double& GeoPgon::getDPhi () const
{
  //## begin GeoPgon::getDPhi%3CD5AC05036A.get preserve=no
  return m_dPhi;
  //## end GeoPgon::getDPhi%3CD5AC05036A.get
}

inline const unsigned int& GeoPgon::getNSides () const
{
  //## begin GeoPgon::getNSides%3CD5AC9100AE.get preserve=no
  return m_nSides;
  //## end GeoPgon::getNSides%3CD5AC9100AE.get
}

//## begin module%3CD2A40B0223.epilog preserve=yes
//## end module%3CD2A40B0223.epilog


#endif
