/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD26D7503D0.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD26D7503D0.cm

//## begin module%3CD26D7503D0.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD26D7503D0.cp

//## Module: GeoPara%3CD26D7503D0; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPara.h

#ifndef GeoPara_h
#define GeoPara_h 1

//## begin module%3CD26D7503D0.additionalIncludes preserve=no
//## end module%3CD26D7503D0.additionalIncludes

//## begin module%3CD26D7503D0.includes preserve=yes
//## end module%3CD26D7503D0.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD26D7503D0.additionalDeclarations preserve=yes
//## end module%3CD26D7503D0.additionalDeclarations


//## begin GeoPara%3CD26D7503D0.preface preserve=yes
//## end GeoPara%3CD26D7503D0.preface

//## Class: GeoPara%3CD26D7503D0
//	This shape represents a parallelapiped.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPara : public GeoShape  //## Inherits: <unnamed>%3CD26D7503D3
{
  //## begin GeoPara%3CD26D7503D0.initialDeclarations preserve=yes
  //## end GeoPara%3CD26D7503D0.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoPara%3CD5A8D5005A
      //	Constructor for the BOX.
      GeoPara (double XHalfLength, double YHalfLength, double ZHalfLength, double Alpha, double Theta, double Phi);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A6DB00FC
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A83C0208
      //	Returns the PARA shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A83C0226
      //	Returns the PARA shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A3602F5
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD26D7503D1
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD26D7503D2
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: xHalfLength%3CD5A83B0262
      //	Half length in the x-direction.
      const double& getXHalfLength () const;

      //## Attribute: yHalfLength%3CD5A83B026C
      //	Half-length in the y direction.
      const double& getYHalfLength () const;

      //## Attribute: zHalfLength%3CD5A83B0276
      //	Half-length in the z direction.
      const double& getZHalfLength () const;

      //## Attribute: theta%3CD5A87A0095
      //	Polar (theta) angle.
      const double& getTheta () const;

      //## Attribute: alpha%3CDBEBAF01D4
      //	The angle alpha...between the two sides of the top face
      //	of the parallelapiped.
      const double& getAlpha () const;

      //## Attribute: phi%3CD5A87B0231
      //	Azimuthal (phi) angle.
      const double& getPhi () const;

    // Additional Public Declarations
      //## begin GeoPara%3CD26D7503D0.public preserve=yes
      //## end GeoPara%3CD26D7503D0.public

  protected:
    //## Destructor (generated)
      virtual ~GeoPara();

    // Additional Protected Declarations
      //## begin GeoPara%3CD26D7503D0.protected preserve=yes
      //## end GeoPara%3CD26D7503D0.protected

  private:
    //## Constructors (generated)
      GeoPara(const GeoPara &right);

    //## Assignment Operation (generated)
      GeoPara & operator=(const GeoPara &right);

    // Data Members for Class Attributes

      //## begin GeoPara::s_classType%3CD26D7503D1.attr preserve=no  public: static const std::string {U} "Para"
      static const std::string s_classType;
      //## end GeoPara::s_classType%3CD26D7503D1.attr

      //## begin GeoPara::s_classTypeID%3CD26D7503D2.attr preserve=no  public: static const ShapeType {U} 0x12
      static const ShapeType s_classTypeID;
      //## end GeoPara::s_classTypeID%3CD26D7503D2.attr

      //## begin GeoPara::m_xHalfLength%3CD5A83B0262.attr preserve=no  public: double {U} 
      double m_xHalfLength;
      //## end GeoPara::m_xHalfLength%3CD5A83B0262.attr

      //## begin GeoPara::m_yHalfLength%3CD5A83B026C.attr preserve=no  public: double {U} 
      double m_yHalfLength;
      //## end GeoPara::m_yHalfLength%3CD5A83B026C.attr

      //## begin GeoPara::m_zHalfLength%3CD5A83B0276.attr preserve=no  public: double {U} 
      double m_zHalfLength;
      //## end GeoPara::m_zHalfLength%3CD5A83B0276.attr

      //## begin GeoPara::m_theta%3CD5A87A0095.attr preserve=no  public: double {U} 
      double m_theta;
      //## end GeoPara::m_theta%3CD5A87A0095.attr

      //## begin GeoPara::m_alpha%3CDBEBAF01D4.attr preserve=no  public: double {U} 
      double m_alpha;
      //## end GeoPara::m_alpha%3CDBEBAF01D4.attr

      //## begin GeoPara::m_phi%3CD5A87B0231.attr preserve=no  public: double {U} 
      double m_phi;
      //## end GeoPara::m_phi%3CD5A87B0231.attr

    // Additional Private Declarations
      //## begin GeoPara%3CD26D7503D0.private preserve=yes
      //## end GeoPara%3CD26D7503D0.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPara%3CD26D7503D0.implementation preserve=yes
      //## end GeoPara%3CD26D7503D0.implementation

};

//## begin GeoPara%3CD26D7503D0.postscript preserve=yes
//## end GeoPara%3CD26D7503D0.postscript

// Class GeoPara 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoPara::getClassType ()
{
  //## begin GeoPara::getClassType%3CD26D7503D1.get preserve=no
  return s_classType;
  //## end GeoPara::getClassType%3CD26D7503D1.get
}

inline ShapeType GeoPara::getClassTypeID ()
{
  //## begin GeoPara::getClassTypeID%3CD26D7503D2.get preserve=no
  return s_classTypeID;
  //## end GeoPara::getClassTypeID%3CD26D7503D2.get
}

inline const double& GeoPara::getXHalfLength () const
{
  //## begin GeoPara::getXHalfLength%3CD5A83B0262.get preserve=no
  return m_xHalfLength;
  //## end GeoPara::getXHalfLength%3CD5A83B0262.get
}

inline const double& GeoPara::getYHalfLength () const
{
  //## begin GeoPara::getYHalfLength%3CD5A83B026C.get preserve=no
  return m_yHalfLength;
  //## end GeoPara::getYHalfLength%3CD5A83B026C.get
}

inline const double& GeoPara::getZHalfLength () const
{
  //## begin GeoPara::getZHalfLength%3CD5A83B0276.get preserve=no
  return m_zHalfLength;
  //## end GeoPara::getZHalfLength%3CD5A83B0276.get
}

inline const double& GeoPara::getTheta () const
{
  //## begin GeoPara::getTheta%3CD5A87A0095.get preserve=no
  return m_theta;
  //## end GeoPara::getTheta%3CD5A87A0095.get
}

inline const double& GeoPara::getAlpha () const
{
  //## begin GeoPara::getAlpha%3CDBEBAF01D4.get preserve=no
  return m_alpha;
  //## end GeoPara::getAlpha%3CDBEBAF01D4.get
}

inline const double& GeoPara::getPhi () const
{
  //## begin GeoPara::getPhi%3CD5A87B0231.get preserve=no
  return m_phi;
  //## end GeoPara::getPhi%3CD5A87B0231.get
}

//## begin module%3CD26D7503D0.epilog preserve=yes
//## end module%3CD26D7503D0.epilog


#endif
