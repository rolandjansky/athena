/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module.CLHEP::cm

//## begin module.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module.cp

//## Module: CentralScrutinizer; Pseudo Package specification
//## Subsystem: GeoModelExamples
//## Source file: L:\GEO\GeoModelExamples\CentralScrutinizer.h


#ifndef CentralScrutinizer_h
#define CentralScrutinizer_h 1

//## begin module.additionalIncludes preserve=no
//## end module.additionalIncludes

//## begin module.includes preserve=yes
//## end module.includes

// GeoVDetectorElement
#include "GeoModelKernel/GeoVDetectorElement.h"

//## begin module.additionalDeclarations preserve=yes
//## end module.additionalDeclarations


//## Class: CentralScrutinizer
//## Category: GeoModelExamples
//## Subsystem: GeoModelExamples
//## Persistence: Transient
//## Cardinality/Multiplicity: n


class CentralScrutinizer : public GeoVDetectorElement  //## Inherits: <unnamed>

{
  //## begin CentralScrutinizer.initialDeclarations preserve=yes
  //## end CentralScrutinizer.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: CentralScrutinizer%1021126962
      //	Constructor for Central Scrutinizer.  Synchs readout
      //	geometry to material geometry.
      CentralScrutinizer(const GeoVFullPhysVol *fullPhysVol);

    //## Destructor (generated)
      virtual ~CentralScrutinizer();


    //## Other Operations (specified)
      //## Operation: identify%1053785955
      virtual Identifier identify() const;

    // Additional Public Declarations
      //## begin CentralScrutinizer.public preserve=yes
      //## end CentralScrutinizer.public


  protected:
    // Additional Protected Declarations
      //## begin CentralScrutinizer.protected preserve=yes
      //## end CentralScrutinizer.protected


  private:
    //## Constructors (generated)
      CentralScrutinizer(const CentralScrutinizer &right);

    //## Assignment Operation (generated)
      const CentralScrutinizer & operator=(const CentralScrutinizer &right);

    // Additional Private Declarations
      //## begin CentralScrutinizer.private preserve=yes
      //## end CentralScrutinizer.private


  private:  //## implementation
    // Data Members for Associations

      //## Association: Central Readout Geometry%3CDD4F94021D
      //## Role: CentralScrutinizer::detector
      //## begin CentralScrutinizer::detector.role preserve=no  public: ToyDetectorManager {n -> 1UHG}
      //## end CentralScrutinizer::detector.role

    // Additional Implementation Declarations
      //## begin CentralScrutinizer.implementation preserve=yes
      //## end CentralScrutinizer.implementation

};

// Class CentralScrutinizer 





#endif


