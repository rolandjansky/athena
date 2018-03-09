/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4ElementFactory.h"
#include "GeoModelKernel/GeoElement.h"
#include "G4Element.hh"
#include <iostream>
#include <map>

Geo2G4ElementFactory::Geo2G4ElementFactory()
{
}

G4Element *Geo2G4ElementFactory::Build(const GeoElement* theEle)
{
  //
  // Check if this element has already been defined.
  //
  G4Element* elm;
  std::string sym = theEle->getSymbol();
  if (definedElements.find(sym) != definedElements.end())
    {
      return definedElements[sym];
    }
  elm = new G4Element(theEle->getName(),
                      sym,
                      theEle->getZ(),
                      theEle->getA());

  definedElements[sym]=elm;
  return elm;
}
