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
  const std::string& sym = theEle->getSymbol();
  if (m_definedElements.find(sym) != m_definedElements.end())
    {
      return m_definedElements[sym];
    }
  elm = new G4Element(theEle->getName(),
                      sym,
                      theEle->getZ(),
                      theEle->getA());

  m_definedElements[sym]=elm;
  return elm;
}
