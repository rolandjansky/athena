/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module.CLHEP::cm

//## begin module.cp preserve=no
//## end module.cp

//## Module: CentralScrutinizer; Pseudo Package body
//## Subsystem: GeoModelExamples
//## Source file: L:\GEO\GeoModelExamples\CentralScrutinizer.cxx


//## begin module.additionalIncludes preserve=no
//## end module.additionalIncludes

//## begin module.includes preserve=yes
#include "Identifier/Identifier.h"
//## end module.includes

// CentralScrutinizer
#include "GeoModelExamples/CentralScrutinizer.h"

//## begin module.additionalDeclarations preserve=yes
//## end module.additionalDeclarations


// Class CentralScrutinizer 


CentralScrutinizer::CentralScrutinizer(const GeoVFullPhysVol *fullPhysVol)
  //## begin CentralScrutinizer::CentralScrutinizer%1021126962.hasinit preserve=no
  //## end CentralScrutinizer::CentralScrutinizer%1021126962.hasinit
  //## begin CentralScrutinizer::CentralScrutinizer%1021126962.initialization preserve=yes
  :GeoVDetectorElement(fullPhysVol)
  //## end CentralScrutinizer::CentralScrutinizer%1021126962.initialization
{
  //## begin CentralScrutinizer::CentralScrutinizer%1021126962.body preserve=yes
  //## end CentralScrutinizer::CentralScrutinizer%1021126962.body
}


CentralScrutinizer::~CentralScrutinizer()
{
  //## begin CentralScrutinizer::~CentralScrutinizer%.body preserve=yes
  //## end CentralScrutinizer::~CentralScrutinizer%.body
}



//## Other Operations (implementation)
Identifier CentralScrutinizer::identify() const
{
  //## begin CentralScrutinizer::identify%1053785955.body preserve=yes
  return Identifier();
  //## end CentralScrutinizer::identify%1053785955.body
}


// Additional Declarations
//## begin CentralScrutinizer.declarations preserve=yes
//## end CentralScrutinizer.declarations




