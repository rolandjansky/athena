/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A51701E2.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A51701E2.cm

//## begin module%3CD2A51701E2.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2A51701E2.cp

//## Module: GeoTrap%3CD2A51701E2; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTrap.h

#ifndef GeoTrap_h
#define GeoTrap_h 1

//## begin module%3CD2A51701E2.additionalIncludes preserve=no
//## end module%3CD2A51701E2.additionalIncludes

//## begin module%3CD2A51701E2.includes preserve=yes
//## end module%3CD2A51701E2.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2A51701E2.additionalDeclarations preserve=yes
//## end module%3CD2A51701E2.additionalDeclarations


//## begin GeoTrap%3CD2A51701E2.preface preserve=yes
//## end GeoTrap%3CD2A51701E2.preface

//## Class: GeoTrap%3CD2A51701E2
//	This shape represents a very general trapezoid
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoTrap : public GeoShape  //## Inherits: <unnamed>%3CD2A51701E5
{
  //## begin GeoTrap%3CD2A51701E2.initialDeclarations preserve=yes
  //## end GeoTrap%3CD2A51701E2.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoTrap%3CD5BEDD0276
      //	Constructor for the TRAP.
      GeoTrap (double ZHalfLength, double Theta, double Phi, double Dydzn, double Dxdyndzn, double Dxdypdzn, double Angleydzn, double Dydzp, double Dxdyndzp, double Dxdypdzp, double Angleydzp);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A6EF014B
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A8570149
      //	Returns the TRAP shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A8570167
      //	Returns the TRAP shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A76015C
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2A51701E3
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2A51701E4
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: zHalfLength%3CD5BD6000A1
      //	Z half length.
      const double& getZHalfLength () const;

      //## Attribute: theta%3CD5BD6000AB
      //	Polar (theta) angle.
      const double& getTheta () const;

      //## Attribute: phi%3CD5BD6000BF
      //	Azimuthal (phi) angle.
      const double& getPhi () const;

      //## Attribute: dydzn%3CD5BD8B01DA
      //	Y half length at -z.
      const double& getDydzn () const;

      //## Attribute: dxdyndzn%3CD5BD9800E8
      //	X half length at -z, -y.
      const double& getDxdyndzn () const;

      //## Attribute: dxdypdzn%3CD5BD9B0164
      //	X half length at -z, +y
      const double& getDxdypdzn () const;

      //## Attribute: angleydzn%3CD5BDA002DE
      const double& getAngleydzn () const;

      //## Attribute: dydzp%3CD5BDA30007
      //	Y half length at +z.
      const double& getDydzp () const;

      //## Attribute: dxdyndzp%3CD5BDA5010F
      //	X half length at +z, -y
      const double& getDxdyndzp () const;

      //## Attribute: dxdypdzp%3CD5BDA80235
      //	X half length at +z, +y
      const double& getDxdypdzp () const;

      //## Attribute: angleydzp%3CD5BDAA03BF
      const double& getAngleydzp () const;

    // Additional Public Declarations
      //## begin GeoTrap%3CD2A51701E2.public preserve=yes
      //## end GeoTrap%3CD2A51701E2.public

  protected:
    //## Destructor (generated)
      virtual ~GeoTrap();

    // Additional Protected Declarations
      //## begin GeoTrap%3CD2A51701E2.protected preserve=yes
      //## end GeoTrap%3CD2A51701E2.protected

  private:
    //## Constructors (generated)
      GeoTrap(const GeoTrap &right);

    //## Assignment Operation (generated)
      GeoTrap & operator=(const GeoTrap &right);

    // Data Members for Class Attributes

      //## begin GeoTrap::s_classType%3CD2A51701E3.attr preserve=no  public: static const std::string {U} "Trap"
      static const std::string s_classType;
      //## end GeoTrap::s_classType%3CD2A51701E3.attr

      //## begin GeoTrap::s_classTypeID%3CD2A51701E4.attr preserve=no  public: static const ShapeType {U} 0x15
      static const ShapeType s_classTypeID;
      //## end GeoTrap::s_classTypeID%3CD2A51701E4.attr

      //## begin GeoTrap::m_zHalfLength%3CD5BD6000A1.attr preserve=no  public: double {U} 
      double m_zHalfLength;
      //## end GeoTrap::m_zHalfLength%3CD5BD6000A1.attr

      //## begin GeoTrap::m_theta%3CD5BD6000AB.attr preserve=no  public: double {U} 
      double m_theta;
      //## end GeoTrap::m_theta%3CD5BD6000AB.attr

      //## begin GeoTrap::m_phi%3CD5BD6000BF.attr preserve=no  public: double {U} 
      double m_phi;
      //## end GeoTrap::m_phi%3CD5BD6000BF.attr

      //## begin GeoTrap::m_dydzn%3CD5BD8B01DA.attr preserve=no  public: double {U} 
      double m_dydzn;
      //## end GeoTrap::m_dydzn%3CD5BD8B01DA.attr

      //## begin GeoTrap::m_dxdyndzn%3CD5BD9800E8.attr preserve=no  public: double {U} 
      double m_dxdyndzn;
      //## end GeoTrap::m_dxdyndzn%3CD5BD9800E8.attr

      //## begin GeoTrap::m_dxdypdzn%3CD5BD9B0164.attr preserve=no  public: double {U} 
      double m_dxdypdzn;
      //## end GeoTrap::m_dxdypdzn%3CD5BD9B0164.attr

      //## begin GeoTrap::m_angleydzn%3CD5BDA002DE.attr preserve=no  public: double {U} 
      double m_angleydzn;
      //## end GeoTrap::m_angleydzn%3CD5BDA002DE.attr

      //## begin GeoTrap::m_dydzp%3CD5BDA30007.attr preserve=no  public: double {U} 
      double m_dydzp;
      //## end GeoTrap::m_dydzp%3CD5BDA30007.attr

      //## begin GeoTrap::m_dxdyndzp%3CD5BDA5010F.attr preserve=no  public: double {U} 
      double m_dxdyndzp;
      //## end GeoTrap::m_dxdyndzp%3CD5BDA5010F.attr

      //## begin GeoTrap::m_dxdypdzp%3CD5BDA80235.attr preserve=no  public: double {U} 
      double m_dxdypdzp;
      //## end GeoTrap::m_dxdypdzp%3CD5BDA80235.attr

      //## begin GeoTrap::m_angleydzp%3CD5BDAA03BF.attr preserve=no  public: double {U} 
      double m_angleydzp;
      //## end GeoTrap::m_angleydzp%3CD5BDAA03BF.attr

    // Additional Private Declarations
      //## begin GeoTrap%3CD2A51701E2.private preserve=yes
      //## end GeoTrap%3CD2A51701E2.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoTrap%3CD2A51701E2.implementation preserve=yes
      //## end GeoTrap%3CD2A51701E2.implementation

};

//## begin GeoTrap%3CD2A51701E2.postscript preserve=yes
//## end GeoTrap%3CD2A51701E2.postscript

// Class GeoTrap 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoTrap::getClassType ()
{
  //## begin GeoTrap::getClassType%3CD2A51701E3.get preserve=no
  return s_classType;
  //## end GeoTrap::getClassType%3CD2A51701E3.get
}

inline ShapeType GeoTrap::getClassTypeID ()
{
  //## begin GeoTrap::getClassTypeID%3CD2A51701E4.get preserve=no
  return s_classTypeID;
  //## end GeoTrap::getClassTypeID%3CD2A51701E4.get
}

inline const double& GeoTrap::getZHalfLength () const
{
  //## begin GeoTrap::getZHalfLength%3CD5BD6000A1.get preserve=no
  return m_zHalfLength;
  //## end GeoTrap::getZHalfLength%3CD5BD6000A1.get
}

inline const double& GeoTrap::getTheta () const
{
  //## begin GeoTrap::getTheta%3CD5BD6000AB.get preserve=no
  return m_theta;
  //## end GeoTrap::getTheta%3CD5BD6000AB.get
}

inline const double& GeoTrap::getPhi () const
{
  //## begin GeoTrap::getPhi%3CD5BD6000BF.get preserve=no
  return m_phi;
  //## end GeoTrap::getPhi%3CD5BD6000BF.get
}

inline const double& GeoTrap::getDydzn () const
{
  //## begin GeoTrap::getDydzn%3CD5BD8B01DA.get preserve=no
  return m_dydzn;
  //## end GeoTrap::getDydzn%3CD5BD8B01DA.get
}

inline const double& GeoTrap::getDxdyndzn () const
{
  //## begin GeoTrap::getDxdyndzn%3CD5BD9800E8.get preserve=no
  return m_dxdyndzn;
  //## end GeoTrap::getDxdyndzn%3CD5BD9800E8.get
}

inline const double& GeoTrap::getDxdypdzn () const
{
  //## begin GeoTrap::getDxdypdzn%3CD5BD9B0164.get preserve=no
  return m_dxdypdzn;
  //## end GeoTrap::getDxdypdzn%3CD5BD9B0164.get
}

inline const double& GeoTrap::getAngleydzn () const
{
  //## begin GeoTrap::getAngleydzn%3CD5BDA002DE.get preserve=no
  return m_angleydzn;
  //## end GeoTrap::getAngleydzn%3CD5BDA002DE.get
}

inline const double& GeoTrap::getDydzp () const
{
  //## begin GeoTrap::getDydzp%3CD5BDA30007.get preserve=no
  return m_dydzp;
  //## end GeoTrap::getDydzp%3CD5BDA30007.get
}

inline const double& GeoTrap::getDxdyndzp () const
{
  //## begin GeoTrap::getDxdyndzp%3CD5BDA5010F.get preserve=no
  return m_dxdyndzp;
  //## end GeoTrap::getDxdyndzp%3CD5BDA5010F.get
}

inline const double& GeoTrap::getDxdypdzp () const
{
  //## begin GeoTrap::getDxdypdzp%3CD5BDA80235.get preserve=no
  return m_dxdypdzp;
  //## end GeoTrap::getDxdypdzp%3CD5BDA80235.get
}

inline const double& GeoTrap::getAngleydzp () const
{
  //## begin GeoTrap::getAngleydzp%3CD5BDAA03BF.get preserve=no
  return m_angleydzp;
  //## end GeoTrap::getAngleydzp%3CD5BDAA03BF.get
}

//## begin module%3CD2A51701E2.epilog preserve=yes
//## end module%3CD2A51701E2.epilog


#endif
