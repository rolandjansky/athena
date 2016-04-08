/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDDBC2403CB.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDDBC2403CB.cm

//## begin module%3CDDBC2403CB.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDDBC2403CB.cp

//## Module: GeoAlignableTransform%3CDDBC2403CB; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoAlignableTransform.h

#ifndef GeoAlignableTransform_h
#define GeoAlignableTransform_h 1

//## begin module%3CDDBC2403CB.additionalIncludes preserve=no
//## end module%3CDDBC2403CB.additionalIncludes

//## begin module%3CDDBC2403CB.includes preserve=yes
#include <memory>
//## end module%3CDDBC2403CB.includes

// GeoTransform
#include "GeoModelKernel/GeoTransform.h"
//## begin module%3CDDBC2403CB.additionalDeclarations preserve=yes
#include <vector>
#include "GeoModelKernel/GeoGraphNode.h"
//## end module%3CDDBC2403CB.additionalDeclarations


//## begin GeoAlignableTransform%3CDDBC2403CB.preface preserve=yes
//## end GeoAlignableTransform%3CDDBC2403CB.preface

//## Class: GeoAlignableTransform%3CDDBC2403CB
//	This class behaves just like a GeoTransform, except that
//	a misalignment can be added to the default transform.
//	In that case the getTransform() method returns the
//	product of the two transforms, with the default
//	transform left-multiplying the alignment delta.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoAlignableTransform : public GeoTransform  //## Inherits: <unnamed>%3CDDBC350235
{
  //## begin GeoAlignableTransform%3CDDBC2403CB.initialDeclarations preserve=yes
  //## end GeoAlignableTransform%3CDDBC2403CB.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoAlignableTransform%3CDE1FCE0285
      //	Constructor for GeoAlignableTransform.
      GeoAlignableTransform (const HepGeom::Transform3D &Transform);


    //## Other Operations (specified)
      //## Operation: getTransform%3CDE250E00CF
      //	Gets the total transform, including the alignment
      //	correction.
      virtual HepGeom::Transform3D getTransform () const;

      //## Operation: getDelta%3CDE1FCE0253
      //	Gets the delta for alignment.
      virtual const HepGeom::Transform3D * getDelta () const;

      //## Operation: setDelta%3CDE23E70157
      //	Sets an alignment delta.
      virtual void setDelta (const HepGeom::Transform3D &Delta);

      //## Operation: clearDelta%3CDE23ED014C
      //	Clears the alignment delta
      virtual void clearDelta ();

      //## Operation: dockTo%3CE3DA4700CE
      //	When a node is added to a parent in the graph, the node
      //	is always notified.  What happens at that time is up to
      //	the node.  Most nodes do not need to do anything.
      //	Some--the alignable transforms in particular--need to
      //	take some actions, such as adding the parent volume to a
      //	list.
      virtual void dockTo (GeoVPhysVol* parent);

    // Additional Public Declarations
      //## begin GeoAlignableTransform%3CDDBC2403CB.public preserve=yes
      //## end GeoAlignableTransform%3CDDBC2403CB.public

  protected:
    //## Destructor (generated)
      virtual ~GeoAlignableTransform();

    // Additional Protected Declarations
      //## begin GeoAlignableTransform%3CDDBC2403CB.protected preserve=yes
      //## end GeoAlignableTransform%3CDDBC2403CB.protected

  private:
    //## Constructors (generated)
      GeoAlignableTransform(const GeoAlignableTransform &right);

    //## Assignment Operation (generated)
      GeoAlignableTransform & operator=(const GeoAlignableTransform &right);

    // Data Members for Class Attributes

      //## Attribute: m_delta%3CDE208E0390
      //	Pointer to an alignment correction.  Until some
      //	alignment correction is set, this pointer is NULL and
      //	the memory is unallocated.
      //## begin GeoAlignableTransform::m_delta%3CDE208E0390.attr preserve=no  private: HepGeom::Transform3D * {U} 
      HepGeom::Transform3D *m_delta;
      //## end GeoAlignableTransform::m_delta%3CDE208E0390.attr

      //## Attribute: m_parentList%3CE3E84300E6
      //	A list of parents who use this alignable target.  They
      //	must all be notified when the alignment changes!
      //## begin GeoAlignableTransform::m_parentList%3CE3E84300E6.attr preserve=no  private: std::vector<GeoGraphNode *>  {U} 
      std::vector<GeoGraphNode *>  m_parentList;
      //## end GeoAlignableTransform::m_parentList%3CE3E84300E6.attr

    // Additional Private Declarations
      //## begin GeoAlignableTransform%3CDDBC2403CB.private preserve=yes
      //## end GeoAlignableTransform%3CDDBC2403CB.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoAlignableTransform%3CDDBC2403CB.implementation preserve=yes
      //## end GeoAlignableTransform%3CDDBC2403CB.implementation

};

//## begin GeoAlignableTransform%3CDDBC2403CB.postscript preserve=yes
//## end GeoAlignableTransform%3CDDBC2403CB.postscript

// Class GeoAlignableTransform 

//## begin module%3CDDBC2403CB.epilog preserve=yes
//## end module%3CDDBC2403CB.epilog


#endif
