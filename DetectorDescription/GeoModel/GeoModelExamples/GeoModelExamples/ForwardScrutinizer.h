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

//## Module: ForwardScrutinizer; Pseudo Package specification
//## Subsystem: GeoModelExamples
//## Source file: L:\GEO\GeoModelExamples\ForwardScrutinizer.h


#ifndef ForwardScrutinizer_h
#define ForwardScrutinizer_h 1

//## begin module.additionalIncludes preserve=no
//## end module.additionalIncludes

//## begin module.includes preserve=yes
//## end module.includes

// GeoVDetectorElement
#include "GeoModelKernel/GeoVDetectorElement.h"

//## begin module.additionalDeclarations preserve=yes
//## end module.additionalDeclarations


//## Class: ForwardScrutinizer
//## Category: GeoModelExamples
//## Subsystem: GeoModelExamples
//## Persistence: Transient
//## Cardinality/Multiplicity: n


class ForwardScrutinizer : public GeoVDetectorElement  //## Inherits: <unnamed>

{
  //## begin ForwardScrutinizer.initialDeclarations preserve=yes
  //## end ForwardScrutinizer.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: ForwardScrutinizer%1021126961
      //	Constructor for ForwardScrutinizer.  Connects readout
      //	geometry to material geometry.
      ForwardScrutinizer(const GeoVFullPhysVol *fullPhysVol);

    //## Destructor (generated)
      virtual ~ForwardScrutinizer();


    //## Other Operations (specified)
      //## Operation: identify%1053785956
      virtual Identifier identify() const;

    // Additional Public Declarations
      //## begin ForwardScrutinizer.public preserve=yes
      //## end ForwardScrutinizer.public


  protected:
    // Additional Protected Declarations
      //## begin ForwardScrutinizer.protected preserve=yes
      //## end ForwardScrutinizer.protected


  private:
    //## Constructors (generated)
      ForwardScrutinizer(const ForwardScrutinizer &right);

    //## Assignment Operation (generated)
      const ForwardScrutinizer & operator=(const ForwardScrutinizer &right);

    // Additional Private Declarations
      //## begin ForwardScrutinizer.private preserve=yes
      //## end ForwardScrutinizer.private


  private:  //## implementation
    // Data Members for Associations

      //## Association: Forward Readout Geometry%3CD3EE290300
      //## Role: ForwardScrutinizer::detector
      //## begin ForwardScrutinizer::detector.role preserve=no  public: ToyDetectorManager {n -> 1UHG}
      //## end ForwardScrutinizer::detector.role

    // Additional Implementation Declarations
      //## begin ForwardScrutinizer.implementation preserve=yes
      //## end ForwardScrutinizer.implementation

};

// Class ForwardScrutinizer 





#endif


