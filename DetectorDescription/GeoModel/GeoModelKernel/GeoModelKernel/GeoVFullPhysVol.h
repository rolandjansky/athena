/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD8F7603CA.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD8F7603CA.cm

//## begin module%3CDD8F7603CA.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CDD8F7603CA.cp

//## Module: GeoVFullPhysVol%3CDD8F7603CA; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVFullPhysVol.h

#ifndef GeoVFullPhysVol_h
#define GeoVFullPhysVol_h 1

//## begin module%3CDD8F7603CA.additionalIncludes preserve=no
//## end module%3CDD8F7603CA.additionalIncludes

//## begin module%3CDD8F7603CA.includes preserve=yes
//## end module%3CDD8F7603CA.includes

// GeoVPhysVol
#include "GeoModelKernel/GeoVPhysVol.h"
// GeoAbsPositionInfo
#include "GeoModelKernel/GeoAbsPositionInfo.h"
//## begin module%3CDD8F7603CA.additionalDeclarations preserve=yes
//## end module%3CDD8F7603CA.additionalDeclarations


//## begin GeoVFullPhysVol%3CDD8F7603CA.preface preserve=yes
//## end GeoVFullPhysVol%3CDD8F7603CA.preface

//## Class: GeoVFullPhysVol%3CDD8F7603CA; Abstract
//	A class of physical volumes whose default and aligned
//	global position is kept in cache and readily available,
//	without traversing.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoVFullPhysVol : public GeoVPhysVol  //## Inherits: <unnamed>%3CDD8FB600DD
{
  //## begin GeoVFullPhysVol%3CDD8F7603CA.initialDeclarations preserve=yes
  //## end GeoVFullPhysVol%3CDD8F7603CA.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoVFullPhysVol%3CDDA7F301DE
      //	Constructor for a VPhysVolume
      GeoVFullPhysVol (const GeoLogVol* LogVol);


    //## Other Operations (specified)
      //## Operation: getAbsoluteTransform%3CDE70FB0196
      //	Returns the absolute transform of the volume.
      const HepGeom::Transform3D & getAbsoluteTransform () const;

      //## Operation: clearPositionInfo%3CDE9AB9033A
      //	Clears the position information.  This can be used if
      //	the cache is determined to be invalid.  The usual client
      //	is one of the parents, upon changing its own position.
      //	There is little need for casual users to call this.
      void clearPositionInfo () const;

      //## Operation: getDefAbsoluteTransform%3CDE9CDF0359
      //	Returns the default absolute transform of the volume.
      const HepGeom::Transform3D & getDefAbsoluteTransform () const;

      //## Operation: getAbsoluteName%3CEB216903A2
      //	Returns the absolute name of this node.
      const std::string &  getAbsoluteName ();

      //## Operation: getId%3CEB24B3033F
      //	Returns the identification bits.
      unsigned int getId () const;

    // Additional Public Declarations
      //## begin GeoVFullPhysVol%3CDD8F7603CA.public preserve=yes
      //## end GeoVFullPhysVol%3CDD8F7603CA.public

  protected:
    //## Destructor (generated)
      virtual ~GeoVFullPhysVol();

    // Additional Protected Declarations
      //## begin GeoVFullPhysVol%3CDD8F7603CA.protected preserve=yes
      //## end GeoVFullPhysVol%3CDD8F7603CA.protected

  private:
    //## Constructors (generated)
      GeoVFullPhysVol(const GeoVFullPhysVol &right);

    //## Assignment Operation (generated)
      GeoVFullPhysVol & operator=(const GeoVFullPhysVol &right);

    // Data Members for Class Attributes

      //## Attribute: m_absName%3CEB2197011E
      //	The absolute name of this volume.
      //## begin GeoVFullPhysVol::m_absName%3CEB2197011E.attr preserve=no  private: std::string {U} 
      std::string m_absName;
      //## end GeoVFullPhysVol::m_absName%3CEB2197011E.attr

      //## Attribute: m_id%3CEB246401FB
      //	An identifier.  This is locally cached in a full
      //	physical volume.
      //## begin GeoVFullPhysVol::m_id%3CEB246401FB.attr preserve=no  private: mutable Query<int> * {U} 
      mutable Query<int> *m_id;
      //## end GeoVFullPhysVol::m_id%3CEB246401FB.attr

    // Additional Private Declarations
      //## begin GeoVFullPhysVol%3CDD8F7603CA.private preserve=yes
      //## end GeoVFullPhysVol%3CDD8F7603CA.private

  private: //## implementation
    // Data Members for Associations

      //## Association: GeoModelKernel::GraphNodes::Absolute Position%3CDD9595003E
      //## Role: GeoVFullPhysVol::m_absPosInfo%3CDD95950248
      //	Information on the where this volume is, by default and
      //	after alignment corrections.
      //## begin GeoVFullPhysVol::m_absPosInfo%3CDD95950248.role preserve=no  private: GeoAbsPositionInfo {1 -> 1RHgN}
      mutable GeoAbsPositionInfo *m_absPosInfo;
      //## end GeoVFullPhysVol::m_absPosInfo%3CDD95950248.role

    // Additional Implementation Declarations
      //## begin GeoVFullPhysVol%3CDD8F7603CA.implementation preserve=yes
      //## end GeoVFullPhysVol%3CDD8F7603CA.implementation

};

//## begin GeoVFullPhysVol%3CDD8F7603CA.postscript preserve=yes
//## end GeoVFullPhysVol%3CDD8F7603CA.postscript

// Class GeoVFullPhysVol 

//## begin module%3CDD8F7603CA.epilog preserve=yes
//## end module%3CDD8F7603CA.epilog


#endif
