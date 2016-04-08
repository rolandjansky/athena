/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module.CLHEP::cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module.CLHEP::cm

//## begin module.cp preserve=no
//## end module.cp

//## Module: ForwardScrutinizer; Pseudo Package body
//## Subsystem: GeoModelExamples
//## Source file: L:\GEO\GeoModelExamples\ForwardScrutinizer.cxx


//## begin module.additionalIncludes preserve=no
//## end module.additionalIncludes

//## begin module.includes preserve=yes
#include "Identifier/Identifier.h"
//## end module.includes

// ForwardScrutinizer
#include "GeoModelExamples/ForwardScrutinizer.h"

//## begin module.additionalDeclarations preserve=yes
//## end module.additionalDeclarations


// Class ForwardScrutinizer 


ForwardScrutinizer::ForwardScrutinizer(const GeoVFullPhysVol *fullPhysVol)
  //## begin ForwardScrutinizer::ForwardScrutinizer%1021126961.hasinit preserve=no
  //## end ForwardScrutinizer::ForwardScrutinizer%1021126961.hasinit
  //## begin ForwardScrutinizer::ForwardScrutinizer%1021126961.initialization preserve=yes
  :GeoVDetectorElement(fullPhysVol)
  //## end ForwardScrutinizer::ForwardScrutinizer%1021126961.initialization
{
  //## begin ForwardScrutinizer::ForwardScrutinizer%1021126961.body preserve=yes
  //## end ForwardScrutinizer::ForwardScrutinizer%1021126961.body
}


ForwardScrutinizer::~ForwardScrutinizer()
{
  //## begin ForwardScrutinizer::~ForwardScrutinizer%.body preserve=yes
  //## end ForwardScrutinizer::~ForwardScrutinizer%.body
}



//## Other Operations (implementation)
Identifier ForwardScrutinizer::identify() const
{
  //## begin ForwardScrutinizer::identify%1053785956.body preserve=yes
  return Identifier();
  //## end ForwardScrutinizer::identify%1053785956.body
}


// Additional Declarations
//## begin ForwardScrutinizer.declarations preserve=yes
//## end ForwardScrutinizer.declarations




