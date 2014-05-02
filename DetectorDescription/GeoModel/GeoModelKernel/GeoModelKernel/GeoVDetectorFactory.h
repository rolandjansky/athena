/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3E33EC05024E.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E33EC05024E.cm

//## begin module%3E33EC05024E.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3E33EC05024E.cp

//## Module: GeoVDetectorFactory%3E33EC05024E; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVDetectorFactory.h

#ifndef GeoVDetectorFactory_h
#define GeoVDetectorFactory_h 1

//## begin module%3E33EC05024E.additionalIncludes preserve=no
//## end module%3E33EC05024E.additionalIncludes

//## begin module%3E33EC05024E.includes preserve=yes
//## end module%3E33EC05024E.includes

// GeoVPhysVol
#include "GeoModelKernel/GeoVPhysVol.h"

class GeoPhysVol;
class GeoVDetectorManager;

//## begin module%3E33EC05024E.additionalDeclarations preserve=yes
//## end module%3E33EC05024E.additionalDeclarations


//## begin GeoVDetectorFactory%3E33EC05024E.preface preserve=yes
//## end GeoVDetectorFactory%3E33EC05024E.preface

//## Class: GeoVDetectorFactory%3E33EC05024E
//	Abstact Base Class for a detector factory.  The detector
//	factory builds the geometry and creates a GeoDetector
//	Manager, which one may thereafter query for all raw
//	geometry, and all readout geometry.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3CD4055502AF;GeoVPhysVol { -> }
//## Uses: <unnamed>%3E3426210264;GeoPhysVol { -> F}
//## Uses: <unnamed>%3E3426ED0109;GeoVDetectorManager { -> F}

class GeoVDetectorFactory 
{
  //## begin GeoVDetectorFactory%3E33EC05024E.initialDeclarations preserve=yes
  //## end GeoVDetectorFactory%3E33EC05024E.initialDeclarations

  public:
    //## Constructors (generated)
      GeoVDetectorFactory();

    //## Destructor (generated)
      virtual ~GeoVDetectorFactory();


    //## Other Operations (specified)
      //## Operation: create%3E33EC9901F7
      //	Create the system.
      virtual void create (GeoPhysVol* world) = 0;

      //## Operation: getDetectorManager%3E33F04C02A3
      //	Access to the Detector Manager.  When implementing this
      //	method, please use the covariant return type mechanism.
      virtual const GeoVDetectorManager* getDetectorManager () const = 0;

    // Additional Public Declarations
      //## begin GeoVDetectorFactory%3E33EC05024E.public preserve=yes
      //## end GeoVDetectorFactory%3E33EC05024E.public

  protected:
    // Additional Protected Declarations
      //## begin GeoVDetectorFactory%3E33EC05024E.protected preserve=yes
      //## end GeoVDetectorFactory%3E33EC05024E.protected

  private:
    //## Constructors (generated)
      GeoVDetectorFactory(const GeoVDetectorFactory &right);

    //## Assignment Operation (generated)
      GeoVDetectorFactory & operator=(const GeoVDetectorFactory &right);

    // Additional Private Declarations
      //## begin GeoVDetectorFactory%3E33EC05024E.private preserve=yes
      //## end GeoVDetectorFactory%3E33EC05024E.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoVDetectorFactory%3E33EC05024E.implementation preserve=yes
      //## end GeoVDetectorFactory%3E33EC05024E.implementation

};

//## begin GeoVDetectorFactory%3E33EC05024E.postscript preserve=yes
//## end GeoVDetectorFactory%3E33EC05024E.postscript

// Class GeoVDetectorFactory 

//## begin module%3E33EC05024E.epilog preserve=yes
//## end module%3E33EC05024E.epilog


#endif
