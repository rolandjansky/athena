/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD25BA0113.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD25BA0113.cm

//## begin module%3CDD25BA0113.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDD25BA0113.cp

//## Module: GeoLogVol%3CDD25BA0113; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoLogVol.h

#ifndef GeoLogVol_h
#define GeoLogVol_h 1

//## begin module%3CDD25BA0113.additionalIncludes preserve=no
//## end module%3CDD25BA0113.additionalIncludes

//## begin module%3CDD25BA0113.includes preserve=yes
//## end module%3CDD25BA0113.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
// RCBase
#include "GeoModelKernel/RCBase.h"
// GeoMaterial
#include "GeoModelKernel/GeoMaterial.h"
//## begin module%3CDD25BA0113.additionalDeclarations preserve=yes
#include <vector>
//## end module%3CDD25BA0113.additionalDeclarations


//## begin GeoLogVol%3CDD25BA0113.preface preserve=yes
//## end GeoLogVol%3CDD25BA0113.preface

//## Class: GeoLogVol%3CDD25BA0113
//	A Logical Volume, or agglomeration of shape and material.
//	The logical volume increases the reference count of the
//	shape and the material.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoLogVol : public RCBase  //## Inherits: <unnamed>%3CDD263D036A
{
  //## begin GeoLogVol%3CDD25BA0113.initialDeclarations preserve=yes
  //## end GeoLogVol%3CDD25BA0113.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoLogVol%3CDD2AB7008D
      //	Constructor for the logical volume.
      GeoLogVol (const std::string &Name, const GeoShape *Shape, const GeoMaterial *Material);


    //## Other Operations (specified)
      //## Operation: getName%3CE7DD8A026D
      //	Returns the name of the logical volume.
      const std::string & getName () const;

      //## Operation: getShape%3CE7DE4F0342
      //	Returns the shape of the logical volume.
      const GeoShape * getShape () const;

      //## Operation: getMaterial%3CE7DE5101D3
      //	Returns the material of the logical volume.
      const GeoMaterial * getMaterial () const;

    // Additional Public Declarations
      //## begin GeoLogVol%3CDD25BA0113.public preserve=yes
      //## end GeoLogVol%3CDD25BA0113.public

  protected:
    //## Destructor (generated)
      virtual ~GeoLogVol();

    // Additional Protected Declarations
      //## begin GeoLogVol%3CDD25BA0113.protected preserve=yes
      //## end GeoLogVol%3CDD25BA0113.protected

  private:
    //## Constructors (generated)
      GeoLogVol(const GeoLogVol &right);

    //## Assignment Operation (generated)
      GeoLogVol & operator=(const GeoLogVol &right);

    // Data Members for Class Attributes

      //## Attribute: m_name%3CDD2B9F0358
      //	The name of the logical volume.
      //## begin GeoLogVol::m_name%3CDD2B9F0358.attr preserve=no  private: std::string {U} 
      std::string m_name;
      //## end GeoLogVol::m_name%3CDD2B9F0358.attr

    // Additional Private Declarations
      //## begin GeoLogVol%3CDD25BA0113.private preserve=yes
      //## end GeoLogVol%3CDD25BA0113.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::Material specification%3CDD26580337
      //## Role: GeoLogVol::m_material%3CDD265B0039
      //	Material composition of this volume.
      //## begin GeoLogVol::m_material%3CDD265B0039.role preserve=no  public: GeoMaterial {1 -> 1RHgN}
      const GeoMaterial *m_material;
      //## end GeoLogVol::m_material%3CDD265B0039.role

      //## Association: GeoModelKernel::Shape Specification%3CDD277402EF
      //## Role: GeoLogVol::m_shape%3CDD277502FC
      //	Shape of this volume.
      //## begin GeoLogVol::m_shape%3CDD277502FC.role preserve=no  public: GeoShape {1 -> 1RHgN}
      const GeoShape *m_shape;
      //## end GeoLogVol::m_shape%3CDD277502FC.role

    // Additional Implementation Declarations
      //## begin GeoLogVol%3CDD25BA0113.implementation preserve=yes
      //## end GeoLogVol%3CDD25BA0113.implementation

};

//## begin GeoLogVol%3CDD25BA0113.postscript preserve=yes
//## end GeoLogVol%3CDD25BA0113.postscript

// Class GeoLogVol 


//## Other Operations (inline)
inline const std::string & GeoLogVol::getName () const
{
  //## begin GeoLogVol::getName%3CE7DD8A026D.body preserve=yes
  return m_name;
  //## end GeoLogVol::getName%3CE7DD8A026D.body
}

inline const GeoShape * GeoLogVol::getShape () const
{
  //## begin GeoLogVol::getShape%3CE7DE4F0342.body preserve=yes
  return m_shape;
  //## end GeoLogVol::getShape%3CE7DE4F0342.body
}

inline const GeoMaterial * GeoLogVol::getMaterial () const
{
  //## begin GeoLogVol::getMaterial%3CE7DE5101D3.body preserve=yes
  return m_material;
  //## end GeoLogVol::getMaterial%3CE7DE5101D3.body
}

//## begin module%3CDD25BA0113.epilog preserve=yes
//## end module%3CDD25BA0113.epilog


#endif
