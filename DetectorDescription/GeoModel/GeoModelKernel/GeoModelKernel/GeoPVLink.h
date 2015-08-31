/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D13DDFD02C8.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D13DDFD02C8.cm

//## begin module%3D13DDFD02C8.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3D13DDFD02C8.cp

//## Module: GeoPVLink%3D13DDFD02C8; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPVLink.h

#ifndef GeoPVLink_h
#define GeoPVLink_h 1

//## begin module%3D13DDFD02C8.additionalIncludes preserve=no
//## end module%3D13DDFD02C8.additionalIncludes

//## begin module%3D13DDFD02C8.includes preserve=yes
//## end module%3D13DDFD02C8.includes

// GeoPVConstLink
#include "GeoModelKernel/GeoPVConstLink.h"
//## begin module%3D13DDFD02C8.additionalDeclarations preserve=yes
//## end module%3D13DDFD02C8.additionalDeclarations


//## begin GeoPVLink%3D13DDFD02C8.preface preserve=yes
//## end GeoPVLink%3D13DDFD02C8.preface

//## Class: GeoPVLink%3D13DDFD02C8
//	Smart pointer to a GeoVPhysVol...non-constant
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPVLink : public GeoPVConstLink  //## Inherits: <unnamed>%3D13DE0A01F4
{
  //## begin GeoPVLink%3D13DDFD02C8.initialDeclarations preserve=yes
  //## end GeoPVLink%3D13DDFD02C8.initialDeclarations

  public:
    //## Constructors (generated)
      GeoPVLink();

      GeoPVLink(const GeoPVLink &right);

    //## Constructors (specified)
      //## Operation: GeoPVLink%3D14067D024B
      GeoPVLink (const GeoVPhysVol  *addr);

    //## Destructor (generated)
      virtual ~GeoPVLink();


    //## Other Operations (specified)
      //## Operation: operator *%3D13DE1B0311
      //	Dereference this smart pointer, like a regular pointer.
      virtual GeoVPhysVol & operator * ();

      //## Operation: operator->%3D13DE1B032F
      //	Allow use of smartPointer->operation().....
      virtual GeoVPhysVol * operator -> ();

      //## Operation: operator *%3D140E0001EC
      //	Dereference this smart pointer, like a regular pointer.
      virtual const GeoVPhysVol & operator * () const;

      //## Operation: operator->%3D140E000214
      //	Allow use of smartPointer->operation().....
      virtual const GeoVPhysVol * operator -> () const;

      //## Operation: operator=%3D1DC1D202EA
      //	Assignment operator.
      GeoPVLink& operator = (const GeoPVLink& right);

    // Additional Public Declarations
      //## begin GeoPVLink%3D13DDFD02C8.public preserve=yes
      //## end GeoPVLink%3D13DDFD02C8.public

  protected:
    // Additional Protected Declarations
      //## begin GeoPVLink%3D13DDFD02C8.protected preserve=yes
      //## end GeoPVLink%3D13DDFD02C8.protected

  private:
    // Additional Private Declarations
      //## begin GeoPVLink%3D13DDFD02C8.private preserve=yes
      //## end GeoPVLink%3D13DDFD02C8.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPVLink%3D13DDFD02C8.implementation preserve=yes
      //## end GeoPVLink%3D13DDFD02C8.implementation

};

//## begin GeoPVLink%3D13DDFD02C8.postscript preserve=yes
//## end GeoPVLink%3D13DDFD02C8.postscript

// Class GeoPVLink 

//## begin module%3D13DDFD02C8.epilog preserve=yes
//## end module%3D13DDFD02C8.epilog


#endif
