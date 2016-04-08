/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A59100F8.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A59100F8.cm

//## begin module%3CD2A59100F8.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2A59100F8.cp

//## Module: GeoTrd%3CD2A59100F8; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTrd.h

#ifndef GeoTrd_h
#define GeoTrd_h 1

//## begin module%3CD2A59100F8.additionalIncludes preserve=no
//## end module%3CD2A59100F8.additionalIncludes

//## begin module%3CD2A59100F8.includes preserve=yes
//## end module%3CD2A59100F8.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2A59100F8.additionalDeclarations preserve=yes
//## end module%3CD2A59100F8.additionalDeclarations


//## begin GeoTrd%3CD2A59100F8.preface preserve=yes
//## end GeoTrd%3CD2A59100F8.preface

//## Class: GeoTrd%3CD2A59100F8
//	This shape represents a simple trapezoid
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoTrd : public GeoShape  //## Inherits: <unnamed>%3CD2A59100FB
{
  //## begin GeoTrd%3CD2A59100F8.initialDeclarations preserve=yes
  //## end GeoTrd%3CD2A59100F8.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoTrd%3CD5B5110183
      //	Constructor for the TRD.
      GeoTrd (double XHalfLength1, double XHalfLength2, double YHalfLength1, double YHalfLength2, double ZHalfLength);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A6F6026D
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A8610279
      //	Returns the TRD shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A861028D
      //	Returns the TRD shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A8300BB
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2A59100F9
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2A59100FA
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: xHalfLength1%3CD5B49B031E
      //	Half length in the x-direction at -dz.
      const double& getXHalfLength1 () const;

      //## Attribute: xHalfLength2%3CD5B4A500FB
      //	Half length in the x-direction at +dz
      const double& getXHalfLength2 () const;

      //## Attribute: yHalfLength1%3CD5B49B0332
      //	Half-length in the y direction at +dz.
      const double& getYHalfLength1 () const;

      //## Attribute: yHalfLength2%3CD5B4A50105
      //	Half-length in the y direction at -dz
      const double& getYHalfLength2 () const;

      //## Attribute: zHalfLength%3CD5B49B033C
      //	Half-length in the z direction.
      const double& getZHalfLength () const;

    // Additional Public Declarations
      //## begin GeoTrd%3CD2A59100F8.public preserve=yes
      //## end GeoTrd%3CD2A59100F8.public

  protected:
    //## Destructor (generated)
      virtual ~GeoTrd();

    // Additional Protected Declarations
      //## begin GeoTrd%3CD2A59100F8.protected preserve=yes
      //## end GeoTrd%3CD2A59100F8.protected

  private:
    //## Constructors (generated)
      GeoTrd(const GeoTrd &right);

    //## Assignment Operation (generated)
      GeoTrd & operator=(const GeoTrd &right);

    // Data Members for Class Attributes

      //## begin GeoTrd::s_classType%3CD2A59100F9.attr preserve=no  public: static const std::string {U} "Trd"
      static const std::string s_classType;
      //## end GeoTrd::s_classType%3CD2A59100F9.attr

      //## begin GeoTrd::s_classTypeID%3CD2A59100FA.attr preserve=no  public: static const ShapeType {U} 0x16
      static const ShapeType s_classTypeID;
      //## end GeoTrd::s_classTypeID%3CD2A59100FA.attr

      //## begin GeoTrd::m_xHalfLength1%3CD5B49B031E.attr preserve=no  public: double {U} 
      double m_xHalfLength1;
      //## end GeoTrd::m_xHalfLength1%3CD5B49B031E.attr

      //## begin GeoTrd::m_xHalfLength2%3CD5B4A500FB.attr preserve=no  public: double {U} 
      double m_xHalfLength2;
      //## end GeoTrd::m_xHalfLength2%3CD5B4A500FB.attr

      //## begin GeoTrd::m_yHalfLength1%3CD5B49B0332.attr preserve=no  public: double {U} 
      double m_yHalfLength1;
      //## end GeoTrd::m_yHalfLength1%3CD5B49B0332.attr

      //## begin GeoTrd::m_yHalfLength2%3CD5B4A50105.attr preserve=no  public: double {U} 
      double m_yHalfLength2;
      //## end GeoTrd::m_yHalfLength2%3CD5B4A50105.attr

      //## begin GeoTrd::m_zHalfLength%3CD5B49B033C.attr preserve=no  public: double {U} 
      double m_zHalfLength;
      //## end GeoTrd::m_zHalfLength%3CD5B49B033C.attr

    // Additional Private Declarations
      //## begin GeoTrd%3CD2A59100F8.private preserve=yes
      //## end GeoTrd%3CD2A59100F8.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoTrd%3CD2A59100F8.implementation preserve=yes
      //## end GeoTrd%3CD2A59100F8.implementation

};

//## begin GeoTrd%3CD2A59100F8.postscript preserve=yes
//## end GeoTrd%3CD2A59100F8.postscript

// Class GeoTrd 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoTrd::getClassType ()
{
  //## begin GeoTrd::getClassType%3CD2A59100F9.get preserve=no
  return s_classType;
  //## end GeoTrd::getClassType%3CD2A59100F9.get
}

inline ShapeType GeoTrd::getClassTypeID ()
{
  //## begin GeoTrd::getClassTypeID%3CD2A59100FA.get preserve=no
  return s_classTypeID;
  //## end GeoTrd::getClassTypeID%3CD2A59100FA.get
}

inline const double& GeoTrd::getXHalfLength1 () const
{
  //## begin GeoTrd::getXHalfLength1%3CD5B49B031E.get preserve=no
  return m_xHalfLength1;
  //## end GeoTrd::getXHalfLength1%3CD5B49B031E.get
}

inline const double& GeoTrd::getXHalfLength2 () const
{
  //## begin GeoTrd::getXHalfLength2%3CD5B4A500FB.get preserve=no
  return m_xHalfLength2;
  //## end GeoTrd::getXHalfLength2%3CD5B4A500FB.get
}

inline const double& GeoTrd::getYHalfLength1 () const
{
  //## begin GeoTrd::getYHalfLength1%3CD5B49B0332.get preserve=no
  return m_yHalfLength1;
  //## end GeoTrd::getYHalfLength1%3CD5B49B0332.get
}

inline const double& GeoTrd::getYHalfLength2 () const
{
  //## begin GeoTrd::getYHalfLength2%3CD5B4A50105.get preserve=no
  return m_yHalfLength2;
  //## end GeoTrd::getYHalfLength2%3CD5B4A50105.get
}

inline const double& GeoTrd::getZHalfLength () const
{
  //## begin GeoTrd::getZHalfLength%3CD5B49B033C.get preserve=no
  return m_zHalfLength;
  //## end GeoTrd::getZHalfLength%3CD5B49B033C.get
}

//## begin module%3CD2A59100F8.epilog preserve=yes
//## end module%3CD2A59100F8.epilog


#endif
