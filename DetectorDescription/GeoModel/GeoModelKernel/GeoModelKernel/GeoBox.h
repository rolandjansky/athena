/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD2696E017F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD2696E017F.cm

//## begin module%3CD2696E017F.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD2696E017F.cp

//## Module: GeoBox%3CD2696E017F; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoBox.h

#ifndef GeoBox_h
#define GeoBox_h 1

//## begin module%3CD2696E017F.additionalIncludes preserve=no
//## end module%3CD2696E017F.additionalIncludes

//## begin module%3CD2696E017F.includes preserve=yes
//## end module%3CD2696E017F.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD2696E017F.additionalDeclarations preserve=yes
//## end module%3CD2696E017F.additionalDeclarations


//## begin GeoBox%3CD2696E017F.preface preserve=yes
//## end GeoBox%3CD2696E017F.preface

//## Class: GeoBox%3CD2696E017F
//	This shape represents a box.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoBox : public GeoShape  //## Inherits: <unnamed>%3CD2696E0180
{
  //## begin GeoBox%3CD2696E017F.initialDeclarations preserve=yes
  //## end GeoBox%3CD2696E017F.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoBox%3CD58E8B01CE
      //	Constructor for the BOX.
      GeoBox (double XHalfLength, double YHalfLength, double ZHalfLength);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A69A0008
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A7FB011E
      //	Returns the BOX shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A7FB0132
      //	Returns the BOX shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A050150
      //	 Executes a GeoShapeAction.
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD26A64002E
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD26A640038
      //	For type identification.,
      static ShapeType getClassTypeID ();

      //## Attribute: xHalfLength%3CD2A8C400CD
      //	Half length in the x-direction.
      const double& getXHalfLength () const;

      //## Attribute: yHalfLength%3CD2A8DE02BF
      //	Half-length in the y direction.
      const double& getYHalfLength () const;

      //## Attribute: zHalfLength%3CD2A8E00146
      //	Half-length in the z direction.
      const double& getZHalfLength () const;

    // Additional Public Declarations
      //## begin GeoBox%3CD2696E017F.public preserve=yes
      //## end GeoBox%3CD2696E017F.public

  protected:
    //## Destructor (generated)
      virtual ~GeoBox();

    // Additional Protected Declarations
      //## begin GeoBox%3CD2696E017F.protected preserve=yes
      //## end GeoBox%3CD2696E017F.protected

  private:
    //## Constructors (generated)
      GeoBox(const GeoBox &right);

    //## Assignment Operation (generated)
      GeoBox & operator=(const GeoBox &right);

    // Data Members for Class Attributes

      //## begin GeoBox::s_classType%3CD26A64002E.attr preserve=no  public: static const std::string {U} "Box"
      static const std::string s_classType;
      //## end GeoBox::s_classType%3CD26A64002E.attr

      //## begin GeoBox::s_classTypeID%3CD26A640038.attr preserve=no  public: static const ShapeType {U} 0x10
      static const ShapeType s_classTypeID;
      //## end GeoBox::s_classTypeID%3CD26A640038.attr

      //## begin GeoBox::m_xHalfLength%3CD2A8C400CD.attr preserve=no  public: double {U} 
      double m_xHalfLength;
      //## end GeoBox::m_xHalfLength%3CD2A8C400CD.attr

      //## begin GeoBox::m_yHalfLength%3CD2A8DE02BF.attr preserve=no  public: double {U} 
      double m_yHalfLength;
      //## end GeoBox::m_yHalfLength%3CD2A8DE02BF.attr

      //## begin GeoBox::m_zHalfLength%3CD2A8E00146.attr preserve=no  public: double {U} 
      double m_zHalfLength;
      //## end GeoBox::m_zHalfLength%3CD2A8E00146.attr

    // Additional Private Declarations
      //## begin GeoBox%3CD2696E017F.private preserve=yes
      //## end GeoBox%3CD2696E017F.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoBox%3CD2696E017F.implementation preserve=yes
      //## end GeoBox%3CD2696E017F.implementation

};

//## begin GeoBox%3CD2696E017F.postscript preserve=yes
//## end GeoBox%3CD2696E017F.postscript

// Class GeoBox 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoBox::getClassType ()
{
  //## begin GeoBox::getClassType%3CD26A64002E.get preserve=no
  return s_classType;
  //## end GeoBox::getClassType%3CD26A64002E.get
}

inline ShapeType GeoBox::getClassTypeID ()
{
  //## begin GeoBox::getClassTypeID%3CD26A640038.get preserve=no
  return s_classTypeID;
  //## end GeoBox::getClassTypeID%3CD26A640038.get
}

inline const double& GeoBox::getXHalfLength () const
{
  //## begin GeoBox::getXHalfLength%3CD2A8C400CD.get preserve=no
  return m_xHalfLength;
  //## end GeoBox::getXHalfLength%3CD2A8C400CD.get
}

inline const double& GeoBox::getYHalfLength () const
{
  //## begin GeoBox::getYHalfLength%3CD2A8DE02BF.get preserve=no
  return m_yHalfLength;
  //## end GeoBox::getYHalfLength%3CD2A8DE02BF.get
}

inline const double& GeoBox::getZHalfLength () const
{
  //## begin GeoBox::getZHalfLength%3CD2A8E00146.get preserve=no
  return m_zHalfLength;
  //## end GeoBox::getZHalfLength%3CD2A8E00146.get
}

//## begin module%3CD2696E017F.epilog preserve=yes
//## end module%3CD2696E017F.epilog


#endif
