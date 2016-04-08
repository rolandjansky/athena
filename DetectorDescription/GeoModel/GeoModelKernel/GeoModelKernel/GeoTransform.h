/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDDBC1B00ED.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDDBC1B00ED.cm

//## begin module%3CDDBC1B00ED.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDDBC1B00ED.cp

//## Module: GeoTransform%3CDDBC1B00ED; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoTransform.h

#ifndef GeoTransform_h
#define GeoTransform_h 1

//## begin module%3CDDBC1B00ED.additionalIncludes preserve=no
//## end module%3CDDBC1B00ED.additionalIncludes

//## begin module%3CDDBC1B00ED.includes preserve=yes
#include "CLHEP/Geometry/Transform3D.h"
//## end module%3CDDBC1B00ED.includes

// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3CDDBC1B00ED.additionalDeclarations preserve=yes
//## end module%3CDDBC1B00ED.additionalDeclarations


//## begin GeoTransform%3CDDBC1B00ED.preface preserve=yes
//## end GeoTransform%3CDDBC1B00ED.preface

//## Class: GeoTransform%3CDDBC1B00ED
//	A basic geometrical (Euclidean) transform.  Can return a
//	HepGeom::Transform3D.  Reference counted.  There is no
//	(mis)alignment present in this class.  For a transform
//	that can be  (mis)aligned, see GeoAlignableTransform.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoTransform : public GeoGraphNode  //## Inherits: <unnamed>%3CDDBCB3010A
{
  //## begin GeoTransform%3CDDBC1B00ED.initialDeclarations preserve=yes
  //## end GeoTransform%3CDDBC1B00ED.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoTransform%3CDE1F350127
      //	Constructor for GeoTransform.
      GeoTransform (const HepGeom::Transform3D &Transform);


    //## Other Operations (specified)
      //## Operation: getTransform%3CDE1C3B01CB
      //	Gets the total transformation.
      virtual HepGeom::Transform3D getTransform () const;

      //## Operation: getDefTransform%3CDE1D0E014D
      //	Gets the default transformation (no alignment
      //	correction).
      HepGeom::Transform3D getDefTransform () const;

      //## Operation: getDelta%3CDE1D1102F6
      //	Gets the delta for alignment.
      virtual const HepGeom::Transform3D * getDelta () const;

      //## Operation: exec%3CDF56E70216
      //	Executes a GeoNodeAction.
      virtual void exec (GeoNodeAction *action) const;

    // Additional Public Declarations
      //## begin GeoTransform%3CDDBC1B00ED.public preserve=yes
      //## end GeoTransform%3CDDBC1B00ED.public

  protected:
    //## Destructor (generated)
      virtual ~GeoTransform();

    // Additional Protected Declarations
      //## begin GeoTransform%3CDDBC1B00ED.protected preserve=yes
      //## end GeoTransform%3CDDBC1B00ED.protected

  private:
    //## Constructors (generated)
      GeoTransform(const GeoTransform &right);

    //## Assignment Operation (generated)
      GeoTransform & operator=(const GeoTransform &right);

    // Data Members for Class Attributes

      //## Attribute: m_transform%3CDE1BF802B5
      //	The Euclidean (Rigid Body) transform.
      //## begin GeoTransform::m_transform%3CDE1BF802B5.attr preserve=no  private: HepGeom::Transform3D {U} 
      HepGeom::Transform3D m_transform;
      //## end GeoTransform::m_transform%3CDE1BF802B5.attr

    // Additional Private Declarations
      //## begin GeoTransform%3CDDBC1B00ED.private preserve=yes
      //## end GeoTransform%3CDDBC1B00ED.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoTransform%3CDDBC1B00ED.implementation preserve=yes
      //## end GeoTransform%3CDDBC1B00ED.implementation

};

//## begin GeoTransform%3CDDBC1B00ED.postscript preserve=yes
//## end GeoTransform%3CDDBC1B00ED.postscript

// Class GeoTransform 

//## begin module%3CDDBC1B00ED.epilog preserve=yes
//## end module%3CDDBC1B00ED.epilog


#endif
