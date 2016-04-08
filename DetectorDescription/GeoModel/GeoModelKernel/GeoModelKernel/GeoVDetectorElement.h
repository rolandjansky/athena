/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDDAEF8001F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDDAEF8001F.cm

//## begin module%3CDDAEF8001F.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDDAEF8001F.cp

//## Module: GeoVDetectorElement%3CDDAEF8001F; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVDetectorElement.h

#ifndef GeoVDetectorElement_h
#define GeoVDetectorElement_h 1

//## begin module%3CDDAEF8001F.additionalIncludes preserve=no
//## end module%3CDDAEF8001F.additionalIncludes

//## begin module%3CDDAEF8001F.includes preserve=yes
//## end module%3CDDAEF8001F.includes

// GeoVFullPhysVol
#include "GeoModelKernel/GeoVFullPhysVol.h"
//## begin module%3CDDAEF8001F.additionalDeclarations preserve=yes
class Identifier;
//## end module%3CDDAEF8001F.additionalDeclarations


//## begin GeoVDetectorElement%3CDDAEF8001F.preface preserve=yes
//## end GeoVDetectorElement%3CDDAEF8001F.preface

//## Class: GeoVDetectorElement%3CDDAEF8001F; Abstract
//	A GeoVDetectorElement is a piece of readout geometry.
//	This is an abstract base class for readout objects which
//	have a distinct association to a piece of physical
//	geometry and whose position we track by specifying a
//	Euclidean transformation w.r.t global coordinates. level
//	that one can keep track of with Euclidean
//	transformations.  Examples:
//
//	* silicon wafers (not strips or pixels!)
//	* calorimeter modules (not cells!)
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoVDetectorElement 
{
  //## begin GeoVDetectorElement%3CDDAEF8001F.initialDeclarations preserve=yes
  //## end GeoVDetectorElement%3CDDAEF8001F.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoVDetectorElement%3CDDB11202FE
      GeoVDetectorElement (const GeoVFullPhysVol* fullPhysVol);

    //## Destructor (generated)
      virtual ~GeoVDetectorElement();


    //## Other Operations (specified)
      //## Operation: getMaterialGeom%3CE7FDA50076
      //	Returns a pointer to a physical volume which is
      //	associated with this detector element.
      const GeoVFullPhysVol* getMaterialGeom () const;

    // Additional Public Declarations
      //## begin GeoVDetectorElement%3CDDAEF8001F.public preserve=yes
      //## end GeoVDetectorElement%3CDDAEF8001F.public

  protected:
    // Additional Protected Declarations
      //## begin GeoVDetectorElement%3CDDAEF8001F.protected preserve=yes
      //## end GeoVDetectorElement%3CDDAEF8001F.protected

  private:
    //## Constructors (generated)
      GeoVDetectorElement(const GeoVDetectorElement &right);

    //## Assignment Operation (generated)
      GeoVDetectorElement & operator=(const GeoVDetectorElement &right);

    // Additional Private Declarations
      //## begin GeoVDetectorElement%3CDDAEF8001F.private preserve=yes
      //## end GeoVDetectorElement%3CDDAEF8001F.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::Subsystem::Material/Readout Synch%3CDDB2A7015C
      //## Role: GeoVDetectorElement::m_materialGeom%3CDDB2A80095
      //## begin GeoVDetectorElement::m_materialGeom%3CDDB2A80095.role preserve=no  public: GeoVFullPhysVol {1 -> 1RHgN}
      const GeoVFullPhysVol *m_materialGeom;
      //## end GeoVDetectorElement::m_materialGeom%3CDDB2A80095.role

    // Additional Implementation Declarations
      //## begin GeoVDetectorElement%3CDDAEF8001F.implementation preserve=yes
      //## end GeoVDetectorElement%3CDDAEF8001F.implementation

};

//## begin GeoVDetectorElement%3CDDAEF8001F.postscript preserve=yes
//## end GeoVDetectorElement%3CDDAEF8001F.postscript

// Class GeoVDetectorElement 


//## Other Operations (inline)
inline const GeoVFullPhysVol* GeoVDetectorElement::getMaterialGeom () const
{
  //## begin GeoVDetectorElement::getMaterialGeom%3CE7FDA50076.body preserve=yes
  return m_materialGeom;
  //## end GeoVDetectorElement::getMaterialGeom%3CE7FDA50076.body
}

//## begin module%3CDDAEF8001F.epilog preserve=yes
//## end module%3CDDAEF8001F.epilog


#endif
