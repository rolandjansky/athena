/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD8B7801FE.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD8B7801FE.cm

//## begin module%3CDD8B7801FE.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDD8B7801FE.cp

//## Module: GeoAbsPositionInfo%3CDD8B7801FE; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoAbsPositionInfo.h

#ifndef GeoAbsPositionInfo_h
#define GeoAbsPositionInfo_h 1

//## begin module%3CDD8B7801FE.additionalIncludes preserve=no
//## end module%3CDD8B7801FE.additionalIncludes

//## begin module%3CDD8B7801FE.includes preserve=yes
#include <CLHEP/Geometry/Transform3D.h>
//## end module%3CDD8B7801FE.includes

//## begin module%3CDD8B7801FE.additionalDeclarations preserve=yes
////class HepGeom::Transform3D;
//## end module%3CDD8B7801FE.additionalDeclarations


//## begin GeoAbsPositionInfo%3CDD8B7801FE.preface preserve=yes
//## end GeoAbsPositionInfo%3CDD8B7801FE.preface

//## Class: GeoAbsPositionInfo%3CDD8B7801FE
//	This class represents anything whose default and aligned
//	postions are known and cached.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoAbsPositionInfo 
{
  //## begin GeoAbsPositionInfo%3CDD8B7801FE.initialDeclarations preserve=yes
  //## end GeoAbsPositionInfo%3CDD8B7801FE.initialDeclarations

  public:
    //## Constructors (generated)
      GeoAbsPositionInfo();

    //## Destructor (generated)
      ~GeoAbsPositionInfo();


    //## Other Operations (specified)
      //## Operation: getAbsTransform%3CDD8CF402F5
      //	Returns the default absolute transform.
      const HepGeom::Transform3D * getAbsTransform () const;

      //## Operation: getDefAbsTransform%3CDD8D7F00E2
      //	Returns the default absolute transform.
      const HepGeom::Transform3D * getDefAbsTransform () const;

      //## Operation: clearAbsTransform%3CDE5FCD02C8
      //	Clears the absolute transform.
      void clearAbsTransform ();

      //## Operation: clearDefAbsTransform%3CDE5FD8006B
      //	Clears the default absolute transform.
      void clearDefAbsTransform ();

      //## Operation: setAbsTransform%3CDE60550300
      //	Sets the absolute transform.
      void setAbsTransform (const HepGeom::Transform3D &  xform);

      //## Operation: setDefAbsTransform%3CDE60B902F0
      //	Sets the absolute transform.
      void setDefAbsTransform (const HepGeom::Transform3D &  xform);

    // Additional Public Declarations
      //## begin GeoAbsPositionInfo%3CDD8B7801FE.public preserve=yes
      //## end GeoAbsPositionInfo%3CDD8B7801FE.public

  protected:
    // Additional Protected Declarations
      //## begin GeoAbsPositionInfo%3CDD8B7801FE.protected preserve=yes
      //## end GeoAbsPositionInfo%3CDD8B7801FE.protected

  private:
    //## Constructors (generated)
      GeoAbsPositionInfo(const GeoAbsPositionInfo &right);

    //## Assignment Operation (generated)
      GeoAbsPositionInfo & operator=(const GeoAbsPositionInfo &right);

    // Data Members for Class Attributes

      //## Attribute: m_absTransform%3CDE5D7E0001
      //	The absolute transform from the world coord down to this
      //	positioned object.
      //## begin GeoAbsPositionInfo::m_absTransform%3CDE5D7E0001.attr preserve=no  private: HepGeom::Transform3D * {U} NULL
      HepGeom::Transform3D *m_absTransform;
      //## end GeoAbsPositionInfo::m_absTransform%3CDE5D7E0001.attr

      //## Attribute: m_defAbsTransform%3CDE5D9202D1
      //	The default absolute transform from the world coord down
      //	to this positioned object.
      //## begin GeoAbsPositionInfo::m_defAbsTransform%3CDE5D9202D1.attr preserve=no  private: HepGeom::Transform3D * {U} NULL
      HepGeom::Transform3D *m_defAbsTransform;
      //## end GeoAbsPositionInfo::m_defAbsTransform%3CDE5D9202D1.attr

    // Additional Private Declarations
      //## begin GeoAbsPositionInfo%3CDD8B7801FE.private preserve=yes
      //## end GeoAbsPositionInfo%3CDD8B7801FE.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoAbsPositionInfo%3CDD8B7801FE.implementation preserve=yes
      //## end GeoAbsPositionInfo%3CDD8B7801FE.implementation

};

//## begin GeoAbsPositionInfo%3CDD8B7801FE.postscript preserve=yes
//## end GeoAbsPositionInfo%3CDD8B7801FE.postscript

// Class GeoAbsPositionInfo 


//## Other Operations (inline)
inline const HepGeom::Transform3D * GeoAbsPositionInfo::getAbsTransform () const
{
  //## begin GeoAbsPositionInfo::getAbsTransform%3CDD8CF402F5.body preserve=yes
  return m_absTransform;
  //## end GeoAbsPositionInfo::getAbsTransform%3CDD8CF402F5.body
}

inline const HepGeom::Transform3D * GeoAbsPositionInfo::getDefAbsTransform () const
{
  //## begin GeoAbsPositionInfo::getDefAbsTransform%3CDD8D7F00E2.body preserve=yes
  return m_defAbsTransform;
  //## end GeoAbsPositionInfo::getDefAbsTransform%3CDD8D7F00E2.body
}

//## begin module%3CDD8B7801FE.epilog preserve=yes
//## end module%3CDD8B7801FE.epilog


#endif
