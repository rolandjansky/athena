/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A64B01BD.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A64B01BD.cm

//## begin module%3CD2A64B01BD.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2A64B01BD.cp

//## Module: GeoTubs%3CD2A64B01BD; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTubs.h

#ifndef GeoTubs_h
#define GeoTubs_h 1

//## begin module%3CD2A64B01BD.additionalIncludes preserve=no
//## end module%3CD2A64B01BD.additionalIncludes

//## begin module%3CD2A64B01BD.includes preserve=yes
//## end module%3CD2A64B01BD.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2A64B01BD.additionalDeclarations preserve=yes
//## end module%3CD2A64B01BD.additionalDeclarations


//## begin GeoTubs%3CD2A64B01BD.preface preserve=yes
//## end GeoTubs%3CD2A64B01BD.preface

//## Class: GeoTubs%3CD2A64B01BD
//	This shape represents a tube section
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoTubs : public GeoShape  //## Inherits: <unnamed>%3CD2A64B01C0
{
  //## begin GeoTubs%3CD2A64B01BD.initialDeclarations preserve=yes
  //## end GeoTubs%3CD2A64B01BD.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoTubs%3CD5B1E402BD
      //	Constructor for the TUBS.
      GeoTubs (double RMin, double RMax, double ZHalfLength, double SPhi, double DPhi);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A71A00FD
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A86F0099
      //	Returns the TUBS shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A86F00B7
      //	Returns the TUBS shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A9200C6
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2A64B01BE
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2A64B01BF
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: rMin%3CD5B13B004E
      //	Minimum (inner) tube section radius.
      const double& getRMin () const;

      //## Attribute: rMax%3CD5B13B0058
      //	Maximum (outer) tube section radius.
      const double& getRMax () const;

      //## Attribute: zHalfLength%3CD5B13B0062
      //	Tube section half-length in the z direction.
      const double& getZHalfLength () const;

      //## Attribute: sPhi%3CD5B1670318
      //	Starting angle of the tube section in radians.
      const double& getSPhi () const;

      //## Attribute: dPhi%3CD5B16901E4
      //	Delta angle of the tube section in radians.
      const double& getDPhi () const;

    // Additional Public Declarations
      //## begin GeoTubs%3CD2A64B01BD.public preserve=yes
      //## end GeoTubs%3CD2A64B01BD.public

  protected:
    //## Destructor (generated)
      virtual ~GeoTubs();

    // Additional Protected Declarations
      //## begin GeoTubs%3CD2A64B01BD.protected preserve=yes
      //## end GeoTubs%3CD2A64B01BD.protected

  private:
    //## Constructors (generated)
      GeoTubs(const GeoTubs &right);

    //## Assignment Operation (generated)
      GeoTubs & operator=(const GeoTubs &right);

    // Data Members for Class Attributes

      //## begin GeoTubs::s_classType%3CD2A64B01BE.attr preserve=no  public: static const std::string {U} "Tubs"
      static const std::string s_classType;
      //## end GeoTubs::s_classType%3CD2A64B01BE.attr

      //## begin GeoTubs::s_classTypeID%3CD2A64B01BF.attr preserve=no  public: static const ShapeType {U} 0x18
      static const ShapeType s_classTypeID;
      //## end GeoTubs::s_classTypeID%3CD2A64B01BF.attr

      //## begin GeoTubs::m_rMin%3CD5B13B004E.attr preserve=no  public: double {U} 
      double m_rMin;
      //## end GeoTubs::m_rMin%3CD5B13B004E.attr

      //## begin GeoTubs::m_rMax%3CD5B13B0058.attr preserve=no  public: double {U} 
      double m_rMax;
      //## end GeoTubs::m_rMax%3CD5B13B0058.attr

      //## begin GeoTubs::m_zHalfLength%3CD5B13B0062.attr preserve=no  public: double {U} 
      double m_zHalfLength;
      //## end GeoTubs::m_zHalfLength%3CD5B13B0062.attr

      //## begin GeoTubs::m_sPhi%3CD5B1670318.attr preserve=no  public: double {U} 
      double m_sPhi;
      //## end GeoTubs::m_sPhi%3CD5B1670318.attr

      //## begin GeoTubs::m_dPhi%3CD5B16901E4.attr preserve=no  public: double {U} 
      double m_dPhi;
      //## end GeoTubs::m_dPhi%3CD5B16901E4.attr

    // Additional Private Declarations
      //## begin GeoTubs%3CD2A64B01BD.private preserve=yes
      //## end GeoTubs%3CD2A64B01BD.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoTubs%3CD2A64B01BD.implementation preserve=yes
      //## end GeoTubs%3CD2A64B01BD.implementation

};

//## begin GeoTubs%3CD2A64B01BD.postscript preserve=yes
//## end GeoTubs%3CD2A64B01BD.postscript

// Class GeoTubs 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoTubs::getClassType ()
{
  //## begin GeoTubs::getClassType%3CD2A64B01BE.get preserve=no
  return s_classType;
  //## end GeoTubs::getClassType%3CD2A64B01BE.get
}

inline ShapeType GeoTubs::getClassTypeID ()
{
  //## begin GeoTubs::getClassTypeID%3CD2A64B01BF.get preserve=no
  return s_classTypeID;
  //## end GeoTubs::getClassTypeID%3CD2A64B01BF.get
}

inline const double& GeoTubs::getRMin () const
{
  //## begin GeoTubs::getRMin%3CD5B13B004E.get preserve=no
  return m_rMin;
  //## end GeoTubs::getRMin%3CD5B13B004E.get
}

inline const double& GeoTubs::getRMax () const
{
  //## begin GeoTubs::getRMax%3CD5B13B0058.get preserve=no
  return m_rMax;
  //## end GeoTubs::getRMax%3CD5B13B0058.get
}

inline const double& GeoTubs::getZHalfLength () const
{
  //## begin GeoTubs::getZHalfLength%3CD5B13B0062.get preserve=no
  return m_zHalfLength;
  //## end GeoTubs::getZHalfLength%3CD5B13B0062.get
}

inline const double& GeoTubs::getSPhi () const
{
  //## begin GeoTubs::getSPhi%3CD5B1670318.get preserve=no
  return m_sPhi;
  //## end GeoTubs::getSPhi%3CD5B1670318.get
}

inline const double& GeoTubs::getDPhi () const
{
  //## begin GeoTubs::getDPhi%3CD5B16901E4.get preserve=no
  return m_dPhi;
  //## end GeoTubs::getDPhi%3CD5B16901E4.get
}

//## begin module%3CD2A64B01BD.epilog preserve=yes
//## end module%3CD2A64B01BD.epilog


#endif
