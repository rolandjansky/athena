/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2A5E301F0.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2A5E301F0.cm

//## begin module%3CD2A5E301F0.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2A5E301F0.cp

//## Module: GeoTube%3CD2A5E301F0; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTube.h

#ifndef GeoTube_h
#define GeoTube_h 1

//## begin module%3CD2A5E301F0.additionalIncludes preserve=no
//## end module%3CD2A5E301F0.additionalIncludes

//## begin module%3CD2A5E301F0.includes preserve=yes
//## end module%3CD2A5E301F0.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2A5E301F0.additionalDeclarations preserve=yes
//## end module%3CD2A5E301F0.additionalDeclarations


//## begin GeoTube%3CD2A5E301F0.preface preserve=yes
//## end GeoTube%3CD2A5E301F0.preface

//## Class: GeoTube%3CD2A5E301F0
//	This shape represents a tube
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoTube : public GeoShape  //## Inherits: <unnamed>%3CD2A5E301FC
{
  //## begin GeoTube%3CD2A5E301F0.initialDeclarations preserve=yes
  //## end GeoTube%3CD2A5E301F0.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoTube%3CD5AF79016A
      //	Constructor for the TUBE.
      GeoTube (double RMin, double RMax, double ZHalfLength);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A7130369
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A868007B
      //	Returns the TUBE shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A8680099
      //	Returns the TUBE shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A8A03D2
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD2A5E301FA
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD2A5E301FB
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: rMin%3CD5AEED028B
      //	Minimum (inner) tube radius.
      const double& getRMin () const;

      //## Attribute: rMax%3CD5AEED0295
      //	Maximum (outer) tube radius.
      const double& getRMax () const;

      //## Attribute: zHalfLength%3CD5AEED0296
      //	Tube half-length in the z direction.
      const double& getZHalfLength () const;

    // Additional Public Declarations
      //## begin GeoTube%3CD2A5E301F0.public preserve=yes
      //## end GeoTube%3CD2A5E301F0.public

  protected:
    //## Destructor (generated)
      virtual ~GeoTube();

    // Additional Protected Declarations
      //## begin GeoTube%3CD2A5E301F0.protected preserve=yes
      //## end GeoTube%3CD2A5E301F0.protected

  private:
    //## Constructors (generated)
      GeoTube(const GeoTube &right);

    //## Assignment Operation (generated)
      GeoTube & operator=(const GeoTube &right);

    // Data Members for Class Attributes

      //## begin GeoTube::s_classType%3CD2A5E301FA.attr preserve=no  public: static const std::string {U} "Tube"
      static const std::string s_classType;
      //## end GeoTube::s_classType%3CD2A5E301FA.attr

      //## begin GeoTube::s_classTypeID%3CD2A5E301FB.attr preserve=no  public: static const ShapeType {U} 0x17
      static const ShapeType s_classTypeID;
      //## end GeoTube::s_classTypeID%3CD2A5E301FB.attr

      //## begin GeoTube::m_rMin%3CD5AEED028B.attr preserve=no  public: double {U} 
      double m_rMin;
      //## end GeoTube::m_rMin%3CD5AEED028B.attr

      //## begin GeoTube::m_rMax%3CD5AEED0295.attr preserve=no  public: double {U} 
      double m_rMax;
      //## end GeoTube::m_rMax%3CD5AEED0295.attr

      //## begin GeoTube::m_zHalfLength%3CD5AEED0296.attr preserve=no  public: double {U} 
      double m_zHalfLength;
      //## end GeoTube::m_zHalfLength%3CD5AEED0296.attr

    // Additional Private Declarations
      //## begin GeoTube%3CD2A5E301F0.private preserve=yes
      //## end GeoTube%3CD2A5E301F0.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoTube%3CD2A5E301F0.implementation preserve=yes
      //## end GeoTube%3CD2A5E301F0.implementation

};

//## begin GeoTube%3CD2A5E301F0.postscript preserve=yes
//## end GeoTube%3CD2A5E301F0.postscript

// Class GeoTube 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoTube::getClassType ()
{
  //## begin GeoTube::getClassType%3CD2A5E301FA.get preserve=no
  return s_classType;
  //## end GeoTube::getClassType%3CD2A5E301FA.get
}

inline ShapeType GeoTube::getClassTypeID ()
{
  //## begin GeoTube::getClassTypeID%3CD2A5E301FB.get preserve=no
  return s_classTypeID;
  //## end GeoTube::getClassTypeID%3CD2A5E301FB.get
}

inline const double& GeoTube::getRMin () const
{
  //## begin GeoTube::getRMin%3CD5AEED028B.get preserve=no
  return m_rMin;
  //## end GeoTube::getRMin%3CD5AEED028B.get
}

inline const double& GeoTube::getRMax () const
{
  //## begin GeoTube::getRMax%3CD5AEED0295.get preserve=no
  return m_rMax;
  //## end GeoTube::getRMax%3CD5AEED0295.get
}

inline const double& GeoTube::getZHalfLength () const
{
  //## begin GeoTube::getZHalfLength%3CD5AEED0296.get preserve=no
  return m_zHalfLength;
  //## end GeoTube::getZHalfLength%3CD5AEED0296.get
}

//## begin module%3CD2A5E301F0.epilog preserve=yes
//## end module%3CD2A5E301F0.epilog


#endif
